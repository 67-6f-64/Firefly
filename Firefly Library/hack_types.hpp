#pragma once

#include "native.hpp"

namespace firefly
{
	enum class hack_type
	{
		full_godmode = 0,
		s58_godmode,
		
		face_left,
		aircheck,
		no_character_knockback,
		unlimited_attack,
				
		full_disarm,
		faster_mobs,
		no_mob_animation,
		no_mob_knockback,
		suck_left,
		suck_right,
		auto_aggro,
		mob_freeze,

		no_background,
		no_structures_platforms,
		no_fadestages,
		no_hit_effect,
		no_damage_text,
		no_kill_text,
		
		auto_npc,
		auto_exit_cashshop,
		auto_rune,
		auto_sudden_mission,

		no_blue_boxes,
		no_falling_objects,
		super_tubi,
		semi_itemvac,
		familiar_disarm,

		hack_type_count
	};
}