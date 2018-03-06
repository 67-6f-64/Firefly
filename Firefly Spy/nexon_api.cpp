#include "nexon_api.hpp"
#include "detours.hpp"

#include <iostream>

namespace firefly
{
	bool Hook_nxapi_init(HMODULE hModule)
	{
		typedef BOOL (__cdecl* nxapi_init_t)(const char* version);
		static nxapi_init_t nxapi_init = reinterpret_cast<nxapi_init_t>(GetProcAddress(hModule, "nxapi_init"));

		nxapi_init_t nxapi_init_hook = [](const char* version) -> BOOL
		{
			return TRUE;
		};

		return detours::redirect(true, reinterpret_cast<void**>(&nxapi_init), nxapi_init_hook);
	}
	
	bool Hook_nxapi_shutdown(HMODULE hModule)
	{
		typedef BOOL (__cdecl* nxapi_shutdown_t)();
		static nxapi_shutdown_t nxapi_shutdown = reinterpret_cast<nxapi_shutdown_t>(GetProcAddress(hModule, "nxapi_shutdown"));

		nxapi_shutdown_t nxapi_shutdown_hook = []() -> BOOL
		{
			return TRUE;
		};

		return detours::redirect(true, reinterpret_cast<void**>(&nxapi_shutdown), nxapi_shutdown_hook);
	}

	//nxapi_get_api, 1, 0x13a0						*****
	//nxapi_get_friends, 2, 0x13d0					*****
	//nxapi_get_user, 3, 0x13c0						*****
	//nxapi_init, 4, 0x12f0							
	//nxapi_register_apicall_handler, 5, 0x1440		*****
	//nxapi_register_callback, 6, 0x13e0			*****
	//nxapi_shutdown, 7, 0x1380						
	//nxapi_unregister_apicall_handler, 8, 0x1440	*****
	//nxapi_unregister_callback, 9, 0x1410			*****

	void initialize_nexon_api_hooks()
	{
		HMODULE hModule = GetModuleHandle("nexon_api.dll");

		if (hModule)
		{
			Hook_nxapi_init(hModule);
			Hook_nxapi_shutdown(hModule);
		}
	}
}