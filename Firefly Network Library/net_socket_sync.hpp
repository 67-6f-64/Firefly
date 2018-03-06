#pragma once

#include "net_socket.hpp"

namespace firefly
{
	/* assumed blocking socket */
	class net_socket_sync : public net_socket
	{
	public:
		net_socket_sync();
		~net_socket_sync();
		
		bool initialize_socket(bool server);
		bool uninitialize_socket();

		bool receive_key(desc_info& desc, aes_buffer& aes);
		
		bool read_packet(desc_info& desc, net_buffer& packet, bool decrypt = true);
		bool write_packet(desc_info& desc, net_buffer& packet, bool encrypt = true);

	protected:
		bool read_fixed(desc_info& desc, unsigned char* buffer, std::size_t assumed_size, bool decrypt);
		bool write_fixed(desc_info& desc, unsigned char* buffer, std::size_t size, bool encrypt);

		bool read_raw(desc_info& desc, unsigned char* buffer, std::size_t size, bool decrypt);
		bool write_raw(desc_info& desc, unsigned char const* buffer, std::size_t size, bool encrypt);
	};
}