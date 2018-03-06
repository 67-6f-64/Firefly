#pragma once

#include "base_stage.hpp"
#include "client_socket.hpp"

#include "account.hpp"
#include "character_info.hpp"

namespace firefly
{
	class login_stage : public base_stage
	{
		enum game_start_mode
		{
			none = 0,
			game_launcher = 2,
			nexon_launcher = 4,
		};

		/* CLogin::CHANNELITEM */
		struct channel_item
		{
			std::string name;
			unsigned int user_number;
			unsigned char world_id;
			unsigned char id;
			unsigned char adult_channel;
		};

		/* CLogin::WORLDITEM */
		struct world_item
		{
			unsigned char id;
			std::string name;
			unsigned char state;
			std::string event_description;
			unsigned short event_exp_rate;
			unsigned short event_drop_rate;
			unsigned char block_character_creation;
			unsigned char channel_count;
			unsigned short balloon_count;
			std::vector<channel_item> channels;
		};

	public:
		template <typename... Args>
		login_stage(client_socket* sock, maplestory_account& account, maplestory_session& session, Args&&... args);

		bool handle_handshake(unsigned char locale, unsigned short version_major, unsigned short version_minor);
		bool handle_packet(unsigned short header, net_buffer& packet);
		
		bool on_enter_stage();
		
		bool send_world_info_request();
		bool on_world_information(net_buffer& packet);
		
		bool send_select_world();
		bool on_account_info_result(net_buffer& packet);
		bool on_set_physical_world_id(net_buffer& packet);
		bool on_select_world_result(net_buffer& packet);
		
		bool send_check_second_password_request();
		bool send_client_loading_time_log();
		bool on_select_character_result(net_buffer& packet);

	private:
		client_socket* sock;
		bool offline_mode;

		maplestory_account& account;
		maplestory_session& session;

		std::vector<world_item> worlds;
		std::vector<character_info> characters;
	};

	template <typename... Args>
	login_stage::login_stage(client_socket* sock, maplestory_account& account, maplestory_session& session, Args&&... args)
		: account(account), session(session), base_stage(std::forward<Args>(args)...)
	{
		this->sock = sock;
		this->offline_mode = true;
	}
}