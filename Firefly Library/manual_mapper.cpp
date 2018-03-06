#include "manual_mapper.hpp"

#include <TlHelp32.h>

#define stub_size 256

#pragma pack(push, 1)
struct execute_stub_data
{
	unsigned char* entrypoint;
	unsigned char* image_base;
};
#pragma pack(pop)

namespace execution_stub
{
	__declspec(naked) unsigned int __stdcall entry_point_stub(execute_stub_data* data)
	{
		__asm
		{
			push ebp
			mov ebp,esp
			push ebx
			mov ebx,[ebp+0x08]

			push 00
			push DLL_PROCESS_ATTACH
			push [ebx+0x04]
			call [ebx]

			pop ebx
			mov esp,ebp
			pop ebp
			ret 0004
		}
	}
}

namespace firefly
{
	manual_mapper::manual_mapper()
		: windows_injector()
	{

	}

	manual_mapper::~manual_mapper()
	{

	}

	bool manual_mapper::manual_map_memory_file(io::file_mapper& file_map, int process_id, bool exclude_header, thread_inject_api thread_api)
	{
		HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);

		if (!process)
			return false;
	
		dos_header* dos_headers = reinterpret_cast<dos_header*>(file_map.data());
		nt_header* nt_headers = reinterpret_cast<nt_header*>(file_map.data() + dos_headers->pe_offset);
	
		if (nt_headers->optional_header.section_alignment & 1)
		{
			CloseHandle(process);
			return false;
		}

		unsigned char* image_base = this->allocate_image_base(process, nt_headers);

		if (!image_base)
		{
			CloseHandle(process);
			return false;
		}

		if (!exclude_header)
		{
			if (!this->write_headers(process, nt_headers, image_base, file_map))
			{
				VirtualFreeEx(process, image_base, nt_headers->optional_header.image_size, MEM_RELEASE);
				CloseHandle(process);
				return false;
			}
		}
	
		if (!this->write_sections(process, nt_headers, image_base, file_map) ||
			!this->build_import_table(process, nt_headers, image_base, file_map, thread_api) ||
			!this->relocate_base(process, nt_headers, image_base, file_map) ||
			!this->set_page_permissions(process, nt_headers, image_base) ||
			!this->execute_tls_entrypoints(process, nt_headers, image_base, thread_api) ||
			!this->execute_entrypoint(process, nt_headers, image_base, thread_api))
		{
			VirtualFreeEx(process, image_base, nt_headers->optional_header.image_size, MEM_RELEASE);
			CloseHandle(process);
			return false;
		}
	
		CloseHandle(process);
		return true;
	}

	unsigned char* manual_mapper::allocate_image_base(HANDLE process, nt_header* nt_headers)
	{
		void* image_base = VirtualAllocEx(process, reinterpret_cast<void*>(nt_headers->optional_header.image_base), nt_headers->optional_header.image_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

		if (!image_base)
		{	
			image_base = VirtualAllocEx(process, NULL, nt_headers->optional_header.image_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		}

		return reinterpret_cast<unsigned char*>(image_base);
	}

	unsigned char* manual_mapper::allocate_manual_func(HANDLE process, unsigned char* destination, void* source, std::size_t size)
	{
		void* alloc = VirtualAllocEx(process, destination, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

		if (!alloc)
			return nullptr;

		if (!WriteProcessMemory(process, destination, source, size, NULL))
		{
			VirtualFree(alloc, size, MEM_DECOMMIT);
			return nullptr;
		}

		return reinterpret_cast<unsigned char*>(alloc);
	}

	section_header* manual_mapper::get_rva_section(unsigned int rva, nt_header* nt_headers)
	{
		unsigned int optional_header_offset = reinterpret_cast<unsigned int>(&reinterpret_cast<nt_header*>(0)->optional_header);
		unsigned int optional_header_size = nt_headers->file_header.optional_header_size;

		section_header* section = reinterpret_cast<section_header*>(reinterpret_cast<unsigned char*>(nt_headers) + optional_header_offset + optional_header_size);

		for (std::size_t i = 0; i < nt_headers->file_header.section_count; i++, section++)
			if (rva >= section->virtual_address && rva < section->virtual_address + section->misc.virtual_size)
				return section;

		return nullptr;
	}

	unsigned int manual_mapper::get_rva_offset(unsigned int rva, nt_header* nt_headers)
	{
		section_header* section = this->get_rva_section(rva, nt_headers);
		return (section != nullptr ? (rva + (section->raw_data_pointer - section->virtual_address)) : 0);
	}

	bool manual_mapper::write_headers(HANDLE process, nt_header* nt_headers, unsigned char* image_base, io::file_mapper& file_map)
	{
		void* headers = VirtualAllocEx(process, image_base, nt_headers->optional_header.headers_size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

		if (!headers)
			return false;

		return (WriteProcessMemory(process, headers, file_map.data(), nt_headers->optional_header.headers_size, NULL) != FALSE);
	}

	bool manual_mapper::write_sections(HANDLE process, nt_header* nt_headers, unsigned char* image_base, io::file_mapper& file_map)
	{
		SYSTEM_INFO system_info;
		memset(&system_info, 0, sizeof(SYSTEM_INFO));

		GetNativeSystemInfo(&system_info);
	
		unsigned int optional_header_offset = reinterpret_cast<unsigned int>(&reinterpret_cast<nt_header*>(0)->optional_header);
		unsigned int optional_header_size = nt_headers->file_header.optional_header_size;

		section_header* section = reinterpret_cast<section_header*>(reinterpret_cast<unsigned char*>(nt_headers) + optional_header_offset + optional_header_size);

		for (std::size_t i = 0; i < nt_headers->file_header.section_count; i++, section++)
		{
			if (section->characteristics & IMAGE_SCN_MEM_DISCARDABLE)
				continue;

			if (!section->raw_data_size)
			{
				unsigned int alignment = nt_headers->optional_header.section_alignment;

				if (alignment > 0)
				{
					void* section_base = VirtualAllocEx(process, image_base + section->virtual_address, alignment, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
				
					if (!section_base)
						return false;

					unsigned char* zero_buffer = new unsigned char[alignment];
					memset(zero_buffer, 0, alignment);

					if (!WriteProcessMemory(process, section_base, zero_buffer, alignment, NULL))
					{
						VirtualFreeEx(process, image_base + section->virtual_address, alignment, MEM_DECOMMIT);
						delete[] zero_buffer;
						return false;
					}
				
					delete[] zero_buffer;
				}
			}
			else
			{
				std::size_t section_min_size = (section->raw_data_size < section->misc.virtual_size ? section->raw_data_size : section->misc.virtual_size);
			
				if (section_min_size > 0)
				{
					void* section_base = VirtualAllocEx(process, image_base + section->virtual_address, section->misc.virtual_size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
				
					if (!section_base)
						return false;
			
					if (!WriteProcessMemory(process, section_base, file_map.data() + section->raw_data_pointer, section_min_size, NULL))
					{
						VirtualFreeEx(process, image_base + section->virtual_address, section->misc.virtual_size, MEM_DECOMMIT);
						return false;
					}
				}
			}
		}
	
		return true;
	}

	bool manual_mapper::build_import_table(HANDLE process, nt_header* nt_headers, unsigned char* image_base, io::file_mapper& file_map, thread_inject_api thread_api)
	{
		const unsigned int ordinal_flag_32 = 0x80000000;
	
		if (!nt_headers->optional_header.data_directory[directory_entry_import].virtual_address)
			return false;

		for (import_descriptor* descriptor = reinterpret_cast<import_descriptor*>(file_map.data() + this->get_rva_offset(nt_headers->optional_header.data_directory[directory_entry_import].virtual_address, nt_headers)); 
			descriptor->characteristics != 0; descriptor++)
		{
			thunk_data* INT = reinterpret_cast<thunk_data*>(file_map.data() + this->get_rva_offset(descriptor->original_first_thunk, nt_headers));
			thunk_data* IAT = reinterpret_cast<thunk_data*>(image_base + descriptor->first_thunk);

			HMODULE module = this->load_remote_library(process, reinterpret_cast<char*>(file_map.data()) + this->get_rva_offset(descriptor->name, nt_headers), thread_api);

			if (!module)
				return false;
		
			for (unsigned int i = 0; INT[i].function != 0; i++)
			{
				unsigned int import_name_ordinal = 0;

				if (INT[i].ordinal & ordinal_flag_32)
					import_name_ordinal = (INT[i].ordinal & 0xFFFF);
				else
					import_name_ordinal = reinterpret_cast<unsigned int>(reinterpret_cast<import_by_name*>(file_map.data() + this->get_rva_offset(INT[i].function, nt_headers))->name);

				FARPROC proc_address = this->get_remote_proc_address(process, module, reinterpret_cast<const char*>(import_name_ordinal));
			
				if (!proc_address)
					return false;

				if (!WriteProcessMemory(process, &IAT[i], &proc_address, sizeof(FARPROC), NULL))
					return false;
			}
		}

		return true;
	}

	bool manual_mapper::relocate_base(HANDLE process, nt_header* nt_headers, unsigned char* image_base, io::file_mapper& file_map)
	{
		const unsigned int relocation_based_absolute = 0;
		const unsigned int relocation_based_highlow = 3;

		if (reinterpret_cast<unsigned int>(image_base) != nt_headers->optional_header.image_base)
		{
			if (!nt_headers->optional_header.data_directory[directory_entry_basereloc].virtual_address)
				return false;

			unsigned int delta = reinterpret_cast<unsigned int>(image_base - nt_headers->optional_header.image_base);

			base_relocation* base_reloc = reinterpret_cast<base_relocation*>(file_map.data() + this->get_rva_offset(nt_headers->optional_header.data_directory[directory_entry_basereloc].virtual_address, nt_headers));

			for (base_relocation* reloc = base_reloc; reinterpret_cast<unsigned int>(reloc) - reinterpret_cast<unsigned int>(base_reloc) < nt_headers->optional_header.data_directory[directory_entry_basereloc].size;
				reloc = reinterpret_cast<base_relocation*>(reinterpret_cast<unsigned char*>(reloc) + reloc->block_size))
			{
				if (reloc->block_size < sizeof(base_relocation))
					continue;

				unsigned char* base = image_base + reloc->virtual_address;
				unsigned short* offsets = reinterpret_cast<unsigned short*>(reinterpret_cast<unsigned char*>(reloc) + sizeof(base_relocation));

				for (unsigned int i = 0; i < ((reloc->block_size - sizeof(base_relocation)) / sizeof(unsigned short)); i++)
				{
					if (!offsets[i])
						continue;

					const unsigned int reloc_type = (offsets[i] >> 12);
					const unsigned int destination = (offsets[i] & 0x0FFF);
					
					if (reloc_type == relocation_based_highlow)
					{
						unsigned int reloc_data = 0;

						if (!ReadProcessMemory(process, base + destination, &reloc_data, sizeof(unsigned int), NULL))
							return false;

						reloc_data += delta;

						if (!WriteProcessMemory(process, base + destination, &reloc_data, sizeof(unsigned int), NULL))
							return false;
					}
					else if (reloc_type == relocation_based_absolute)
					{
						/* Do nothing ... */
					}
				}
			}
		}

		return true;
	}

	bool manual_mapper::set_page_permissions(HANDLE process, nt_header* nt_headers, unsigned char* image_base)
	{
		unsigned int optional_header_offset = reinterpret_cast<unsigned int>(&reinterpret_cast<nt_header*>(0)->optional_header);
		unsigned int optional_header_size = nt_headers->file_header.optional_header_size;

		section_header* section = reinterpret_cast<section_header*>(reinterpret_cast<unsigned char*>(nt_headers) + optional_header_offset + optional_header_size);

		for (std::size_t i = 0; i < nt_headers->file_header.section_count; i++, section++)
		{
			if (section->characteristics & IMAGE_SCN_MEM_DISCARDABLE)
				continue;

			unsigned long protection = 0;
		
			if (section->characteristics & IMAGE_SCN_MEM_EXECUTE)
				protection = PAGE_EXECUTE;
		
			if (section->characteristics & IMAGE_SCN_MEM_READ)
				protection = PAGE_READONLY;
		
			if (section->characteristics & IMAGE_SCN_MEM_WRITE)
				protection = PAGE_READWRITE;
		
			if ((section->characteristics & IMAGE_SCN_MEM_EXECUTE) && protection == PAGE_READWRITE)
				protection = PAGE_EXECUTE_READWRITE;

			if ((section->characteristics & IMAGE_SCN_MEM_EXECUTE) && protection == PAGE_READONLY)
				protection = PAGE_EXECUTE_READ;

			if (!VirtualProtectEx(process, image_base + section->virtual_address, section->misc.virtual_size, protection, &protection))
				return false;
		}

		return true;
	}

	bool manual_mapper::execute_tls_entrypoints(HANDLE process, nt_header* nt_headers, unsigned char* image_base, thread_inject_api thread_api)
	{
		if (!nt_headers->optional_header.data_directory[directory_entry_tls].size)
			return true;
	
		tls_directory tls_dir;

		if (!ReadProcessMemory(process, image_base + nt_headers->optional_header.data_directory[directory_entry_tls].virtual_address, &tls_dir, sizeof(tls_directory), NULL))
			return false;

		tls_callback_t* callback_address = reinterpret_cast<tls_callback_t*>(tls_dir.callback_address);

		if (callback_address)
		{
			unsigned char* callback = nullptr;

			do
			{
				if (!ReadProcessMemory(process, callback_address++, &callback, sizeof(unsigned char*), NULL))
					return false;

				if (callback)
				{
					unsigned char* alloc = reinterpret_cast<unsigned char*>(VirtualAllocEx(process, NULL, sizeof(execute_stub_data) + stub_size, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE));

					if (!alloc)
						return false;

					execute_stub_data stub_data;
					stub_data.entrypoint = callback;
					stub_data.image_base = image_base;

					if (!WriteProcessMemory(process, alloc, &stub_data, sizeof(execute_stub_data), NULL) ||	!WriteProcessMemory(process, alloc + sizeof(execute_stub_data), execution_stub::entry_point_stub, stub_size, NULL))
					{
						VirtualFreeEx(process, alloc, 0, MEM_RELEASE);
						return false;
					}

					HANDLE thread_handle = NULL;

					if (!this->create_thread(thread_api, process, alloc + sizeof(execute_stub_data), alloc, &thread_handle))
					{
						VirtualFreeEx(process, alloc, 0, MEM_RELEASE);
						return false;
					}

					if (WaitForSingleObject(thread_handle, INFINITE) != WAIT_FAILED)
						VirtualFreeEx(process, alloc, 0, MEM_RELEASE);
				}
			}
			while (callback != nullptr);
		}

		return true;
	}

	bool manual_mapper::execute_entrypoint(HANDLE process, nt_header* nt_headers, unsigned char* image_base, thread_inject_api thread_api)
	{
		if (!nt_headers->optional_header.entry_point_address)
			return false;

		unsigned char* alloc = reinterpret_cast<unsigned char*>(VirtualAllocEx(process, NULL, sizeof(execute_stub_data) + stub_size, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE));

		if (!alloc)
			return false;
	
		execute_stub_data stub_data;
		stub_data.entrypoint = reinterpret_cast<unsigned char*>(image_base + nt_headers->optional_header.entry_point_address);
		stub_data.image_base = image_base;

		if (!WriteProcessMemory(process, alloc, &stub_data, sizeof(execute_stub_data), NULL) ||	!WriteProcessMemory(process, alloc + sizeof(execute_stub_data), execution_stub::entry_point_stub, stub_size, NULL))
		{
			VirtualFreeEx(process, alloc, 0, MEM_RELEASE);
			return false;
		}

		HANDLE thread_handle = NULL;

		if (!this->create_thread(thread_api, process, alloc + sizeof(execute_stub_data), alloc, &thread_handle))
		{
			VirtualFreeEx(process, alloc, 0, MEM_RELEASE);
			return false;
		}
	
		if (WaitForSingleObject(thread_handle, INFINITE) != WAIT_FAILED)
			VirtualFreeEx(process, alloc, 0, MEM_RELEASE);

		return true;
	}

	HMODULE manual_mapper::get_remote_library(HANDLE process, std::string const& library_name)
	{
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetProcessId(process));

		if (snapshot == INVALID_HANDLE_VALUE)
			return NULL;

		MODULEENTRY32 me32;
		memset(&me32, 0, sizeof(MODULEENTRY32));

		me32.dwSize = sizeof(MODULEENTRY32);

		if (Module32First(snapshot, &me32))
		{
			do
			{
				if (!lstrcmpi(me32.szModule, library_name.c_str()))
				{
					CloseHandle(snapshot);
					return reinterpret_cast<HMODULE>(me32.modBaseAddr);
				}
			}
			while (Module32Next(snapshot, &me32));
		}

		CloseHandle(snapshot);
		return NULL;
	}

	HMODULE manual_mapper::load_remote_library(HANDLE process, std::string const& library_name, thread_inject_api thread_api)
	{
		HANDLE thread_handle = NULL;
	
		if (!this->inject_file(library_name, process, windows_inject_api::api_load_library_a, thread_api, &thread_handle))
			return NULL;

		if (!thread_handle)
			return NULL;

		if (WaitForSingleObject(thread_handle, INFINITE) == WAIT_FAILED)
		{
			CloseHandle(thread_handle);
			return NULL;
		}

		HMODULE module = NULL;

		if (!GetExitCodeThread(thread_handle, reinterpret_cast<DWORD*>(&module)))
		{
			CloseHandle(thread_handle);
			return NULL;
		}

		CloseHandle(thread_handle);
		return module;
	}

	FARPROC manual_mapper::get_remote_proc_address(HANDLE process, HMODULE module, const char* proc_name)
	{
		dos_header dos_headers;
	
		if (!ReadProcessMemory(process, reinterpret_cast<unsigned char*>(module), &dos_headers, sizeof(dos_header), NULL))
			return nullptr;
	
		if (dos_headers.magic != 'ZM')
			return nullptr;
	
		nt_header nt_headers;

		if (!ReadProcessMemory(process, reinterpret_cast<unsigned char*>(module) + dos_headers.pe_offset, &nt_headers, sizeof(nt_header), NULL))
			return nullptr;
	
		if (nt_headers.file_header.signature != 'EP')
			return nullptr;
	
		export_directory export_dir;

		if (!ReadProcessMemory(process, reinterpret_cast<unsigned char*>(module) + nt_headers.optional_header.data_directory[directory_entry_export].virtual_address, &export_dir, sizeof(export_directory), NULL))
			return nullptr;

		unsigned int* function_rvas = new unsigned int[export_dir.function_count];

		if (!ReadProcessMemory(process, reinterpret_cast<unsigned char*>(module) + export_dir.functions_address, function_rvas, export_dir.function_count * sizeof(unsigned int), NULL))
		{
			delete[] function_rvas;
			return nullptr;
		}

		unsigned int rva_buffer = 0;
	
		if (HIWORD(proc_name))
		{
			unsigned int* name_rvas = new unsigned int[export_dir.name_count];
		
			if (!ReadProcessMemory(process, reinterpret_cast<unsigned char*>(module) + export_dir.names_address, name_rvas, export_dir.name_count * sizeof(unsigned int), NULL))
			{
				delete[] name_rvas;
				delete[] function_rvas;
				return nullptr;
			}

			unsigned short* name_ordinal_rvas = new unsigned short[export_dir.name_count];
		
			if (!ReadProcessMemory(process, reinterpret_cast<unsigned char*>(module) + export_dir.name_ordinals_address, name_ordinal_rvas, export_dir.name_count * sizeof(unsigned short), NULL))
			{
				delete[] name_ordinal_rvas;
				delete[] name_rvas;
				delete[] function_rvas;
				return nullptr;
			}

			char* name_buffer = new char[strlen(proc_name) + 1];

			for (unsigned int i = 0; i < export_dir.name_count; i++)
			{
				if (!ReadProcessMemory(process, reinterpret_cast<unsigned char*>(module) + name_rvas[i], name_buffer, strlen(proc_name) + 1, NULL))
				{
					delete[] name_ordinal_rvas;
					delete[] name_rvas;
					delete[] function_rvas;
					return nullptr;
				}

				if (!strcmp(name_buffer, proc_name))
					rva_buffer = function_rvas[name_ordinal_rvas[i]];
			}
		
			delete[] name_ordinal_rvas;
			delete[] name_rvas;
		}
		else
		{
			rva_buffer = function_rvas[reinterpret_cast<DWORD>(proc_name) - export_dir.base];
		}

		delete[] function_rvas;

		if (!rva_buffer)
			return nullptr;

		if (rva_buffer >= nt_headers.optional_header.data_directory[directory_entry_export].virtual_address && 
			rva_buffer < nt_headers.optional_header.data_directory[directory_entry_export].virtual_address + nt_headers.optional_header.data_directory[directory_entry_export].size)
		{
			char forward_buffer[100];

			if (!ReadProcessMemory(process, reinterpret_cast<unsigned char*>(module) + rva_buffer, forward_buffer, sizeof(forward_buffer), NULL))
				return nullptr;

			char* dot_offset = strrchr(forward_buffer, '.');

			std::string library_name = std::string(forward_buffer, dot_offset - forward_buffer) + std::string(".dll");
			std::string api_name(dot_offset + 1);

			HMODULE library_module = this->get_remote_library(process, library_name);

			if (!library_module)
				return nullptr;

			return this->get_remote_proc_address(process, library_module, api_name.c_str());
		}
		else
		{
			return reinterpret_cast<FARPROC>(reinterpret_cast<unsigned char*>(module) + rva_buffer);
		}

		return nullptr;
	}
}