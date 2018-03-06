#pragma once

#include "native.hpp"
#include "net_buffer.hpp"
#include "net_opcodes.hpp"

#include "exclusive_types.hpp"

namespace firefly
{
	class exclusive_data
	{
	public:
		static exclusive_data& get_instance()
		{
			static exclusive_data instance;
			return instance;
		}

		void handle_packet(net_buffer& packet);
		void handle_type(exclusive_type type, net_buffer& packet);

		bool enabled() const;
		
		bool enable_pet_loot() const;
		unsigned short pet_loot_delay() const;
		unsigned int& pet_loot_last_tick(unsigned int id);

		bool enable_fullmap_attack() const;
		unsigned int fullmap_attack_skill_id() const;

		bool enable_skill_inject() const;
		unsigned int skill_inject_skill_id() const;
		unsigned int skill_inject_delay() const;
		unsigned int& skill_inject_last_tick();

		bool enable_dupex() const;
		int dupex_x() const;
		int dupex_y() const;
		
	private:
		exclusive_data();

	private:
		bool enabled_;
		
		bool enable_pet_loot_;
		unsigned short pet_loot_delay_;
		unsigned int pet_loot_last_tick_[3];

		bool enable_fullmap_attack_;
		unsigned int fullmap_attack_skill_id_;

		bool enable_skill_inject_;
		unsigned int skill_inject_skill_id_;
		unsigned int skill_inject_delay_;
		unsigned int skill_inject_last_tick_;
		
		bool enable_dupex_;
		int dupex_x_;
		int dupex_y_;
	};
}