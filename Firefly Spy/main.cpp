#include "native.hpp"
#include "net_socket.hpp"

#include "windows_hooks.hpp"
#include "network_client.hpp"

#ifndef _PRODUCTION
#include <iostream>

typedef const struct
{
	unsigned int magic;
	void* object;
	_ThrowInfo* info;
} CxxThrowExceptionObject;

void* track_exceptions()
{
	PVECTORED_EXCEPTION_HANDLER exception_handler = [](EXCEPTION_POINTERS* pExceptionInfo) -> long
	{
		if (pExceptionInfo->ExceptionRecord->ExceptionCode == 0xE06D7363) // C++ Exceptions
		{
			CxxThrowExceptionObject* exception_object = reinterpret_cast<CxxThrowExceptionObject*>(pExceptionInfo->ExceptionRecord->ExceptionInformation);

			if (exception_object->magic == 0x19930520 && exception_object->info->pCatchableTypeArray->nCatchableTypes > 0)
			{
				typedef struct ZException
				{					
					HRESULT m_hr;
				} ZException;

				ZException* exc = reinterpret_cast<ZException*>(exception_object->object);
				printf("CMSException: %s - %08X\n", exception_object->info->pCatchableTypeArray->arrayOfCatchableTypes[0]->pType->name, exc->m_hr);
			}
		}
		else if (pExceptionInfo->ExceptionRecord->ExceptionCode != STATUS_PRIVILEGED_INSTRUCTION && pExceptionInfo->ExceptionRecord->ExceptionCode != DBG_PRINTEXCEPTION_C)
		{
			if (pExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_HEAP_CORRUPTION || pExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_ACCESS_VIOLATION)
			{
				if (pExceptionInfo->ExceptionRecord->ExceptionAddress != 0)
				{
					printf("RegException: %08X (%08X)\n", pExceptionInfo->ExceptionRecord->ExceptionCode, pExceptionInfo->ExceptionRecord->ExceptionAddress);

					FILE* ff = fopen("C:\\exc.txt", "a+");

					for (int i = 0; i < 0x400; i += 4)
						fprintf(ff, "Esp+%02X = %08X\n", i, *(DWORD*)(pExceptionInfo->ContextRecord->Esp + i));

					fclose(ff);
				}
			}
			else
				printf("RegException: %08X (%08X)\n", pExceptionInfo->ExceptionRecord->ExceptionCode, pExceptionInfo->ExceptionRecord->ExceptionAddress);
		}

		return EXCEPTION_CONTINUE_SEARCH;
	};

	return AddVectoredExceptionHandler(1, exception_handler);
}
#endif

namespace module
{
	bool initialize(HMODULE module)
	{
#ifndef _PRODUCTION
		AllocConsole();
		SetConsoleTitle("Firefly Spy");
		AttachConsole(GetCurrentProcessId());

		FILE* file = nullptr;
		freopen_s(&file, "CON", "r", stdin);
		freopen_s(&file, "CON", "w", stdout);
		freopen_s(&file, "CON", "w", stderr);
		
		//track_exceptions();
#endif

		firefly::initialize_windows_hooks(module);

#ifdef _PRODUCTION
		SetErrorMode(SetErrorMode(0) | SEM_NOGPFAULTERRORBOX);
#endif

		DisableThreadLibraryCalls(module);
		return true;
	}

	bool uninitialize(HMODULE module)
	{
#ifdef _DEBUG
		return (FreeConsole() != FALSE);
#else
		return true;
#endif
	}
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpvReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		return module::initialize(hModule);

	case DLL_PROCESS_DETACH:
		return module::uninitialize(hModule);

	default:
		break;
	}

	return TRUE;
}