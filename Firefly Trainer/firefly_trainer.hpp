#pragma once

#include "net_socket_async.hpp"
#include "game_client.hpp"

#include "main_window.hpp"

#define version_major 1
#define version_minor 4

namespace firefly
{
	class firefly_trainer : public main_window
	{
	public:
		firefly_trainer(HINSTANCE hinstance, std::string const& class_name, std::string const& window_name);
		~firefly_trainer();

	protected:
		bool authenticate(std::string const& email, std::string const& password);
		bool launch_game(std::string const& directory, std::string const& username, std::string const& password);

		void handle_select_message(SOCKET sock, unsigned short select_event, unsigned short select_error, bool game = true);
		
	private:
		void on_accept(SOCKET sock);
		void on_connect(SOCKET sock);
		void on_close(SOCKET sock, bool game);
		void on_read(SOCKET sock, bool game);
		void on_write(SOCKET sock, bool game);

		bool send_game_packet(net_buffer& packet);
		void handle_packet(net_buffer& packet);

	private:
		void load_data();
		
	private:
		net_socket_async game_sock;
		unsigned short game_port;
		SOCKET game_desc;
		aes_buffer game_aes;

		net_socket_async manager_sock;
		aes_buffer manager_aes;

		game_client launcher;
	};
}