#include "game_bypass.hpp"
#include "detours.hpp"

#include "pe_structs.hpp"

#include <winternl.h>

#pragma comment(lib, "ntdll")
#pragma comment(lib, "version")
#include <string>

namespace firefly
{
	static unsigned char* image_base = nullptr;
	static unsigned char* image_end = nullptr;
	static unsigned char* image_copy = nullptr;
	
	static unsigned int Crc32_GetCrc32_VMCRC = 0x02FC0379;
	static unsigned int Crc32_GetCrc32_VMCRC_check = Crc32_GetCrc32_VMCRC;

	static unsigned int Crc32_GetCrc32_VMTable = 0x02CEA32C;
	
	void __declspec(naked) Crc32_GetCrc32_VMCRC_hook()
	{
		__asm
		{
			cmp ebx,[image_base]
			jb nobypass
			cmp ebx,[image_end]
			ja nobypass
			
			sub ebx,[image_base]
			add ebx,[image_copy]

			nobypass:         
			jmp [Crc32_GetCrc32_VMCRC]
		}
	}

	void __declspec(naked) Crc32_GetCrc32_VMTable_hook()
	{
		__asm                                    
		{
			push ecx
			mov ecx,[Crc32_GetCrc32_VMCRC_check]
			sub ecx,0x10
			cmp edi,ecx
			jb nobypass
			add ecx,0x20
			cmp edi,ecx
			ja nobypass

			sub edi,[image_base]
			add edi,[image_copy]

			nobypass:
			pop ecx
			jmp [Crc32_GetCrc32_VMTable]
		}
	}

	bool set_image_base()
	{
		PEB* peb = reinterpret_cast<TEB*>(__readfsdword(PcTeb))->ProcessEnvironmentBlock;
		
		/* Set PEB->ImageBaseAddress to the image_copy */
		peb->Reserved3[1] = image_copy;

		/* Set the module's DllBase to image_copy */
		PEB_LDR_DATA* loader_data = reinterpret_cast<PEB_LDR_DATA*>(peb->Ldr);

		if (loader_data->InLoadOrderModuleList.Flink == &loader_data->InLoadOrderModuleList)
		{
			return false;
		}

		LDR_DATA_TABLE_ENTRY* first = reinterpret_cast<LDR_DATA_TABLE_ENTRY*>(&loader_data->InLoadOrderModuleList);

		for (LDR_DATA_TABLE_ENTRY* current = reinterpret_cast<LDR_DATA_TABLE_ENTRY*>(first->InLoadOrderLinks.Flink);
			current != first; current = reinterpret_cast<LDR_DATA_TABLE_ENTRY*>(current->InLoadOrderLinks.Flink))
		{
			try
			{
				if (current->DllBase == image_base)
				{
					unsigned long protection;
					VirtualProtect(current, sizeof(LDR_DATA_TABLE_ENTRY), PAGE_EXECUTE_READWRITE, &protection);

					current->DllBase = image_copy;
				}
			}
			catch (...)
			{
				continue;
			}
		}

		return true;
	}
	
	namespace membase
	{
		typedef struct _SYSTEM_HANDLE 
		{
			unsigned int process_id;
			unsigned char object_type_number;
			unsigned char flags;
			unsigned short handle;
			void* object;
			ACCESS_MASK granted_access;
		} SYSTEM_HANDLE, *PSYSTEM_HANDLE;

		typedef struct _SYSTEM_HANDLE_INFORMATION
		{
			unsigned int handle_count;
			SYSTEM_HANDLE handles[1];
		} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;

		const int STATUS_SUCCESS = 0x00000000;
		const int STATUS_INFO_LENGTH_MISMATCH = 0xC0000004;

		const SYSTEM_INFORMATION_CLASS SystemHandleInformation = static_cast<SYSTEM_INFORMATION_CLASS>(16);
		const OBJECT_INFORMATION_CLASS ObjectNameInformation = static_cast<OBJECT_INFORMATION_CLASS>(1);

		bool close_handle(std::string const& handle_name)
		{
			std::wstring wide_handle_name(handle_name.length(), L' ');
			std::copy(handle_name.begin(), handle_name.end(), wide_handle_name.begin());

			int number_of_handles = 512;
			SYSTEM_HANDLE_INFORMATION* handle_group = nullptr;

			unsigned long length = 0;
			NTSTATUS status = 0;

			do
			{
				free(handle_group);

				number_of_handles *= 2;
				handle_group = reinterpret_cast<SYSTEM_HANDLE_INFORMATION*>(malloc(4 + sizeof(SYSTEM_HANDLE) * number_of_handles));
		
				status = NtQuerySystemInformation(SystemHandleInformation, handle_group, 4 + sizeof(SYSTEM_HANDLE) * number_of_handles, &length);
			}
			while (status == STATUS_INFO_LENGTH_MISMATCH);

			if (status != STATUS_SUCCESS)
			{
				free(handle_group);
				return false;
			}

			for (unsigned int i = 0; i < handle_group->handle_count; i++)
			{
				wchar_t object_name[1024];

				if (NtQueryObject(reinterpret_cast<HANDLE>(handle_group->handles[i].handle), ObjectNameInformation, &object_name, 1024, &length) != STATUS_SUCCESS)
				{
					continue;
				}

				if (wcsstr(object_name, wide_handle_name.c_str()) != NULL)
				{
					HANDLE handle;
					DuplicateHandle(GetCurrentProcess(), reinterpret_cast<HANDLE>(handle_group->handles[i].handle), 0, &handle, 0, FALSE, DUPLICATE_CLOSE_SOURCE);
					CloseHandle(handle);
					free(handle_group);
					return true;
				}
			}
	
			free(handle_group);
			return false;
		}
	}
	
	unsigned char* get_game_image_copy(unsigned int* image_size)
	{
		*image_size = static_cast<unsigned int>(image_end - image_base);
		return image_copy;
	}

	void initialize_game_image()
	{
		image_base = reinterpret_cast<unsigned char*>(GetModuleHandle(NULL));

		dos_header* dos_headers = reinterpret_cast<dos_header*>(image_base);
		nt_header* nt_headers = reinterpret_cast<nt_header*>(image_base + dos_headers->pe_offset);

		image_copy = reinterpret_cast<unsigned char*>(malloc(nt_headers->optional_header.image_size));
		memcpy(image_copy, image_base, nt_headers->optional_header.image_size);

		image_end = image_base + nt_headers->optional_header.image_size;
		
		//set_image_base();
	}

	void initialize_game_bypass()
	{
		membase::close_handle("WvsClientMtx");
		
		detours::redirect(true, reinterpret_cast<void**>(&Crc32_GetCrc32_VMCRC), &Crc32_GetCrc32_VMCRC_hook);
		detours::redirect(true, reinterpret_cast<void**>(&Crc32_GetCrc32_VMTable), &Crc32_GetCrc32_VMTable_hook);
	}
}