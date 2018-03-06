#pragma once

#include "native.hpp"

#include "net_buffer.hpp"

#include <string>

namespace firefly
{
	class client_socket_base
	{
	public:
		SOCKET sock() const;

	protected:
		client_socket_base();
		virtual ~client_socket_base();

		bool initialize_socket(HWND hWnd, unsigned int message_id);
		bool uninitialize_socket();
		
		bool connect_socket(std::string const& ip, unsigned short port);
		
		bool read_handshake(net_buffer& handshake);
		bool read_packets(std::vector<net_buffer>& packets, unsigned int& iv);
		
		bool write_packet(net_buffer& packet, unsigned short version_major, unsigned int& iv);
		bool write_flush();

		void reset_buffers();

	private:
		bool raw_recv(unsigned char* buffer, std::size_t size);

	private:
		SOCKET sock_;
		std::vector<unsigned char> read_buffer;
		std::vector<unsigned char> write_buffer;
	};
}