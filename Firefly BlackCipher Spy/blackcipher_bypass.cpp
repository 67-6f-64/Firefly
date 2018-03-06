#include "blackcipher_bypass.hpp"

#include "native.hpp"
#include "detours.hpp"

#include <strsafe.h>
#include <winternl.h>
#include <TlHelp32.h>
#include <intrin.h>
#include <Psapi.h>

#include <string>
#include <algorithm>
#include <vector>

#ifdef _PRODUCTION
#include <VMProtectSDK.h>
#endif

#pragma warning(disable: 4733)
#pragma comment(lib, "ntdll")

#define STATUS_SUCCESS			((NTSTATUS)0x00000000)
#define STATUS_ACCESS_DENIED	((NTSTATUS)0xC0000022)
#define STATUS_INVALID_CID		((NTSTATUS)0xC000000B)

/* How BlackCipher.aes uses the WinAPIs

##############################################################
						NtOpenProcess							
##############################################################

Callchain:
OpenProcess_impl_(Access, Inherit, ProcessID)
-> NtOpenProcess_impl_(ProcessHandle*, DesiredAccess, ObjectAttributes, ClientId)
-> -> NtOpenProcess_buffer(ProcessHandle*, DesiredAccess, ObjectAttributes, ClientId)
-> -> -> fs:[000000C0]

NTSTATUS NTAPI NtOpenProcess_impl_(PHANDLE ProcessHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PCLIENT_ID ClientId)
{
	return (decltype(&NtOpenProcess)(NtOpenProcess_buffer))(ProcessHandle, DesiredAccess, ObjectAttributes, ClientId);
}

HANDLE WINAPI OpenProcess_impl_(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId)
{
	CLIENT_ID ClientId;
	ClientId->UniqueProcess = reintepret_cast<HANDLE>(dwProcessId);
	ClientId->UniqueThread = NULL;

	OBJECT_ATTRIBUTES ObjectAttributes;
	InitializeObjectAttributes(&ObjectAttributes, NULL, bInheritHandle ? OBJ_INHERIT : NULL, NULL, NULL);

	HANDLE ProcessHandle = NULL;

	NTSTATUS Status = NtOpenProcess_impl_(&ProcessHandle, dwDesiredAccess, &ObjectAttributes, &ClientId);

	if (!NT_SUCCESS(Status))
	{
		SetLastError(RtlNtStatusToDosError(Status));
		return INVALID_HANDLE_VALUE;
	}
	
	return ProcessHandle;
}

##############################################################
					NtQueryVirtualMemory					
##############################################################

Callchain:
NtQueryVirtualMemory_impl_(ProcessHandle, BaseAddress, MemoryInformationClass, MemoryInformation, MemoryInformationLength, ReturnLength)
-> NtQueryVirtualMemory_buffer(ProcessHandle, BaseAddress, MemoryInformationClass, MemoryInformation, MemoryInformationLength, ReturnLength)
-> -> fs:[000000C0]

NTSTATUS NTAPI NtQueryVirtualMemory_impl_(HANDLE ProcessHandle, PVOID BaseAddress, MEMORY_INFORMATION_CLASS MemoryInformationClass, PVOID MemoryInformation, SIZE_T MemoryInformationLength, PSIZE_T ReturnLength)
{
	return (decltype(&NtQueryVirtualMemory)(NtQueryVirtualMemory_buffer))(ProcessHandle, BaseAddress, MemoryInformationClass, MemoryInformation, MemoryInformationLength, ReturnLength);
}

##############################################################
					NtReadVirtualMemory						
##############################################################

Callchain:
ReadProcessMemory_impl_(hProcess, lpBaseAddress, lpBuffer, nSize, NumberOfBytesReaded)
-> NtReadVirtualMemory_impl_(ProcessHandle, BaseAddress, Buffer, NumberOfBytesToRead, NumberOfBytesReaded)
-> -> NtReadVirtualMemory_buffer(ProcessHandle, BaseAddress, Buffer, NumberOfBytesToRead, NumberOfBytesReaded)
-> -> -> fs:[000000C0]

NTSTATUS NTAPI NtReadVirtualMemory_impl_(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, ULONG NumberOfBytesToRead, PULONG NumberOfBytesReaded)
{
	return (decltype(&NtReadVirtualMemory)(NtReadVirtualMemory_buffer))(ProcessHandle, BaseAddress, Buffer, NumberOfBytesToRead, NumberOfBytesReaded);
}

BOOL WINAPI ReadProcessMemory_impl_(HANDLE hProcess, LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, PULONG NumberOfBytesReaded)
{
	NTSTATUS Status = NtReadVirtualMemory_impl_(hProcess, lpBaseAddress, lpBuffer, nSize, NumberOfBytesReaded != NULL ? NumberOfBytesReaded : &nSize);

	if (!NT_SUCCESS(Status))
	{
		SetLastError(RtlNtStatusToDosError(Status));
		return FALSE;
	}
	
	return TRUE;
}
*/

namespace firefly
{
	typedef struct _CLIENT_ID {
		ULONG UniqueProcess;
		ULONG UniqueThread;
	 } CLIENT_ID, *PCLIENT_ID;

	typedef enum _MEMORY_INFORMATION_CLASS {
		MemoryBasicInformation,
		MemoryWorkingSetList,
		MemorySectionName
	} MEMORY_INFORMATION_CLASS;
	
	typedef struct _PROCESS_BASIC_INFORMATION {
		ULONG ExitStatus;
		PPEB PebBaseAddress;
		ULONG AffinityMask;
		ULONG BasePriority;
		ULONG UniqueProcessId;
		ULONG ParentUniqueProcessId;
	} PROCESS_BASIC_INFORMATION;

	typedef struct _module_entry
	{
		unsigned int id;
		std::string const name;

		unsigned char* base_address;
		unsigned int base_size;

		HMODULE handle;
	} module_entry;
	
	unsigned char* game_base_address = 0;
	std::vector<module_entry> game_modules;

	unsigned int VMEntryPoint_NtOpenProcess = 0x0143F0E7;			// 0x013C91F7;
	unsigned int VMEntryPoint_NtQueryVirtualMemory = 0x0143C577;	// 0x013C65B3; // 0047B0BB (MemoryBasicInformation), 0047AF17 (MemoryWorkingSetList), 0048D5B3 (MemorySectionName)
	unsigned int VMEntryPoint_ReadProcessMemory = 0x0138CEA7;		// 0x01320386;
	
#ifdef _PRODUCTION
#pragma optimize("g", off)
#endif
	NTSTATUS NTAPI NtOpenProcess_validate_(PHANDLE ProcessHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PCLIENT_ID ClientId)
	{
#ifdef _PRODUCTION
		VMProtectBeginVirtualization("NtOpenProcess_validate_");
#endif
		if (ProcessHandle && *ProcessHandle)
		{
			PROCESS_BASIC_INFORMATION pbi;
			memset(&pbi, 0, sizeof(PROCESS_BASIC_INFORMATION));

			ULONG return_length = 0;
		
			if ((NtQueryInformationProcess(GetCurrentProcess(), ProcessBasicInformation, &pbi, sizeof(PROCESS_BASIC_INFORMATION), &return_length) == STATUS_SUCCESS) &&
				(pbi.ParentUniqueProcessId == ClientId->UniqueProcess))
			{
				game_base_address = reinterpret_cast<unsigned char*>(pbi.PebBaseAddress->Reserved3[1]);

				HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, ClientId->UniqueProcess);

				if (hSnapshot != INVALID_HANDLE_VALUE)
				{
					MODULEENTRY32 module_entry;
					memset(&module_entry, 0, sizeof(MODULEENTRY32));

					module_entry.dwSize = sizeof(MODULEENTRY32);

					if (Module32First(hSnapshot, &module_entry))
					{
						game_modules.clear();

						do
						{
							game_modules.push_back({ module_entry.th32ProcessID, std::string(module_entry.szModule), module_entry.modBaseAddr, module_entry.modBaseSize, module_entry.hModule });
						}
						while (Module32Next(hSnapshot, &module_entry));
					}

					CloseHandle(hSnapshot);
				}
			}
			else
			{
				CloseHandle(*ProcessHandle);
				*ProcessHandle = NULL;

				return STATUS_ACCESS_DENIED;
			}
		}

		return STATUS_SUCCESS;
#ifdef _PRODUCTION
		VMProtectEnd();
#endif
	}
	
	void __declspec(naked) VMEntryPoint_NtOpenProcess_hook()
	{
#ifdef _PRODUCTION
		VMProtectBeginVirtualization("VMEntryPoint_NtOpenProcess_hook");
#endif
		__asm
		{
			push eax
			push [ebp+0x14] // ClientId
			push [ebp+0x10] // ObjectAttributes
			push [ebp+0x0C] // DesiredAccess
			push [ebp+0x08] // ProcessHandle
			call NtOpenProcess_validate_
			test eax,eax
			jz VMEntryPoint_NtOpenProcess_end

			mov dword ptr [esp],eax

			VMEntryPoint_NtOpenProcess_end:
			pop eax
			jmp dword ptr [VMEntryPoint_NtOpenProcess]
		}
#ifdef _PRODUCTION
		VMProtectEnd();
#endif
	}

	NTSTATUS NTAPI NtQueryVirtualMemory_impl_(HANDLE ProcessHandle, LPBYTE BaseAddress, MEMORY_INFORMATION_CLASS MemoryInformationClass, PVOID MemoryInformation, SIZE_T MemoryInformationLength, PSIZE_T ReturnLength)
	{
#ifdef _PRODUCTION
		VMProtectBeginVirtualization("NtQueryVirtualMemory_impl_");
#endif
		typedef NTSTATUS (NTAPI* NtQueryVirtualMemory_t)(HANDLE ProcessHandle, PVOID BaseAddress, MEMORY_INFORMATION_CLASS MemoryInformationClass, PVOID MemoryInformation, SIZE_T MemoryInformationLength, PSIZE_T ReturnLength);
		static NtQueryVirtualMemory_t NtQueryVirtualMemory = reinterpret_cast<NtQueryVirtualMemory_t>(GetProcAddress(LoadLibrary("ntdll"), "NtQueryVirtualMemory"));

		switch (MemoryInformationClass)
		{
		case MemoryBasicInformation:
			memset(MemoryInformation, 0, MemoryInformationLength);

			if (ReturnLength != NULL)
				*ReturnLength = 0;

			return STATUS_SUCCESS;

		case MemoryWorkingSetList:	// Retrieves a huge chunk of data
		case MemorySectionName:		// Retrieves a UNICODE_STRING that contains the name of the section
			memset(MemoryInformation, 0, MemoryInformationLength);

			if (ReturnLength != NULL)
				*ReturnLength = 0;

			return STATUS_ACCESS_VIOLATION;

		default:
			break;
		}
		
		return NtQueryVirtualMemory(ProcessHandle, BaseAddress, MemoryInformationClass, MemoryInformation, MemoryInformationLength, ReturnLength);
#ifdef _PRODUCTION
		VMProtectEnd();
#endif
	}
	
	void __declspec(naked) VMEntryPoint_NtQueryVirtualMemory_hook()
	{
#ifdef _PRODUCTION
		VMProtectBeginVirtualization("VMEntryPoint_NtQueryVirtualMemory_hook");
#endif
		__asm
		{
			push [ebp+0x1C] // ReturnLength
			push [ebp+0x18] // MemoryInformationLength
			push [ebp+0x14] // MemoryInformation
			push [ebp+0x10] // MemoryInformationClass
			push [ebp+0x0C] // BaseAddress
			push [ebp+0x08] // ProcessHandle
			call NtQueryVirtualMemory_impl_
			mov esp,ebp
			pop ebp
			ret 0x0018
		}
#ifdef _PRODUCTION
		VMProtectEnd();
#endif
	}
	
	BOOL WINAPI ReadProcessMemory_game_impl_(HANDLE hProcess, LPBYTE lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, PULONG lpNumberOfBytesRead)
	{
#ifdef _PRODUCTION
		VMProtectBeginVirtualization("ReadProcessMemory_game_impl_");
#endif
		std::vector<module_entry>::iterator iter = std::find_if(game_modules.begin(), game_modules.end(), [&](const module_entry& a) -> bool
		{
			return (lpBaseAddress >= a.base_address && lpBaseAddress < (a.base_address + a.base_size));
		});

		if (iter != game_modules.end())
		{
			if (iter->base_address == game_base_address)
			{
				std::string mapping_name = "Global\\NexonGameClient" + std::to_string(GetProcessId(hProcess));
				HANDLE game_mapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, mapping_name.c_str());

				if (game_mapping != NULL)
				{
					void* game_mapping_view = MapViewOfFile(game_mapping, FILE_MAP_READ, 0, 0, 0);

					if (game_mapping_view != NULL)
					{
						memcpy(lpBuffer, reinterpret_cast<unsigned char*>(game_mapping_view) + (lpBaseAddress - iter->base_address), nSize);

						if (lpNumberOfBytesRead)
							*lpNumberOfBytesRead = nSize;

						UnmapViewOfFile(game_mapping_view);
						CloseHandle(game_mapping);
						return TRUE;
					}
					else
					{
						CloseHandle(game_mapping);
					}
				}
			}
			else if (!strstr(iter->name.c_str(), "BlackCall.aes") && !strstr(iter->name.c_str(), "NGClient.aes"))
			{
				/* Reading the rest of the image blocks from the MapleStory process */
				SetLastError(ERROR_ACCESS_DENIED);
				return FALSE;
			}
		}
		else
		{
			/* Reading non-image blocks from the MapleStory process */
			//SetLastError(ERROR_ACCESS_DENIED);
			//return FALSE;
		}
		
		return ReadProcessMemory(hProcess, lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesRead);
#ifdef _PRODUCTION
		VMProtectEnd();
#endif
	}

	BOOL WINAPI ReadProcessMemory_impl_(HANDLE hProcess, LPBYTE lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, PULONG lpNumberOfBytesRead)
	{
#ifdef _PRODUCTION
		VMProtectBeginVirtualization("ReadProcessMemory_impl_");
#endif
		if (hProcess == GetCurrentProcess())
		{
			SetLastError(ERROR_ACCESS_DENIED);
			return FALSE;
		}
		
		return ReadProcessMemory_game_impl_(hProcess, lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesRead);
#ifdef _PRODUCTION
		VMProtectEnd();
#endif
	}

	void __declspec(naked) VMEntryPoint_ReadProcessMemory_hook()
	{
#ifdef _PRODUCTION
		VMProtectBeginVirtualization("VMEntryPoint_ReadProcessMemory_hook");
#endif
		__asm
		{
			push [ebp+0x18] // NumberOfBytesReaded
			push [ebp+0x14] // nSize
			push [ebp+0x10] // lpBuffer
			push [ebp+0x0C] // lpBaseAddress
			push [ebp+0x08] // hProcess
			call ReadProcessMemory_impl_
			mov esp,ebp
			pop ebp
			ret 0x0014
		}
#ifdef _PRODUCTION
		VMProtectEnd();
#endif
	}
	
	void initialize_VMEntryPoint_hooks()
	{
#ifdef _PRODUCTION
		VMProtectBeginVirtualization("initialize_VMEntryPoint_hooks");
#endif
		detours::redirect(true, reinterpret_cast<void**>(&VMEntryPoint_NtOpenProcess), &VMEntryPoint_NtOpenProcess_hook);
		detours::redirect(true, reinterpret_cast<void**>(&VMEntryPoint_NtQueryVirtualMemory), &VMEntryPoint_NtQueryVirtualMemory_hook);
		detours::redirect(true, reinterpret_cast<void**>(&VMEntryPoint_ReadProcessMemory), &VMEntryPoint_ReadProcessMemory_hook);
#ifdef _PRODUCTION
		VMProtectEnd();
#endif
	}

	bool Hook_GetCommandLineW(bool enable)
	{
		static decltype(&GetCommandLineW) _GetCommandLineW = decltype(&GetCommandLineW)(GetProcAddress(GetModuleHandle("Kernel32"), "GetCommandLineW"));

		decltype(&GetCommandLineW) GetCommandLineW_hook = []() -> LPWSTR
		{
#ifdef _PRODUCTION
			VMProtectBeginVirtualization("GetCommandLineW_hook");
#endif
			static bool is_detoured = false;

			if (!is_detoured)
			{
				initialize_VMEntryPoint_hooks();
				is_detoured = true;
			}
			
			return _GetCommandLineW();
#ifdef _PRODUCTION
			VMProtectEnd();
#endif
		};
		
		return detours::redirect(enable, reinterpret_cast<void**>(&_GetCommandLineW), GetCommandLineW_hook);
	}
#ifdef _PRODUCTION
#pragma optimize("g", on)
#endif

	bool Hook_CopyFileW(bool enable)
	{
		static decltype(&CopyFileW) _CopyFileW = &CopyFileW;

		decltype(&CopyFileW) CopyFileW_hook = [](LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName, BOOL bFailIfExists) -> BOOL
		{
			LPCWSTR lpExistingFileNameEnd = lpExistingFileName + wcslen(lpExistingFileName);

			if (!lstrcmpiW(lpExistingFileNameEnd - wcslen(L"ntdll.dll"), L"ntdll.dll") || !lstrcmpiW(lpExistingFileNameEnd - wcslen(L"kernelbase.dll"), L"kernelbase.dll"))
			{
				LPCWSTR lpNewFileNameEnd = lpNewFileName + wcslen(lpNewFileName);
				return FALSE;
			}
			
			return _CopyFileW(lpExistingFileName, lpNewFileName, bFailIfExists);
		};
		
		return detours::redirect(enable, reinterpret_cast<void**>(&_CopyFileW), CopyFileW_hook);
	}

	bool initialize_blackcipher_bypass(HMODULE module)
	{
		return (Hook_GetCommandLineW(true) && Hook_CopyFileW(true));
	}
}