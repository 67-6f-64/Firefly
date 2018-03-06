#pragma once

#include "native.hpp"
#include "file_mapper.hpp"

#include "nmco_api\NMCOClientObject.h"
#include "nmco_api\NMManager.h"

#include <memory>
#include <string>

namespace firefly
{
	enum class window_type
	{
		splashscreen,
		maplestory,
	};

	class game_client
	{
	public:
		game_client();
		~game_client();
		
		bool start_process(std::string const& directory, std::string const& username, std::string const& password);
		bool terminate_process();
		
		void set_spy_file_map(std::vector<unsigned char>& file_data);
		void set_blackcipher_spy_file_map(std::vector<unsigned char>& file_data);

		bool send_communication_data(HWND sender, unsigned short port);
		
		unsigned int get_process_id() const;

	private:
		bool create_process(std::string const& directory, std::string const& commandline, unsigned int* error_code);

		HWND get_window_handle(window_type type);
		
		void print_login_error(NMLoginAuthReplyCode reply_code);
		void print_launch_error(unsigned int error_code);

	private:
		STARTUPINFO startup_info;
		PROCESS_INFORMATION process_info;
		
		std::unique_ptr<io::file_mapper> spy_file_map;
		std::unique_ptr<io::file_mapper> blackcipher_spy_file_map;
	};
}