#pragma once

#include <string>

namespace firefly
{
	struct maplestory_account
	{
		maplestory_account()
		{
			this->character_id = -1;
		}

		std::string username;
		std::string password;
		std::string pic;

		unsigned int world_id;
		unsigned int physical_world_id;

		unsigned int channel_id;

		unsigned int character_id;
		std::string character_name;
	};
	
	struct maplestory_session
	{
		unsigned __int64 session_id;
		unsigned int account_id;

		unsigned int character_id;
		std::string character_name;
	};
}