#pragma once

#include "native.hpp"

namespace firefly
{
	enum class botting_type
	{
		auto_hp = 0,
		auto_mp,

		auto_feed_pet,

		auto_loot,
		auto_attack,
		auto_timed_skill_1,
		auto_timed_skill_2,
		auto_timed_skill_3,
		auto_timed_skill_4,

		auto_ability_points,

		auto_familiar,

		auto_job_mount,

		auto_evade,
		auto_refresh,
		custom_channel_pool,
		allow_names,

		botting_type_count
	};

}