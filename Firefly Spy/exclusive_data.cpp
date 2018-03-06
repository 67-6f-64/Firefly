#include "exclusive_data.hpp"

namespace firefly
{
	void exclusive_data::handle_packet(net_buffer& packet)
	{
		unsigned char index = packet.read1();

		if (index == static_cast<unsigned char>(-1))
		{
			if (this->enabled_ = (packet.read1() != FALSE))
			{
				this->handle_type(exclusive_type::pet_loot, packet);
				this->handle_type(exclusive_type::fullmap_attack, packet);
				this->handle_type(exclusive_type::skill_inject, packet);
				this->handle_type(exclusive_type::dupex, packet);
			}
		}
		else
		{
			return this->handle_type(static_cast<exclusive_type>(index), packet);
		}
	}

	void exclusive_data::handle_type(exclusive_type type, net_buffer& packet)
	{
		switch (type)
		{
		case exclusive_type::pet_loot:
			if (this->enable_pet_loot_ = (packet.read1() != FALSE))
				this->pet_loot_delay_ = packet.read2();

			break;

		case exclusive_type::fullmap_attack:
			if (this->enable_fullmap_attack_ = (packet.read1() != FALSE))
				this->fullmap_attack_skill_id_ = packet.read4();

			break;

		case exclusive_type::skill_inject:
			if (this->enable_skill_inject_ = (packet.read1() != FALSE))
			{
				this->skill_inject_skill_id_ = packet.read4();
				this->skill_inject_delay_ = packet.read4();
				this->skill_inject_last_tick_ = 0;
			}

			break;

		case exclusive_type::dupex:
			if (this->enable_dupex_ = (packet.read1() != FALSE))
			{
				this->dupex_x_ = static_cast<int>(packet.read4());
				this->dupex_y_ = static_cast<int>(packet.read4());
			}

			break;

		default:
			break;
		}
	}

	bool exclusive_data::enabled() const
	{
		return this->enabled_;
	}

	bool exclusive_data::enable_pet_loot() const
	{
		return this->enable_pet_loot_;
	}

	unsigned short exclusive_data::pet_loot_delay() const
	{
		return this->pet_loot_delay_;
	}

	unsigned int& exclusive_data::pet_loot_last_tick(unsigned int id)
	{
		if (id > _countof(this->pet_loot_last_tick_))
			id = 0;

		return this->pet_loot_last_tick_[id];
	}

	bool exclusive_data::enable_fullmap_attack() const
	{
		return this->enable_fullmap_attack_;
	}

	unsigned int exclusive_data::fullmap_attack_skill_id() const
	{
		return this->fullmap_attack_skill_id_;
	}

	bool exclusive_data::enable_skill_inject() const
	{
		return this->enable_skill_inject_;
	}

	unsigned int exclusive_data::skill_inject_skill_id() const
	{
		return this->skill_inject_skill_id_;
	}

	unsigned int exclusive_data::skill_inject_delay() const
	{
		return this->skill_inject_delay_;
	}
	
	unsigned int& exclusive_data::skill_inject_last_tick()
	{
		return this->skill_inject_last_tick_;
	}

	bool exclusive_data::enable_dupex() const
	{
		return this->enable_dupex_;
	}

	int exclusive_data::dupex_x() const
	{
		return this->dupex_x_;
	}

	int exclusive_data::dupex_y() const
	{
		return this->dupex_y_;
	}

	exclusive_data::exclusive_data()
	{
		this->enabled_ = false;
		
		this->enable_pet_loot_ = false;
		this->pet_loot_delay_ = 0;
		memset(this->pet_loot_last_tick_, 0, sizeof(this->pet_loot_last_tick_));
		
		this->enable_fullmap_attack_ = false;
		this->fullmap_attack_skill_id_ = 0;

		this->enable_skill_inject_ = false;
		this->skill_inject_skill_id_ = 0;
		this->skill_inject_delay_ = 0;
		this->skill_inject_last_tick_ = 0;

		this->enable_dupex_ = false;
		this->dupex_x_ = 0;
		this->dupex_y_ = 0;
	}
}