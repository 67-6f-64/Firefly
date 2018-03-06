#include "kami_data.hpp"

namespace firefly
{
	void kami_data::handle_packet(net_buffer& packet)
	{
		this->enabled_ = (packet.read1() != FALSE);
		
		if (this->enabled_)
		{
			this->horizontal_range_ = static_cast<short>(packet.read2());
			this->vertical_range_ = static_cast<short>(packet.read2());
			
			this->movement_type_ = static_cast<kami_movement_type_ids>(packet.read1());
			this->target_type_ = static_cast<kami_target_type_ids>(packet.read1());

			this->enable_item_loot_ = (packet.read1() != FALSE);

			if (this->enable_item_loot_)
			{
				this->item_loot_value_ = packet.read2();
				this->item_loot_target_type_ = static_cast<kami_target_type_ids>(packet.read1());
			}
		}
	}

	bool kami_data::enabled() const
	{
		return this->enabled_;
	}
	
	short kami_data::horizontal_range() const
	{
		return this->horizontal_range_;
	}

	short kami_data::vertical_range() const
	{
		return this->vertical_range_;
	}
		
	kami_movement_type_ids kami_data::movement_type() const
	{
		return this->movement_type_;
	}

	kami_target_type_ids kami_data::target_type() const
	{
		return this->target_type_;
	}

	bool kami_data::enable_item_loot() const
	{
		return this->enable_item_loot_;
	}

	unsigned short kami_data::item_loot_value() const
	{
		return this->item_loot_value_;
	}

	kami_target_type_ids kami_data::item_loot_target_type() const
	{
		return this->item_loot_target_type_;
	}

	bool kami_data::is_item_looting() const
	{
		return this->item_looting_;
	}

	void kami_data::set_item_looting(bool enable)
	{
		this->item_looting_ = enable;
	}

	kami_data::kami_data()
	{
		this->enabled_ = false;
		
		this->horizontal_range_ = 0;
		this->vertical_range_ = 0;
		
		this->movement_type_ = kami_movement_type_ids::portal;
		this->target_type_ = kami_target_type_ids::random;

		this->enable_item_loot_ = false;
		this->item_loot_value_ = 0;
		this->item_loot_target_type_ = kami_target_type_ids::random;

		this->item_looting_ = false;
	}
}