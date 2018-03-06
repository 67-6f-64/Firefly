#pragma once

#include "native.hpp"

#include "aes_constants.hpp"
#include "net_buffer.hpp"

#include <string>

namespace firefly
{
	enum class identifier_type
	{
		ip,
		hostname,		
	};

	class desc_info
	{
	public:
		desc_info(SOCKET desc, aes_buffer aes);

		SOCKET desc() const;
		aes_buffer aes() const;

	public:
		SOCKET desc_;
		aes_buffer aes_;
	};

	class net_socket
	{
	public:
		bool connect_socket(std::string const& identifier, unsigned short port, identifier_type type = identifier_type::ip);

		unsigned short bind_socket(unsigned short port = 0);
		bool listen_socket();

		SOCKET sock() const;

	protected:
		net_socket();
		~net_socket();

		bool initialize_net_socket(bool server);
		bool uninitialize_net_socket();

	private:
		SOCKET sock_;
	};
}