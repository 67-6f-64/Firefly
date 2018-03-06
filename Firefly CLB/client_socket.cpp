#include "client_socket.hpp"

#include "game_stage.hpp"
#include "login_stage.hpp"

#include "config.hpp"
#include "opcodes.hpp"

#include "output.hpp"

#include <cryptopp\osrng.h>
#include <cryptopp\des.h>
#include <cryptopp\modes.h>
#include <cryptopp\filters.h>

#ifdef _DEBUG
#pragma comment(lib, "cryptopp\\cryptlib_debug_dll")
#else
#pragma comment(lib, "cryptopp\\cryptlib_release_dll")
#endif

namespace firefly
{
	bool client_socket::send_packet(net_buffer& packet)
	{
		return this->write_packet(packet, this->version_major, this->send_iv);
	}

	client_socket::client_socket()
	{

	}
	
	client_socket::~client_socket()
	{
		printf("~client_socket()\n");
	}
	
	void client_socket::reset()
	{
		this->stage.reset();
	}

	void client_socket::prepare_connection(HWND hWnd)
	{
		this->hWnd = hWnd;
	}
	
	bool client_socket::migrate(server_type type)
	{
		return this->migrate(type, config::server::europe::ip, config::server::europe::port);
	}

	bool client_socket::migrate(server_type type, std::string const& ip, unsigned short port)
	{
		printf("Migrating to %s : %d\n", ip.c_str(), port);
		this->stage.reset();
			
		switch (type)
		{
		case server_type::game:
			this->stage = std::make_unique<game_stage>(this, this->account, this->session);
			break;

		case server_type::login:
			this->stage = std::make_unique<login_stage>(this, this->account, this->session);
			break;

		default:
			printf("Unknown stage requested: %d\n", type);
			break;
		}
		
		return (this->stage != nullptr && this->try_connect(ip, port));
	}
	
	bool client_socket::on_socket_message(SOCKET sock, unsigned short select_event, unsigned short select_error)
	{
		if (select_error)
		{
			switch (select_error)
			{
			case WSAECONNABORTED:
				return this->on_close(sock);

			default:
				printf("[on_socket_message] Async select error: %d\n", select_error);
				break;
			}
		}
		else
		{
			switch (select_event)
			{
			case FD_CONNECT:
				return this->on_connect(sock);

			case FD_CLOSE:
				return this->on_close(sock);

			case FD_READ:
				return this->on_read(sock);

			case FD_WRITE:
				return this->on_write(sock);

			default:
				break;
			}
		}

		return false;
	}
	
	bool client_socket::try_connect(std::string const& ip, unsigned short port)
	{
		this->handshake = false;

		if (this->sock() != INVALID_SOCKET)
		{
			if (!this->uninitialize_socket())
				return false;
		}

		return (this->initialize_socket(this->hWnd, WM_SOCKET) && 
			this->connect_socket(ip, port));
	}

	bool client_socket::try_disconnect()
	{
		return this->uninitialize_socket();
	}

	bool client_socket::on_read(SOCKET sock)
	{
		if (!this->handshake)
		{
			if (!(this->handshake = this->process_handshake()))
				return false;
			
			if (this->stage)
			{
				if (!this->stage.get()->handle_handshake(this->locale, this->version_major, this->version_minor))
					return false;

				if (!this->stage.get()->on_enter_stage())
					return false;
			}
		}
		else
		{
			std::vector<net_buffer> packets;

			if (!this->read_packets(packets, this->recv_iv))
				return false;
			
			for (net_buffer& packet : packets)
			{
				if (!this->process_packet(packet))
					return false;
			}
		}

		return true;
	}

	bool client_socket::on_write(SOCKET sock)
	{
		return this->write_flush();
	}
	
	bool client_socket::process_handshake()
	{
		net_buffer packet;
			
		if (!this->read_handshake(packet))
			return false;

		this->version_major = packet.read2();
		this->version_minor = packet.read_string_as<unsigned short>();

		this->send_iv = packet.read4();
		this->recv_iv = packet.read4();

		this->locale = packet.read1();
		return true;
	}

	bool client_socket::process_packet(net_buffer& packet)
	{
		unsigned short header = packet.read2();

		switch (header)
		{
		case opcode::in::LP_AliveReq:
			return this->on_alive_req(packet);

		case opcode::in::LP_SecurityPacket:
			return this->on_security_packet(packet);

		case opcode::in::LP_HeaderEncryption:
			return this->on_header_encryption_packet(packet);

		default:
			return this->stage.get()->handle_packet(header, packet);
		}
		
		return true;
	}
	
	bool client_socket::on_alive_req(net_buffer& packet)
	{
		net_buffer out_packet(opcode::out::CP_AliveAck);
		out_packet.write4(GetTickCount());

		return this->send_packet(out_packet);
	}
	
	bool client_socket::on_security_packet(net_buffer& packet)
	{
		return true;
	}
	
	bool client_socket::on_header_encryption_packet(net_buffer& packet)
	{
		unsigned int buffer_xms = packet.read4();
		unsigned int buffer_length = packet.read4();

		unsigned char* buffer = new unsigned char[buffer_length];
		packet.read_buffer(buffer, buffer_length);

		//decrypt_xms_tree(buffer, buffer_length, buffer_xms);

		delete[] buffer;
		return true;
	}
}