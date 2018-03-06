#pragma once

#include "native.hpp"
#include "addresses.hpp"

#include "stat_objects.hpp"
#include "generic_hook_data.hpp"

#include "TSingleton.hpp"

namespace firefly
{
	enum class ui_types
	{
		UI_NOT_DEFINED = -1,
		UI_ITEM = 0x0,
		UI_EQUIP = 0x1,
		UI_STAT = 0x2,
		UI_SKILL = 0x3,
		UI_MINIMAP = 0x4,
		UI_KEYCONFIG = 0x5,
		UI_QUESTINFO = 0x6,
		UI_USERLIST = 0x7,
		UI_MESSENGER = 0x8,
		UI_MONSTERBOOK = 0x9,
		UI_USERINFO = 0xA,
		UI_SHORTCUT = 0xB,
		UI_MENU = 0xC,
	};

	class CWvsContext : public TSingleton<CWvsContext, TSingleton_CWvsContext>
	{
		~CWvsContext() = delete;

	public:
		int get_channel()
		{
			return this->m_nChannelID;
		}
		
		CharacterData* get_character_data()
		{
			return this->m_pCharacterData.p;
		}
		
		int get_max_hp()
		{
			return this->m_basicStat.get_max_hp();
		}

		int get_max_mp()
		{
			return this->m_basicStat.get_max_mp();
		}

		void toggle_ui(ui_types ui_type, int default_tab = -1)
		{
			return CWvsContext__UI_Toggle(this, nullptr, static_cast<int>(ui_type), default_tab);
		}

		void send_spawn_familiar_request(unsigned int type_id)
		{
			return CWvsContext__SendSpawnFamiliarRequest(this, nullptr, type_id);
		}

	private:
		padding(TSingleton_CWvsContext_nChannelID_offset);
		int m_nChannelID;
		padding_sub(TSingleton_CWvsContext_pCharacterData_offset, TSingleton_CWvsContext_nChannelID_offset + sizeof(int) + sizeof(void*));
		ZRef<CharacterData> m_pCharacterData;
		BasicStat m_basicStat;
	};

	class CAntiRepeat
	{
		~CAntiRepeat() = delete;

	public:
		int m_nX;
		int m_nY;
		int m_nRepeatCount;
		int m_nCountLimit;
	};

	static_assert_size(sizeof(CWvsContext), TSingleton_CWvsContext_pCharacterData_offset + 4 + sizeof(BasicStat));
	static_assert_size(sizeof(CAntiRepeat), 0x10);
}