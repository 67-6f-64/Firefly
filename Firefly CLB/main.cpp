#include "main_window.hpp"
#include "client_socket_crypto.hpp"

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

namespace process
{
	bool initialize()
	{		
		AllocConsole();
		SetConsoleTitle("Firefly CLB");
		AttachConsole(GetCurrentProcessId());

		FILE* file = nullptr;
		freopen_s(&file, "CON", "r", stdin);
		freopen_s(&file, "CON", "w", stdout);
		freopen_s(&file, "CON", "w", stderr);

		firefly::custom_control::initialize();

		WSADATA wsa_data;
		memset(&wsa_data, 0, sizeof(WSADATA));

		if (WSAStartup(MAKEWORD(2, 2), &wsa_data) || wsa_data.wVersion != MAKEWORD(2, 2))
			return false;
		
		if (!nmco::initialize())
			return false;
		
		if (!firefly::client_socket_crypto::get_instance().initialize())
			return false;

		CoInitializeEx(0, COINIT_MULTITHREADED);
		return true;
	}

	void uninitialize()
	{
		CoUninitialize();
		WSACleanup();
		firefly::custom_control::uninitialize();
		FreeConsole();
	}
}

INT APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, CHAR* lpCommandLine, INT nCommandShow)
{
	if (process::initialize())
	{
		try
		{
			firefly::main_window clientless_bot(hInstance, "firefly_clb_class", "Firefly Clientless Bot");
			clientless_bot.start(true);
		
			MSG message;

			while (GetMessage(&message, 0, 0, 0) > 0)
			{
				if (!clientless_bot.is_dialog_message(&message))
				{
					TranslateMessage(&message);
					DispatchMessage(&message);
				}
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