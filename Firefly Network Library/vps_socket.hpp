#pragma once

#include "net_socket_sync.hpp"
#include "vps_data.hpp"

#include <functional>

namespace firefly
{
	enum class error_type
	{
		initialize,
		connect,
		read,
		write
	};

	class vps_socket : public net_socket_sync
	{
		typedef std::function<bool(vps_socket*)> interaction_handler;

	public:
		vps_socket(std::string const& hostname, unsigned short port);
		~vps_socket();
		
		bool make_vps_interaction(auth_type type, auth_version version_info, void* data, std::size_t length, interaction_handler finalize_handler = [](firefly::vps_socket* client) -> bool { return true; });
		
		bool read_packet(net_buffer& packet, bool decrypt = true);

	private:
		bool exchange_keys();
		bool check_status();
		
		void output_wsa_error(error_type type);

	private:
		aes_buffer aes;
		
		std::string hostname;
		unsigned short port;
	};
}