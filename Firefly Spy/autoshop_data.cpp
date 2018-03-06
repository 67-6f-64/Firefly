#include "autoshop_data.hpp"

namespace firefly
{
	void autoshop_data::handle_packet(net_buffer& packet)
	{
		autoshop_type_ids type = static_cast<autoshop_type_ids>(packet.read1());

		switch (type)
		{
		case autoshop_type_ids::autoshop_toggle:
			return this->handle_toggle(packet);

		case autoshop_type_ids::autoshop_test_transfer_rush:
			return this->handle_test_transfer_rush(packet);

		case autoshop_type_ids::autoshop_test_sell_rush:
			return this->handle_test_sell_rush(packet);
			
		case autoshop_type_ids::autoshop_type_unknown:
		default:
			break;
		}
	}

	void autoshop_data::handle_toggle(net_buffer& packet)
	{
		this->enabled_ = (packet.read1() != FALSE);
		
		if (this->enabled_)
		{
			this->auto_pot_hp_restock_ = (packet.read1() != FALSE);

			if (this->auto_pot_hp_restock_)
			{
				this->auto_pot_restock_hp_from_ = packet.read2();
				this->auto_pot_restock_hp_to_ = packet.read2();
			}

			this->auto_pot_mp_restock_ = (packet.read1() != FALSE);

			if (this->auto_pot_hp_restock_)
			{
				this->auto_pot_restock_mp_from_ = packet.read2();
				this->auto_pot_restock_mp_to_ = packet.read2();
			}

			this->auto_start_bullet_restock_ = (packet.read1() != FALSE);

			this->auto_sell_ = (packet.read1() != FALSE);

			if (this->auto_sell_)
			{
				this->auto_sell_type_equip_ = (packet.read1() != FALSE);
				this->auto_sell_type_etc_ = (packet.read1() != FALSE);
				this->auto_sell_type_use_ = (packet.read1() != FALSE);

				this->auto_sell_delay_ = packet.read4();
				
				this->auto_sell_filter_type_potential_ = (packet.read1() != FALSE);
				this->auto_sell_filter_type_stars_ = (packet.read1() != FALSE);
				this->auto_sell_filter_type_custom_ = (packet.read1() != FALSE);
			}

			this->auto_transfer_mesos_ = (packet.read1() != FALSE);

			if (this->auto_transfer_mesos_)
			{		
				this->auto_transfer_mesos_amount_ = packet.read4();
				this->auto_transfer_mesos_limit_ = packet.read4();
				this->auto_transfer_mesos_delay_ = packet.read4();
		
				this->auto_transfer_channel_ = packet.read1();
				this->auto_transfer_free_market_ = packet.read1();
		
				this->auto_transfer_shop_id_ = packet.read4();
				this->auto_transfer_item_id_ = packet.read4();
				this->auto_transfer_item_price_ = packet.read4();
			}
		}
	}

	void autoshop_data::handle_test_transfer_rush(net_buffer& packet)
	{
		if (!this->auto_transfer_mesos_test_)
		{
			this->auto_transfer_mesos_test_ = true;

			this->auto_transfer_mesos_amount_ = packet.read4();
			this->auto_transfer_mesos_limit_ = packet.read4();
			this->auto_transfer_mesos_delay_ = packet.read4();
		
			this->auto_transfer_channel_ = packet.read1();
			this->auto_transfer_free_market_ = packet.read1();
		
			this->auto_transfer_shop_id_ = packet.read4();
			this->auto_transfer_item_id_ = packet.read4();
			this->auto_transfer_item_price_ = packet.read4();
		}
	}

	void autoshop_data::handle_test_sell_rush(net_buffer& packet)
	{
		this->auto_sell_test_ = (packet.read1() != FALSE);
	
		if (this->auto_sell_test_)
		{
			this->auto_sell_type_equip_ = (packet.read1() != FALSE);
			this->auto_sell_type_etc_ = (packet.read1() != FALSE);
			this->auto_sell_type_use_ = (packet.read1() != FALSE);

			this->auto_sell_delay_ = packet.read4();
				
			this->auto_sell_filter_type_potential_ = (packet.read1() != FALSE);
			this->auto_sell_filter_type_stars_ = (packet.read1() != FALSE);
			this->auto_sell_filter_type_custom_ = (packet.read1() != FALSE);
		}
	}
	
	bool autoshop_data::enabled() const
	{
		return this->enabled_;
	}
	
	bool autoshop_data::auto_pot_hp_restock() const
	{
		return this->auto_pot_hp_restock_;
	}

	unsigned short autoshop_data::auto_pot_restock_hp_from() const
	{
		return this->auto_pot_restock_hp_from_;
	}

	unsigned short autoshop_data::auto_pot_restock_hp_to() const
	{
		return this->auto_pot_restock_hp_to_;
	}
	
	bool autoshop_data::auto_pot_mp_restock() const
	{
		return this->auto_pot_mp_restock_;
	}

	unsigned short autoshop_data::auto_pot_restock_mp_from() const
	{
		return this->auto_pot_restock_mp_from_;
	}

	unsigned short autoshop_data::auto_pot_restock_mp_to() const
	{
		return this->auto_pot_restock_mp_to_;
	}

	bool autoshop_data::auto_start_bullet_restock() const
	{
		return this->auto_start_bullet_restock_;
	}

	bool autoshop_data::auto_sell() const
	{
		return this->auto_sell_;
	}
	
	bool autoshop_data::auto_sell_test() const
	{
		return this->auto_sell_test_;
	}

	bool autoshop_data::auto_sell_type_equip() const
	{
		return this->auto_sell_type_equip_;
	}

	bool autoshop_data::auto_sell_type_etc() const
	{
		return this->auto_sell_type_etc_;
	}

	bool autoshop_data::auto_sell_type_use() const
	{
		return this->auto_sell_type_use_;
	}

	unsigned int autoshop_data::auto_sell_delay() const
	{
		return this->auto_sell_delay_;
	}

	bool autoshop_data::auto_sell_filter_type_potential() const
	{
		return this->auto_sell_filter_type_potential_;
	}

	bool autoshop_data::auto_sell_filter_type_stars() const
	{
		return this->auto_sell_filter_type_stars_;
	}

	bool autoshop_data::auto_sell_filter_type_custom() const
	{
		return this->auto_sell_filter_type_custom_;
	}
	
	bool autoshop_data::auto_transfer_mesos() const
	{
		return this->auto_transfer_mesos_;
	}

	bool autoshop_data::auto_transfer_mesos_test() const
	{
		return this->auto_transfer_mesos_test_;
	}

	unsigned int autoshop_data::auto_transfer_mesos_amount() const
	{
		return this->auto_transfer_mesos_amount_;
	}

	unsigned int autoshop_data::auto_transfer_mesos_limit() const
	{
		return this->auto_transfer_mesos_limit_;
	}

	unsigned int autoshop_data::auto_transfer_mesos_delay() const
	{
		return this->auto_transfer_mesos_delay_;
	}

	unsigned char autoshop_data::auto_transfer_channel() const
	{
		return this->auto_transfer_channel_;
	}

	unsigned char autoshop_data::auto_transfer_free_market() const
	{
		return this->auto_transfer_free_market_;
	}

	unsigned int autoshop_data::auto_transfer_shop_id() const
	{
		return this->auto_transfer_shop_id_;
	}

	unsigned int autoshop_data::auto_transfer_item_id() const
	{
		return this->auto_transfer_item_id_;
	}

	unsigned int autoshop_data::auto_transfer_item_price() const
	{
		return this->auto_transfer_item_price_;
	}

	autoshop_data::autoshop_data()
	{
		this->enabled_ = false;

		this->auto_pot_hp_restock_ = false;
		this->auto_pot_restock_hp_from_ = 0;
		this->auto_pot_restock_hp_to_ = 0;
		
		this->auto_pot_mp_restock_ = false;
		this->auto_pot_restock_mp_from_ = 0;
		this->auto_pot_restock_mp_to_ = 0;
	
		this->auto_start_bullet_restock_ = false;

		this->auto_sell_ = false;
		this->auto_sell_test_ = false;

		this->auto_sell_type_equip_ = false;
		this->auto_sell_type_etc_ = false;
		this->auto_sell_type_use_ = false;

		this->auto_sell_delay_ = 0;
				
		this->auto_sell_filter_type_potential_ = false;
		this->auto_sell_filter_type_stars_ = false;
		this->auto_sell_filter_type_custom_ = false;
		
		this->auto_transfer_mesos_ = false;
		this->auto_transfer_mesos_test_ = false;

		this->auto_transfer_mesos_amount_ = 0;
		this->auto_transfer_mesos_limit_ = 0;
		this->auto_transfer_mesos_delay_ = 0;
		
		this->auto_transfer_channel_ = 0;
		this->auto_transfer_free_market_ = 0;
		
		this->auto_transfer_shop_id_ = 0;
		this->auto_transfer_item_id_ = 0;
		this->auto_transfer_item_price_ = 0;
	}
}