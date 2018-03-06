#pragma once

#include "native.hpp"
#include "addresses.hpp"

#include "context_objects.hpp"
#include "TSingleton.hpp"

namespace firefly
{
	class CSkillInfo : public TSingleton<CSkillInfo, TSingleton_CSkillInfo>
	{
		~CSkillInfo() = delete;

	public:
		int get_skill(int skill_id, void** skill_entry)
		{
			if (CWvsContext::IsInitialized())
			{
				void* character_data = CWvsContext::GetInstance()->get_character_data();

				if (character_data)
				{
					return CSkillInfo__GetSkillLevel(this, nullptr, character_data, skill_id, skill_entry, FALSE, FALSE, FALSE, TRUE);
				}
			}

			return 0;
		}
	};

	static_assert_size(sizeof(CSkillInfo), 0x01);
}