#include "windows_injector.hpp"

#define stub_size 512
#define stub_data_size 4 + (MAX_PATH * 2)

namespace firefly
{
	namespace api_stubs
	{
		/* HMODULE WINAPI LoadLibraryEx(LPCTSTR lpLibFileName) */
		__declspec(naked) unsigned int __stdcall LoadLibrary_stub(stub_data* data)
		{
			__asm
			{
				push ebp
				mov ebp,esp
				push ebx
				mov ebx,[ebp+0x08]
				lea eax,[ebx+0x08]
				push eax
				call [ebx]
				pop ebx
				mov esp,ebp
				pop ebp
				ret 0004
			}
		}
	
		/* HMODULE WINAPI LoadLibraryEx(LPCTSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) */
		__declspec(naked) unsigned int __stdcall LoadLibraryEx_stub(stub_data* data)
		{
			__asm
			{
				push ebp
				mov ebp,esp
				push ebx
				mov ebx,[ebp+0x08]
				push 0x00
				push 0x00
				lea eax,[ebx+0x08]
				push eax
				call [ebx]
				pop ebx
				mov esp,ebp
				pop ebp
				ret 0004
			}
		}
	
		/* NTSTATUS NTAPI RtlInitUnicodeString(PUNICODE_STRING DestinationString, PCWSTR SourceString) */
		/* NTSTATUS NTAPI LdrLoadDll(PWCHAR PathToFile, ULONG Flags, PUNICODE_STRING ModuleFileName, PHANDLE ModuleHandle) */
		__declspec(naked) unsigned int __stdcall LdrLoadDll_stub(stub_data* data)
		{
			__asm
			{
				push ebp
				mov ebp,esp
				push ebx
				sub esp,0x0C
				mov ebx,[ebp+0x08]
				lea eax,[ebx+0x08]
				push eax
				lea eax,[esp+0x04]
				push eax
				call [ebx+0x04]
				lea eax,[esp+0x08]
				push eax
				lea eax,[esp+0x04]
				push eax
				push 0x00
				push 0x00
				call [ebx]
				add esp,0x0C
				pop ebx
				mov esp,ebp
				pop ebp
				ret 0004
			}
		}
	}

	bool windows_injector::inject_file(std::string const& file, int process_id, windows_inject_api inject_api, thread_inject_api thread_api)
	{
		HANDLE process = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_TERMINATE | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, process_id);

		if (!process)
			return false;

		bool ret = this->inject_file(file, process, inject_api, thread_api);

		CloseHandle(process);
		return ret;
	}

	bool windows_injector::inject_file(std::string const& file, HANDLE process, windows_inject_api inject_api, thread_inject_api thread_api, HANDLE* thread_handle)
	{
		stub_data* memory_data = reinterpret_cast<stub_data*>(VirtualAllocEx(process, NULL, stub_data_size, MEM_COMMIT, PAGE_READWRITE));

		if (memory_data)
		{
			if (!this->write_apis(inject_api, process, memory_data))
				return false;

			if (!this->write_file_path(inject_api, process, memory_data, file))
				return false;

			void* memory_stub = VirtualAllocEx(process, NULL, stub_size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

			if (memory_stub)
			{
				if (!this->write_api_stub(inject_api, process, memory_stub))
					return false;

				return this->create_thread(thread_api, process, memory_stub, memory_data, thread_handle);
			}
		}

		return false;	
	}

	windows_injector::windows_injector()
		: thread_manager()
	{

	}

	windows_injector::~windows_injector()
	{

	}

	bool windows_injector::write_apis(windows_inject_api inject_api, HANDLE process, stub_data* memory_data)
	{
		void* injection_api_address = nullptr;
		void* init_unicode_string_address = nullptr;

		switch (inject_api)
		{
		case api_load_library_a:
			injection_api_address = GetProcAddress(GetModuleHandle("Kernel32"), "LoadLibraryA");
			break;

		case api_load_library_w:
			injection_api_address = GetProcAddress(GetModuleHandle("Kernel32"), "LoadLibraryW");
			break;

		case api_load_library_ex_a:
			injection_api_address = GetProcAddress(GetModuleHandle("Kernel32"), "LoadLibraryExA");
			break;

		case api_load_library_ex_w:
			injection_api_address = GetProcAddress(GetModuleHandle("Kernel32"), "LoadLibraryExW");
			break;

		case api_ldr_load_dll:
			injection_api_address = GetProcAddress(GetModuleHandle("ntdll"), "LdrLoadDll");
			init_unicode_string_address = GetProcAddress(GetModuleHandle("ntdll"), "RtlInitUnicodeString");
			break;

		default:
			return false;
		}
		
		if (!WriteProcessMemory(process, &memory_data->inject_api, &injection_api_address, sizeof(void*), NULL))
			return false;

		if (!WriteProcessMemory(process, &memory_data->init_unicode_string, &init_unicode_string_address, sizeof(void*), NULL))
			return false;

		return true;
	}

	bool windows_injector::write_file_path(windows_inject_api inject_api, HANDLE process, stub_data* memory_data, std::string const& file)
	{
		const char* module_path = file.c_str();
		
		if (inject_api == api_load_library_w || inject_api == api_load_library_ex_w || inject_api == api_ldr_load_dll)
		{
			int length = MultiByteToWideChar(CP_ACP, 0, module_path, -1, NULL, 0);

			if (length <= 0)
				return false;

			wchar_t* wide_module_path = new wchar_t[length];
			MultiByteToWideChar(CP_ACP, 0, module_path, -1, &wide_module_path[0], length);

			if (!WriteProcessMemory(process, &memory_data->wide_file_path, wide_module_path, (strlen(module_path) + 1) * sizeof(wchar_t), NULL))
			{
				delete[] wide_module_path;
				return false;
			}
			
			delete[] wide_module_path;
		}
		else
		{
			if (!WriteProcessMemory(process, &memory_data->file_path, module_path, (strlen(module_path) + 1) * sizeof(char), NULL))
				return false;
		}

		return true;
	}

	bool windows_injector::write_api_stub(windows_inject_api inject_api, HANDLE process, void* memory_stub)
	{
		void* injection_api_stub = nullptr;

		switch (inject_api)
		{
		case api_load_library_a:
		case api_load_library_w:
			injection_api_stub = api_stubs::LoadLibrary_stub;
			break;

		case api_load_library_ex_a:
		case api_load_library_ex_w:
			injection_api_stub = api_stubs::LoadLibraryEx_stub;
			break;

		case api_ldr_load_dll:
			injection_api_stub = api_stubs::LdrLoadDll_stub;
			break;

		default:
			return false;
		}
			
		return (WriteProcessMemory(process, memory_stub, injection_api_stub, stub_size, NULL) != FALSE);
	}
}