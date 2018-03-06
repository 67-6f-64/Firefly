#include <WinSock2.h>
#pragma comment(lib, "ws2_32")

#include "client_socket_base.hpp"
#include "client_socket_crypto.hpp"

#include <WS2tcpip.h>

#include "output.hpp"

namespace firefly
{
	SOCKET client_socket_base::sock() const
	{
		return this->sock_;
	}

	client_socket_base::client_socket_base()
		: sock_(INVALID_SOCKET)
	{

	}

	client_socket_base::~client_socket_base()
	{
		this->uninitialize_socket();
	}

	bool client_socket_base::initialize_socket(HWND hWnd, unsigned int message_id)
	{
		if ((this->sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
			return false;

		return (WSAAsyncSelect(this->sock(), hWnd, message_id, FD_CONNECT | FD_CLOSE | FD_READ | FD_WRITE) != SOCKET_ERROR);
	}
	
	bool client_socket_base::uninitialize_socket()
	{
		if (this->sock_ != INVALID_SOCKET)
		{
			if (shutdown(this->sock_, SD_BOTH) == SOCKET_ERROR)
				return false;

			if (closesocket(this->sock_) == SOCKET_ERROR)
				return false;

			this->sock_ = INVALID_SOCKET;
		}

		return true;
	}
	
	bool client_socket_base::connect_socket(std::string const& ip, unsigned short port)
	{
		this->reset_buffers();

		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());

		std::fill(addr.sin_zero, addr.sin_zero + sizeof(addr.sin_zero), 0);

		return (connect(this->sock_, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr_in)) != SOCKET_ERROR ||
			WSAGetLastError() == WSAEWOULDBLOCK);
	}
	
	bool client_socket_base::read_handshake(net_buffer& packet)
	{
		unsigned short handshake_length = 0;

		if (!this->raw_recv(reinterpret_cast<unsigned char*>(&handshake_length), sizeof(unsigned short)))
			return false;
			
		unsigned char* handshake_buffer = new unsigned char[handshake_length];
		memset(handshake_buffer, 0, handshake_length);

		bool result = this->raw_recv(handshake_buffer, handshake_length);
			
		if (result)
			packet = net_buffer(handshake_buffer, handshake_length);

		delete[] handshake_buffer;
		return result;
	}

	bool client_socket_base::read_packets(std::vector<net_buffer>& packets, unsigned int& iv)
	{
		for (int bytes_read = 0;;)
		{
			std::vector<unsigned char> temp_buffer(4096);
			bytes_read = recv(this->sock(), reinterpret_cast<char*>(&temp_buffer[0]), temp_buffer.size(), 0);

			if (bytes_read == 0)
				break;

			if (bytes_read == SOCKET_ERROR)
			{
				if (WSAGetLastError() != WSAEWOULDBLOCK)
					return false;

				break;
			}

			this->read_buffer.insert(this->read_buffer.end(), temp_buffer.begin(), temp_buffer.begin() + bytes_read);
		}

		while (this->read_buffer.size() >= 4)
		{
			unsigned int packet_length = client_socket_crypto::get_instance().get_packet_length(this->read_buffer.data());
			unsigned int packet_length_total = sizeof(unsigned int) + packet_length;

			if (this->read_buffer.size() < packet_length_total)
				break;
						
			client_socket_crypto::get_instance().aes_crypt(&this->read_buffer[4], packet_length, iv);
			packets.push_back(net_buffer(&this->read_buffer[4], packet_length));

			this->read_buffer.erase(this->read_buffer.begin(), this->read_buffer.begin() + packet_length_total);
		}

		return true;
	}
	
	bool client_socket_base::write_packet(net_buffer& packet, unsigned short version_major, unsigned int& iv)
	{
		unsigned int packet_length = packet.get_size();
		unsigned int packet_length_total = sizeof(unsigned int) + packet_length;
		
		std::vector<unsigned char> buffer(packet_length_total);
		memset(&buffer[0], 0, sizeof(unsigned int));
		memcpy(&buffer[4], packet.get_data(), packet.get_size());

		client_socket_crypto::get_instance().create_header(&buffer[0], packet.get_size(), version_major, iv);
		client_socket_crypto::get_instance().aes_crypt(&buffer[4], packet.get_size(), iv);
		
		std::copy(buffer.begin(), buffer.end(), std::back_inserter(this->write_buffer));
		return this->write_flush();
	}
	
	bool client_socket_base::write_flush()
	{
		for (int bytes_sent = 0; this->write_buffer.size() > 0;)
		{
			bytes_sent = send(this->sock(), reinterpret_cast<char const*>(&this->write_buffer[0]), this->write_buffer.size(), 0);

			if (bytes_sent == SOCKET_ERROR)
			{
				if (WSAGetLastError() != WSAEWOULDBLOCK)
					return false;
			
				break;
			}

			this->write_buffer.erase(this->write_buffer.begin(), this->write_buffer.begin() + bytes_sent);
		}

		return true;
	}

	void client_socket_base::reset_buffers()
	{
		this->read_buffer.clear();
		this->write_buffer.clear();
	}
	
	bool client_socket_base::raw_recv(unsigned char* buffer, std::size_t size)
	{
		for (int bytes_read = 0, offset = 0, data_to_read = size; data_to_read > 0; data_to_read -= bytes_read, offset += bytes_read)
		{
			bytes_read = recv(this->sock(), reinterpret_cast<char*>(buffer) + offset, data_to_read, 0);
	
			if (bytes_read == 0)
				break;

			if (bytes_read == SOCKET_ERROR)
			{
				if (WSAGetLastError() != WSAEWOULDBLOCK)
					return false;

				break;
			}
		}

		return true;
	}
}