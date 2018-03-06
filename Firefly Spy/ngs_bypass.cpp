#include "ngs_bypass.hpp"
#include "game_bypass.hpp"

#include "file_mapper.hpp"
#include "manual_mapper.hpp"

#include "detours.hpp"

#include <strsafe.h>
#include <winternl.h>
#include <intrin.h>

#include <memory>

#define STATUS_SUCCESS			((NTSTATUS)0x00000000)
#define STATUS_ACCESS_DENIED	((NTSTATUS)0xC0000022)

namespace firefly
{
	std::unique_ptr<io::file_mapper> blackcipher_spy_file_map;

	HANDLE maplestory_mapping = NULL;
	void* maplestory_view = NULL;

	bool Hook_CreateProcessInternalW(bool enable)
	{
		typedef BOOL (WINAPI* CreateProcessInternalW_t)(HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes,
			BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation, PHANDLE hNewToken);   

		static CreateProcessInternalW_t _CreateProcessInternalW = reinterpret_cast<CreateProcessInternalW_t>(GetProcAddress(GetModuleHandle("Kernel32"), "CreateProcessInternalW"));

		CreateProcessInternalW_t CreateProcessInternalW_hook = [](HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes,
			BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation, PHANDLE hNewToken) -> BOOL
		{
			if (lpCommandLine && wcsstr(lpCommandLine, L"BlackCipher.aes"))
			{
				BOOL ret = _CreateProcessInternalW(hUserToken, lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, 
					dwCreationFlags | CREATE_SUSPENDED, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation, hNewToken);
				
				if (ret && blackcipher_spy_file_map)
				{
					io::file_mapper filemap(*blackcipher_spy_file_map.get());

#ifdef _PRODUCTION
					if (!filemap.decrypt())
						return false;
#endif

					manual_mapper mapper;
					mapper.manual_map_memory_file(filemap, lpProcessInformation->dwProcessId, false, thread_inject_api::api_create_remote_thread);
				}
				
				ResumeThread(lpProcessInformation->hThread);
				return ret;
			}

			return _CreateProcessInternalW(hUserToken, lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, 
				dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation, hNewToken);
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&_CreateProcessInternalW), CreateProcessInternalW_hook);
	}
	
	bool Hook_NtReadVirtualMemory(bool enable)
	{
		typedef NTSTATUS (NTAPI* NtReadVirtualMemory_t)(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, ULONG NumberOfBytesToRead, PULONG NumberOfBytesReaded);		
		static NtReadVirtualMemory_t _NtReadVirtualMemory = reinterpret_cast<NtReadVirtualMemory_t>(GetProcAddress(GetModuleHandle("ntdll"), "NtReadVirtualMemory"));

		NtReadVirtualMemory_t NtReadVirtualMemory_hook = [](HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, ULONG NumberOfBytesToRead, PULONG NumberOfBytesReaded) -> NTSTATUS
		{
			HMODULE hModule = NULL;
			GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, reinterpret_cast<LPCSTR>(_ReturnAddress()), &hModule);

			if (hModule && hModule == GetModuleHandle("BlackCall.aes"))
			{
				if (ProcessHandle != GetCurrentProcess())
					return STATUS_ACCESS_DENIED;

				GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, reinterpret_cast<LPCSTR>(BaseAddress), &hModule);
				
				if (hModule && hModule == GetModuleHandle(NULL))
					return STATUS_ACCESS_DENIED;
			}

			return _NtReadVirtualMemory(ProcessHandle, BaseAddress,	Buffer, NumberOfBytesToRead, NumberOfBytesReaded);
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&_NtReadVirtualMemory), NtReadVirtualMemory_hook);
	}
	
	void set_blackcipher_spy(unsigned char* data, unsigned int size)
	{
		blackcipher_spy_file_map = std::make_unique<io::file_mapper>(data, size);
	}
	
	bool initialize_ngs_bypass()
	{
		std::string mapping_name = "Global\\NexonGameClient" + std::to_string(GetCurrentProcessId());

		unsigned int image_size = 0;
		unsigned char* image_copy = get_game_image_copy(&image_size);

		maplestory_mapping = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, image_size, mapping_name.c_str());
		
		if (maplestory_mapping == NULL && GetLastError() != ERROR_ALREADY_EXISTS)
			return false;

		maplestory_view = MapViewOfFile(maplestory_mapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);

		if (maplestory_view == NULL)
		{
			CloseHandle(maplestory_mapping);
			return false;
		}

		memcpy(maplestory_view, image_copy, image_size);

		return Hook_CreateProcessInternalW(true) && Hook_NtReadVirtualMemory(true);
	}
	
	void uninitialize_ngs_bypass()
	{
		if (maplestory_view)
			UnmapViewOfFile(maplestory_view);

		if (maplestory_mapping)
			CloseHandle(maplestory_mapping);
	}
}