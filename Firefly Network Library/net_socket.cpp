#include <WinSock2.h>
#pragma comment(lib, "ws2_32")

#include "net_socket.hpp"
#include "net_opcodes.hpp"

#include <WS2tcpip.h>

#include <cryptopp\osrng.h>
#include <cryptopp\aes.h>
#include <cryptopp\modes.h>
#include <cryptopp\filters.h>

#ifdef _DEBUG
#pragma comment(lib, "cryptopp\\cryptlib_debug_dll")
#else
#pragma comment(lib, "cryptopp\\cryptlib_release_dll")
#endif

namespace firefly
{
	/* desc_info */
	desc_info::desc_info(SOCKET desc, aes_buffer aes)
		: desc_(desc), aes_(aes)
	{

	}

	SOCKET desc_info::desc() const
	{
		return this->desc_;
	}

	aes_buffer desc_info::aes() const
	{
		return this->aes_;
	}

	/* net_socket */
	bool net_socket::connect_socket(std::string const& identifier, unsigned short port, identifier_type type)
	{
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);

		switch (type)
		{
		case identifier_type::ip:
			addr.sin_addr.S_un.S_addr = inet_addr(identifier.c_str());
			break;

		case identifier_type::hostname:
			{
				addrinfo addr_type;
				memset(&addr_type, 0, sizeof(addrinfo));

				addr_type.ai_socktype = SOCK_STREAM;
				addr_type.ai_family = AF_INET;

				addrinfo* addr_host;

				if (getaddrinfo(identifier.c_str(), NULL, &addr_type, &addr_host) != 0)
					return false;
				
				addr.sin_addr.S_un.S_addr = reinterpret_cast<sockaddr_in*>(addr_host->ai_addr)->sin_addr.S_un.S_addr;
				freeaddrinfo(addr_host);
			}
			break;

		default:
			break;
		}

		std::fill(addr.sin_zero, addr.sin_zero + sizeof(addr.sin_zero), 0);

		if (connect(this->sock_, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr_in)) == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
				return false;
		}

		return true;
	}

	unsigned short net_socket::bind_socket(unsigned short port)
	{
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.S_un.S_addr = INADDR_ANY;
		std::fill(addr.sin_zero, addr.sin_zero + sizeof(addr.sin_zero), 0);

		if (bind(this->sock_, reinterpret_cast<const sockaddr*>(&addr), sizeof(sockaddr_in)) == SOCKET_ERROR)
			return -1;

		sockaddr_in name_addr;
		int addr_length = sizeof(name_addr);

		if (getsockname(this->sock_, reinterpret_cast<sockaddr*>(&name_addr), &addr_length) == SOCKET_ERROR)
			return -1;

		if (name_addr.sin_family != AF_INET || addr_length != sizeof(name_addr))
			return -1;

		return ntohs(name_addr.sin_port);
	}

	bool net_socket::listen_socket()
	{
		return (listen(this->sock_, SOMAXCONN) != SOCKET_ERROR);
	}

	SOCKET net_socket::sock() const
	{
		return this->sock_;
	}

	net_socket::net_socket()
		: sock_(INVALID_SOCKET)
	{

	}

	net_socket::~net_socket()
	{
		if (this->sock_ != INVALID_SOCKET)
			this->uninitialize_net_socket();
	}

	bool net_socket::initialize_net_socket(bool server)
	{
		if ((this->sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
			return false;

		if (server)
		{
			char opt = 1;

			if (setsockopt(this->sock_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == SOCKET_ERROR)
				return false;
		}

		return true;
	}

	bool net_socket::uninitialize_net_socket()
	{
		if (shutdown(this->sock_, SD_BOTH) == SOCKET_ERROR)
			return false;

		if (closesocket(this->sock_) == SOCKET_ERROR)
			return false;

		this->sock_ = INVALID_SOCKET;
		return true;
	}
}