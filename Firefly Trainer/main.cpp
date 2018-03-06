#include "firefly_trainer.hpp"

#include "nmco_api\NMCOClientObject.h"
#include "nmco_api\NMManager.h"

namespace nmco
{
	bool initialize()
	{
		CNMManager::GetInstance().Init();

		if (!CNMCOClientObject::GetInstance().SetPatchOption(FALSE))
			return false;

		if (!CNMCOClientObject::GetInstance().SetLocale(kLocaleID_US, kRegionCode_NXA2))
			return false;

		if (!CNMCOClientObject::GetInstance().Initialize(kGameCode_maplestory_us))
			return false;

		return true;
	}
}

namespace token
{
	bool set_debug_privilege()
	{
		HANDLE token = NULL;

		if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &token))
		{
			TOKEN_PRIVILEGES token_privileges;
			memset(&token_privileges, 0, sizeof(TOKEN_PRIVILEGES));

			token_privileges.PrivilegeCount = 1;

			if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &token_privileges.Privileges[0].Luid))
			{
				token_privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

				if (AdjustTokenPrivileges(token, FALSE, &token_privileges, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
				{
					CloseHandle(token);
					return true;
				}
			}

			CloseHandle(token);
		}

		return false;
	}

	bool is_process_elevated()
	{
		TOKEN_ELEVATION token_elevation;
		memset(&token_elevation, 0, sizeof(TOKEN_ELEVATION));

		HANDLE token = NULL;

		if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token))
		{
			DWORD token_size = sizeof(TOKEN_ELEVATION);
			GetTokenInformation(token, TokenElevation, &token_elevation, sizeof(token_elevation), &token_size);
			CloseHandle(token);
		}

		return (token_elevation.TokenIsElevated != FALSE);
	}
}

namespace process
{
	bool initialize()
	{		
#ifndef _PRODUCTION
		AllocConsole();
		SetConsoleTitle("Firefly Trainer");
		AttachConsole(GetCurrentProcessId());

		FILE* file = nullptr;
		freopen_s(&file, "CON", "r", stdin);
		freopen_s(&file, "CON", "w", stdout);
		freopen_s(&file, "CON", "w", stderr);
#else
		if (!token::is_process_elevated())
			return false;
#endif
		
		firefly::custom_control::initialize();

		WSADATA wsa_data;
		memset(&wsa_data, 0, sizeof(WSADATA));

		if (WSAStartup(MAKEWORD(2, 2), &wsa_data) || wsa_data.wVersion != MAKEWORD(2, 2))
			return false;
		
		if (!nmco::initialize())
			return false;

		CoInitializeEx(0, COINIT_MULTITHREADED);
		token::set_debug_privilege();
		return true;
	}

	void uninitialize()
	{
		CoUninitialize();
		WSACleanup();
		firefly::custom_control::uninitialize();
#ifndef _PRODUCTION
		FreeConsole();
#endif
	}
}

INT APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, CHAR* lpCommandLine, INT nCommandShow)
{
	if (process::initialize())
	{
		try
		{
			//firefly::firefly_trainer firefly_trainer(hInstance, "firefly_trainer_class", "Firefly Trainer");
			//firefly_trainer.initialize();
			//firefly_trainer.start(true);
			
			MSG message;

			while (GetMessage(&message, 0, 0, 0) > 0)
			{
				//if (!firefly_trainer.is_dialog_message(&message))
				//{
					TranslateMessage(&message);
					DispatchMessage(&message);
				//}
			}

			process::uninitialize();
			return message.wParam;
		}
		catch (std::exception& e)
		{
			MessageBox(NULL, e.what(), "An exception occured!", MB_OK | MB_ICONERROR | MB_TOPMOST | MB_SETFOREGROUND);
		}
	}
	else
	{
		MessageBox(NULL, "Application failed to initialize.\nPlease make sure you run this application as administrator.", "Token Error!", MB_OK | MB_ICONERROR | MB_TOPMOST | MB_SETFOREGROUND);
	}

	process::uninitialize();
	return 0;
}