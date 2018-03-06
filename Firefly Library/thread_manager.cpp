#include "thread_manager.hpp"

#include <Winternl.h>

typedef NTSTATUS (NTAPI* NtCreateThreadEx_t)(PHANDLE hThread, ACCESS_MASK DesiredAccess, LPVOID ObjectAttributes, HANDLE ProcessHandle, LPTHREAD_START_ROUTINE lpStartAddress,
	LPVOID lpParameter, BOOL CreateSuspended, ULONG StackZeroBits, ULONG SizeOfStackCommit, ULONG SizeOfStackReserve, LPVOID lpBytesBuffer);
 
namespace firefly
{
	thread_manager::thread_manager()
	{

	}

	thread_manager::~thread_manager()
	{

	}

	bool thread_manager::create_thread(thread_inject_api thread_api, HANDLE process, void* start_address, void* parameter, HANDLE* thread_handle)
	{
		switch (thread_api)
		{
		case api_create_remote_thread:
			return this->create_remote_thread(process, start_address, parameter, thread_handle);

		case api_create_remote_thread_ex:
			return this->create_remote_thread_ex(process, start_address, parameter, thread_handle);

		case api_nt_create_thread_ex:
			return this->nt_create_thread_ex(process, start_address, parameter, thread_handle);

		default:
			break;
		}

		return false;
	}

	bool thread_manager::create_remote_thread(HANDLE process, void* start_address, void* parameter, HANDLE* thread_handle)
	{
		if (!thread_handle)
		{
			HANDLE temp_thread_handle = CreateRemoteThread(process, NULL, 0, LPTHREAD_START_ROUTINE(start_address), parameter, 0, NULL);

			if (!temp_thread_handle)
				return false;

			CloseHandle(temp_thread_handle);
			return true;
		}

		return ((*thread_handle = CreateRemoteThread(process, NULL, 0, LPTHREAD_START_ROUTINE(start_address), parameter, 0, NULL)) != NULL);
	}

	bool thread_manager::create_remote_thread_ex(HANDLE process, void* start_address, void* parameter, HANDLE* thread_handle)
	{
		if (!thread_handle)
		{
			HANDLE temp_thread_handle = CreateRemoteThreadEx(process, NULL, 0, LPTHREAD_START_ROUTINE(start_address), parameter, 0, NULL, NULL);

			if (!temp_thread_handle)
				return false;

			CloseHandle(temp_thread_handle);
			return true;
		}

		return ((*thread_handle = CreateRemoteThreadEx(process, NULL, 0, LPTHREAD_START_ROUTINE(start_address), parameter, 0, NULL, NULL)) != NULL);
	}

	bool thread_manager::nt_create_thread_ex(HANDLE process, void* start_address, void* parameter, HANDLE* thread_handle)
	{
		NtCreateThreadEx_t NtCreateThreadEx = reinterpret_cast<NtCreateThreadEx_t>(GetProcAddress(GetModuleHandle("ntdll"), "NtCreateThreadEx"));

		if (!NtCreateThreadEx)
			return false;

		if (!thread_handle)
		{
			HANDLE temp_thread_handle = NULL;

			if (NtCreateThreadEx(&temp_thread_handle, GENERIC_ALL, NULL, process, LPTHREAD_START_ROUTINE(start_address), parameter, FALSE, 0, 0, 0, NULL) == 0)
				return false;

			if (!temp_thread_handle)
				return false;

			CloseHandle(temp_thread_handle);
			return true;
		}

		return (NtCreateThreadEx(thread_handle, GENERIC_ALL, NULL, process, LPTHREAD_START_ROUTINE(start_address), parameter, FALSE, 0, 0, 0, NULL) == 0 && *thread_handle != NULL);
	}
}