#pragma once

#include "client_socket_base.hpp"

#include "base_stage.hpp"
#include "account.hpp"

#include <memory>

namespace firefly
{
	enum class server_type
	{
		none,
        login,
		game,
	};

	class client_socket : public client_socket_base
	{
	public:
		bool send_packet(net_buffer& packet);
		
		bool migrate(server_type type);
		bool migrate(server_type type, std::string const& ip, unsigned short port);

	protected:
		client_socket();
		~client_socket();
		
		void reset();
		void prepare_connection(HWND hWnd);
		
		bool on_socket_message(SOCKET sock, unsigned short select_event, unsigned short select_error);
		
	protected:
		virtual bool on_connect(SOCKET sock) = 0;
		virtual bool on_close(SOCKET sock) = 0;
		
	private:
		bool try_connect(std::string const& ip, unsigned short port);
		bool try_disconnect();

		bool on_read(SOCKET sock);
		bool on_write(SOCKET sock);

		bool process_handshake();
		bool process_packet(net_buffer& packet);

	private:
		bool on_alive_req(net_buffer& packet);
		bool on_security_packet(net_buffer& packet);
		bool on_header_encryption_packet(net_buffer& packet);

	protected:
		maplestory_account account;
		maplestory_session session;

	private:
		HWND hWnd;
		std::unique_ptr<base_stage> stage;
		
	private:
		bool handshake;

		unsigned char locale;

		unsigned short version_major;
		unsigned short version_minor;

		unsigned int send_iv;
		unsigned int recv_iv;
	};
}