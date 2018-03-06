#pragma once

#include <string>

namespace firefly
{
	namespace system_info
	{
		unsigned int get_hdd_serial_number();
		unsigned int get_local_ip_address(SOCKET sock);

		std::string get_local_mac_address();
		std::string get_local_mac_serial();
		
		std::string get_local_serials();

		void get_machine_id(unsigned char* buffer);
	}
}