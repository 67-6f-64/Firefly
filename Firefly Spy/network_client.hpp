#pragma once

#include "net_socket_async.hpp"
#include "net_buffer.hpp"

namespace firefly
{
	class network_client : public net_callback
	{
	public:
		static network_client& get_instance()
		{
			static network_client instance;
			return instance;
		}

		void initialize(HMODULE module);
		void connect_client(unsigned short port);

		void on_socket_message(SOCKET desc, unsigned short select_event, unsigned short select_error);
		bool send_firefly_packet(net_buffer& packet);

	private:
		network_client();
		~network_client();

		void on_connect(SOCKET sock);
		void on_close(SOCKET sock);
		void on_read(SOCKET sock);
		void on_write(SOCKET desc);

		void handle_packet(net_buffer& packet);

		void on_aes_key(net_buffer& packet);
		void on_trainer_toggle(net_buffer& packet);
		void on_fetch_data(net_buffer& packet);

	private:
		void on_fetch_spawncontrol_data();
		void on_fetch_rusher_data(net_buffer& packet);
		void on_fetch_dupex_data();

	private:
		net_socket_async sock;
		aes_buffer aes;
	};
}