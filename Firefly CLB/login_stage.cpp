#include "login_stage.hpp"

#include "opcodes.hpp"
#include "system_info.hpp"

#include "utility.hpp"

#include "nmco_api\NMCOClientObject.h"

namespace firefly
{
	bool login_stage::handle_handshake(unsigned char locale, unsigned short version_major, unsigned short version_minor)
	{
		net_buffer out_packet(opcode::out::CP_PermissionRequest);
		out_packet.write1(locale);
		out_packet.write2(version_major);
		out_packet.write2(version_minor);
		out_packet.write1(0x00);

		return this->sock->send_packet(out_packet);
	}

	bool login_stage::handle_packet(unsigned short header, net_buffer& packet)
	{
		switch (header)
		{
		case opcode::in::LP_WorldInformation:
			return this->on_world_information(packet);
			
		case opcode::in::LP_AccountInfoResult:
			return this->on_account_info_result(packet);

		case opcode::in::LP_SetPhysicalWorldID:
			return this->on_set_physical_world_id(packet);

		case opcode::in::LP_SelectWorldResult:
			return this->on_select_world_result(packet);

		case opcode::in::LP_SelectCharacterResult:
			return this->on_select_character_result(packet);
			
		default:
			break;
		}

		return true;
	}
	
	bool login_stage::on_enter_stage()
	{
		printf("[login_stage] entering stage\n");
		return this->send_world_info_request();
	}
	
	bool login_stage::send_world_info_request()
	{
		this->worlds.clear();

		net_buffer packet(opcode::out::CP_WorldInfoRequest);
		return this->sock->send_packet(packet);
	}
	
	bool login_stage::on_world_information(net_buffer& packet)
	{
		world_item world;
		world.id = packet.read1();

		if (world.id == 0xFF)
			return this->send_select_world();
		else
		{
			world.name = packet.read_string();
			world.state = packet.read1();
			world.event_description = packet.read_string();
			world.event_exp_rate = packet.read2();
			world.event_drop_rate = packet.read2();
			world.block_character_creation = packet.read1();
			world.channel_count = packet.read1();

			for (unsigned char i = 0; i < world.channel_count; i++)
			{
				channel_item channel;
				channel.name = packet.read_string();
				channel.user_number = packet.read4();
				channel.world_id = packet.read1();
				channel.id = packet.read1();
				channel.adult_channel = packet.read1();

				world.channels.push_back(channel);
			}

			printf("-> World (%d): %s\n", world.id, world.name.c_str());
			this->worlds.push_back(world);
		}

		return true;
	}
	
	bool login_stage::send_select_world()
	{
		NMLoginAuthReplyCode auth_reply;

		if ((auth_reply = CNMCOClientObject::GetInstance().LoginAuth(this->account.username.c_str(), this->account.password.c_str())) != kLoginAuth_OK)
		{
			switch (auth_reply)
			{
			case kLoginAuth_BlockedIP:
				printf("Couldn't log in (blocked IP)...\n");
				break;

			case kLoginAuth_WrongID:
			case kLoginAuth_UserNotExists:
				printf("Couldn't log in (wrong username)...\n");
				break;

			case kLoginAuth_WrongPassword:
				printf("Couldn't log in (wrong password)...\n");
				break;

			default:
				printf("Couldn't log in (unknown reason)...\n");
				break;
			}

			return false;
		}

		char nexon_passport[1024];
		memset(nexon_passport, 0, sizeof(nexon_passport));
	
		if (!CNMCOClientObject::GetInstance().GetNexonPassport(nexon_passport))
			return false;
		
		unsigned char machine_id[16];
		system_info::get_machine_id(machine_id);

		net_buffer packet(opcode::out::CP_SelectWorld);
		packet.write1(game_start_mode::nexon_launcher);							// nGameStartMode

		packet.write_string(std::string(nexon_passport));
		packet.write_buffer(machine_id, sizeof(machine_id));					// CSystemInfo::GetMachineId
		packet.write4(0x00000000);												// CSystemInfo::GetGameRoomClient
		packet.write1(game_start_mode::nexon_launcher);							// nGameStartMode

		packet.write1(this->account.world_id);									// nWorldID
		packet.write1(this->account.channel_id);								// nChannelID
		packet.write4(system_info::get_local_ip_address(this->sock->sock()));	// nSockAddr

		return this->sock->send_packet(packet);
	}

	bool login_stage::on_account_info_result(net_buffer& packet)
	{
		if (!CNMCOClientObject::GetInstance().LogoutAuth())
			return false;

		if (packet.read1())														// nErrCode
		{
			printf("[login_stage::on_account_info_result] Failed to log in...\n");
			return false;
		}

		this->session.account_id = packet.read4();								// dwAccountId
		packet.read1();															// nGender
		packet.read1();															// nGradeCode
		packet.read4();															// nAccountFlags (bits: 4 = bManagerAccount, 5 = bTesterAccount, 13 = bSubTesterAccount)
		packet.read_buffer(nullptr, 4);											// nVIPGrade
		packet.read1();															// <unknown>
		packet.read_string();													// sNexonClubID
		packet.read1();															// nPurchaseExp
		packet.read1();															// nChatBlockReason
		packet.read_buffer(nullptr, 8);											// dtChatUnblockDate
		packet.read_string();													// sEMailAccount
		packet.read_buffer(nullptr, 8);											// dtRegisterDate
		packet.read4();															// nNumOfCharacter
		this->session.session_id = packet.read8();								// clientKey
		return true;
	}

	bool login_stage::on_set_physical_world_id(net_buffer& packet)
	{
		this->account.physical_world_id = packet.read4();
		return true;
	}

	bool login_stage::on_select_world_result(net_buffer& packet)
	{
		this->characters.clear();

		if (packet.read1())														// nErrCode
		{
			printf("[login_stage::on_select_world_result] Failed to select world...\n");
			return false;
		}

		packet.read_string();													// sWorldType
		packet.read4();															// nTrunkSlotCount
		packet.read1();															// bBurningEventBlock
		int delete_count = packet.read4();										// nReservedDeleteCharacterCount
		packet.read_buffer(nullptr, 8);											// tTimeNow

		for (int i = 0; i < delete_count; i++)
		{
			packet.read4();														// dwReservedDeleteCharacterId
			packet.read_buffer(nullptr, 8);										// ftReservedDeleteCharacterTime
		}
			
		packet.read1();															// bIsEditedList
		int select_count = packet.read4();										// nCharacterSelectCount

		for (int i = 0; i < select_count; i++)
		{
			packet.read4();														// dwCharacterId
		}

		unsigned char character_count = packet.read1();							// nCharacterCount

		if (character_count < 0)
		{
			printf("-> There's no characters available on this server.\n");
			return true;
		}

		for (unsigned char i = 0; i < character_count; i++)
		{
			character_info character_stat;
			character_stat.decode(packet);
				
			character_look avatar_look;
			int nJob = avatar_look.decode(packet);

			if (job::is_zero_job(nJob))
			{
				character_look zero_sub_avatar_look;
				zero_sub_avatar_look.decode(packet);
			}
				
			packet.read1();														// abOnFamily.a (ZArray)
			unsigned char on_ranking = packet.read1();

			if (on_ranking)
				packet.read_buffer(nullptr, 16);								// aRank.a (ZArray)
				
			if (utility::upper(character_stat.sCharacterName).compare(utility::upper(this->account.character_name)) == 0)
				this->account.character_id = i;

			printf("-> Character (%d): %s\n", i, character_stat.sCharacterName);
			this->characters.push_back(character_stat);
		}

		if (this->account.character_id == -1)
		{
			printf("The requested character could not be found..\n");
			return true;
		}

		return (this->send_check_second_password_request() && this->send_client_loading_time_log());
	}
	
	bool login_stage::send_check_second_password_request()
	{
		net_buffer packet(opcode::out::CP_CheckSPWRequest);
		packet.write_string(this->account.pic);
		packet.write4(this->characters[this->account.character_id].dwCharacterID);
		packet.write1(this->offline_mode);
		packet.write_string(system_info::get_local_mac_address());
		packet.write_string(system_info::get_local_serials());

		return this->sock->send_packet(packet);
	}

	bool login_stage::send_client_loading_time_log()
	{			
		net_buffer packet(opcode::out::CP_ClientLodingTimeLog);
		packet.write4(this->characters[this->account.character_id].dwCharacterID);
		packet.write_string(std::string(this->characters[this->account.character_id].sCharacterName));
		packet.write4(0x00002BE8);								// tLoadingTime[0] - Loading time for CLoadClientDataThread::InitializeGameData
		packet.write4(0x00001A5A);								// tLoadingTime[1] - Loading time for CLoadClientDataThread::InitializeSkillData
		packet.write4(0x000022BF);								// tLoadingTime[2] - Loading time for CLoadClientDataThread::InitializeETCData
		packet.write4(0x00001A4E);								// tLoadingTime[3] - Loading time for CLoadClientDataThread::InitializeQuestData
		packet.write4(0x0000990C);								// tLaunchingDelta (nLaunchingEndTime - nLaunchingStartTime)
			
		SYSTEMTIME st;
		GetSystemTime(&st);

		packet.write_buffer(reinterpret_cast<unsigned char*>(&st), sizeof(SYSTEMTIME));	// stStartTime

		st.wMinute += 1;
		st.wSecond = (rand() % 60);
		st.wMilliseconds = (rand() % 1000);

		packet.write_buffer(reinterpret_cast<unsigned char*>(&st), sizeof(SYSTEMTIME));	// stEndTime

		return this->sock->send_packet(packet);
	}

	bool login_stage::on_select_character_result(net_buffer& packet)
	{
		packet.read1();															// nErrCode
		packet.read1();															// nBaseStep
	
		unsigned int game_ip = packet.read4();									// uGameIp
		unsigned short game_port = packet.read2();								// uGamePort

		unsigned int chat_ip = packet.read4();									// uChatIp
		unsigned short chat_port = packet.read2();								// uChatPort

		packet.read4();															// <unknown>

		this->session.character_id = packet.read4();
		this->session.character_name = std::string(this->characters[this->account.character_id].sCharacterName, 13);

		/* game ip construction */
		char game_ip_string[16];
		memset(game_ip_string, 0, sizeof(game_ip_string));
			
		unsigned char* game_ip_buffer = reinterpret_cast<unsigned char*>(&game_ip);
		sprintf(game_ip_string, "%i.%i.%i.%i", game_ip_buffer[0], game_ip_buffer[1], game_ip_buffer[2], game_ip_buffer[3]);
	
		return this->sock->migrate(server_type::game, std::string(game_ip_string), game_port);
	}
}