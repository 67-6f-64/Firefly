#include "game_client.hpp"
#include "manual_mapper.hpp"

#ifdef _PRODUCTION
#include <VMProtectSDK.h>
#endif

namespace firefly
{
	game_client::game_client()
		: spy_file_map(nullptr), blackcipher_spy_file_map(nullptr)
	{

	}

	game_client::~game_client()
	{
		this->terminate_process();
	}
	
	bool game_client::start_process(std::string const& directory, std::string const& username, std::string const& password)
	{
		NMLoginAuthReplyCode reply_code;

		if ((reply_code = CNMCOClientObject::GetInstance().LoginAuth(username.c_str(), password.c_str())) != kLoginAuth_OK)
		{
			this->print_login_error(reply_code);
			return false;
		}

		char nexon_passport[NXPASSPORT_SIZE];
		memset(nexon_passport, 0, sizeof(nexon_passport));
	
		if (!CNMCOClientObject::GetInstance().GetNexonPassport(nexon_passport))
			return false;
		
		unsigned int error_code = 0;

		if (!this->create_process(directory, std::string("-nxl ") + nexon_passport, &error_code))
		{
			this->print_launch_error(error_code);
			return false;
		}

		return true;
	}

	bool game_client::terminate_process()
	{
		if (this->process_info.hProcess)
		{
			unsigned long exit_code = 0;

			if (!GetExitCodeProcess(this->process_info.hProcess, &exit_code))
				return false;

			if (!TerminateProcess(this->process_info.hProcess, exit_code))
				return false;

			memset(&this->startup_info, 0, sizeof(STARTUPINFO));
			memset(&this->process_info, 0, sizeof(PROCESS_INFORMATION));
		}

		return true;
	}
	
	void game_client::set_spy_file_map(std::vector<unsigned char>& file_data)
	{
		this->spy_file_map = std::make_unique<io::file_mapper>(file_data);
	}
	
	void game_client::set_blackcipher_spy_file_map(std::vector<unsigned char>& file_data)
	{
		this->blackcipher_spy_file_map = std::make_unique<io::file_mapper>(file_data);
	}
	
	bool game_client::send_communication_data(HWND sender, unsigned short port)
	{
		HWND hwnd = this->get_window_handle(window_type::splashscreen);

		if (!hwnd)
			return false;

		COPYDATASTRUCT copy_data;
		memset(&copy_data, 0, sizeof(COPYDATASTRUCT));

		copy_data.dwData = port;

		if (this->blackcipher_spy_file_map)
		{
			copy_data.cbData = this->blackcipher_spy_file_map.get()->size();
			copy_data.lpData = this->blackcipher_spy_file_map.get()->data();
		}

		SendMessage(hwnd, WM_COPYDATA, reinterpret_cast<WPARAM>(sender), reinterpret_cast<LPARAM>(&copy_data));
		return true;
	}
	
	unsigned int game_client::get_process_id() const
	{
		return this->process_info.dwProcessId;
	}
	
	bool game_client::create_process(std::string const& directory, std::string const& commandline, unsigned int* error_code)
	{
#ifdef _PRODUCTION
		VMProtectBeginVirtualization("game_client::start_process");
#endif
		memset(&this->startup_info, 0, sizeof(STARTUPINFO));
		memset(&this->process_info, 0, sizeof(PROCESS_INFORMATION));
		
		std::string game_path = directory + "\\MapleStory.exe";
		std::string argument = "\"" + game_path + "\" " + commandline;
		
		if (!CreateProcess(game_path.c_str(), const_cast<char*>(argument.c_str()), NULL, NULL, TRUE, CREATE_SUSPENDED, NULL, directory.c_str(), &this->startup_info, &this->process_info))
		{
			*error_code = GetLastError();
			return false;
		}

#ifndef _PRODUCTION
		this->spy_file_map = std::make_unique<io::file_mapper>("Firefly Spy.dll");
		this->blackcipher_spy_file_map = std::make_unique<io::file_mapper>("Firefly BlackCipher Spy.dll");
#endif

		if (this->spy_file_map)
		{
			io::file_mapper filemap(*this->spy_file_map.get());
			
#ifdef _PRODUCTION
			if (!filemap.decrypt())
				return false;
#endif

			manual_mapper mapper;
			mapper.manual_map_memory_file(filemap, this->process_info.dwProcessId, false, thread_inject_api::api_create_remote_thread);
		}

		ResumeThread(this->process_info.hThread);
		WaitForInputIdle(this->process_info.hProcess, INFINITE);

#ifdef _PRODUCTION
		VMProtectEnd();
#endif
		return true;
	}

	HWND game_client::get_window_handle(window_type type)
	{
		for (HWND hwnd = GetTopWindow(NULL); hwnd != NULL; hwnd = GetNextWindow(hwnd, GW_HWNDNEXT))
		{
			unsigned long current_process_id = 0;
			GetWindowThreadProcessId(hwnd, &current_process_id);

			if (current_process_id != this->process_info.dwProcessId)
				continue;

			char class_name[256];
			memset(class_name, 0, sizeof(class_name));

			if (!GetClassName(hwnd, class_name, sizeof(class_name)))
				continue;

			switch (type)
			{
			case window_type::splashscreen:
				{
					if (!strcmp(class_name, "StartUpDlgClass"))
						return hwnd;
				}
				break;

			case window_type::maplestory:
				{
					if (!strcmp(class_name, "MapleStoryClass"))
						return hwnd;
				}
				break;

			default:
				break;
			}
		}

		return NULL;
	}
	
	void game_client::print_login_error(NMLoginAuthReplyCode reply_code)
	{
		std::string reply_string = "Failed to log into your account. Error code: " + std::to_string(reply_code) + "\nDescription: ";

		switch (reply_code)
		{
		case kLoginAuth_BlockedByServiceArea:
		case kLoginAuth_NotAllowedLocale:
			reply_string += "Your locale is not allowed by the server.";
			break;

		case kLoginAuth_WrongID:
			reply_string += "Your username is wrong.";
			break;

		case kLoginAuth_BlockedIP:
			reply_string += "Your IP is blocked by the server.";
			break;

		case kLoginAuth_TempBlockedByLoginFail:
		case kLoginAuth_TempBlockedByWarning:
		case kLoginAuth_BlockedByAdmin:
			reply_string += "Your account is blocked (temporarily or permanently).";
			break;

		case kLoginAuth_UserNotExists:
			reply_string += "The requested account does not exist.";
			break;

		case kLoginAuth_WrongPassword:
			reply_string += "Your password is wrong.";
			break;
			
		case kLoginAuth_GameServerInspection:
			reply_string += "The Game Server is undergoing an inspection.";
			break;

		case kLoginAuth_AlreadyLoggedIn:
			reply_string += "The requested account is already logged in.";
			break;
			
		case kLoginAuth_PasswordExpired:
			reply_string += "Your password has expired.";
			break;
			
		case kLoginAuth_NotVerified:
			reply_string += "Your account is not verified.";
			break;
			
		case kLoginAuth_BlockedBySuspiciousIP:
			reply_string += "Your account has been blocked for suspicious IP.";
			break;

		case kLoginAuth_BlockedByLongInactivated:
		case kLoginAuth_ExpiredByLongInactivated:
			reply_string += "Your account has expired (or been blocked) for too long inactivity.";
			break;
			
		case kLoginAuth_EMailVerify:
			reply_string += "You have not verified your E-mail.";
			break;

		default:
			reply_string += "Unknown error.";
			break;
		}

		MessageBox(NULL, reply_string.c_str(), "MapleStory Login Error", MB_OK | MB_ICONERROR | MB_SETFOREGROUND | MB_TOPMOST);
	}
	
	void game_client::print_launch_error(unsigned int error_code)
	{
		std::string reply_string = "Failed to launch MapleStory. Error code: " + std::to_string(error_code) + "\nDescription: ";

		switch (error_code)
		{
		case ERROR_FILE_NOT_FOUND:
			reply_string += "File not found.";
			break;

		default:
			reply_string += "Unknown error.";
			break;
		}

		MessageBox(NULL, reply_string.c_str(), "MapleStory Launch Error", MB_OK | MB_ICONERROR | MB_SETFOREGROUND | MB_TOPMOST);
	}
}