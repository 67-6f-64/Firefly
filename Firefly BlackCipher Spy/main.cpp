#include "native.hpp"
#include "blackcipher_bypass.hpp"

#include <iostream>

namespace module
{
	bool initialize(HMODULE module)
	{
#ifndef _PRODUCTION
		//AllocConsole();
		//SetConsoleTitle("Firefly BlackCipher Spy");
		//AttachConsole(GetCurrentProcessId());

		//FILE* file = nullptr;
		//freopen_s(&file, "CON", "r", stdin);
		//freopen_s(&file, "CON", "w", stdout);
		//freopen_s(&file, "CON", "w", stderr);
#endif

		firefly::initialize_blackcipher_bypass(module);

		DisableThreadLibraryCalls(module);
		return true;
	}

	bool uninitialize(HMODULE module)
	{
		return true; //(FreeConsole() != FALSE);
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