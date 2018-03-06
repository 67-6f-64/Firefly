#pragma once

#include "base_stage.hpp"
#include "client_socket.hpp"

namespace firefly
{
	class game_stage : public base_stage
	{
	public:
		template <typename... Args>
		game_stage(client_socket* sock, maplestory_account& account, maplestory_session& session, Args&&... args);

		bool handle_handshake(unsigned char locale, unsigned short version_major, unsigned short version_minor);
		bool handle_packet(unsigned short header, net_buffer& packet);

		bool on_enter_stage();

	private:
		client_socket* sock;

		maplestory_account& account;
		maplestory_session& session;
	};

	template <typename... Args>
	game_stage::game_stage(client_socket* sock, maplestory_account& account, maplestory_session& session, Args&&... args)
		: account(account), session(session), base_stage(std::forward<Args>(args)...)
	{
		this->sock = sock;
	}
}