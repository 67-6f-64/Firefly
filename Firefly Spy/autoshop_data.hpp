#pragma once

#include "native.hpp"
#include "net_buffer.hpp"
#include "net_opcodes.hpp"

namespace firefly
{
	class autoshop_data
	{
	public:
		static autoshop_data& get_instance()
		{
			static autoshop_data instance;
			return instance;
		}

		void handle_packet(net_buffer& packet);

		void handle_toggle(net_buffer& packet);
		void handle_test_transfer_rush(net_buffer& packet);
		void handle_test_sell_rush(net_buffer& packet);

		bool enabled() const;
		
		bool auto_pot_hp_restock() const;
		unsigned short auto_pot_restock_hp_from() const;
		unsigned short auto_pot_restock_hp_to() const;
		
		bool auto_pot_mp_restock() const;
		unsigned short auto_pot_restock_mp_from() const;
		unsigned short auto_pot_restock_mp_to() const;

		bool auto_start_bullet_restock() const;

		bool auto_sell() const;
		bool auto_sell_test() const;

		bool auto_sell_type_equip() const;
		bool auto_sell_type_etc() const;
		bool auto_sell_type_use() const;

		unsigned int auto_sell_delay() const;

		bool auto_sell_filter_type_potential() const;
		bool auto_sell_filter_type_stars() const;
		bool auto_sell_filter_type_custom() const;
		
		bool auto_transfer_mesos() const;
		bool auto_transfer_mesos_test() const;

		unsigned int auto_transfer_mesos_amount() const;
		unsigned int auto_transfer_mesos_limit() const;
		unsigned int auto_transfer_mesos_delay() const;
		
		unsigned char auto_transfer_channel() const;
		unsigned char auto_transfer_free_market() const;
		
		unsigned int auto_transfer_shop_id() const;
		unsigned int auto_transfer_item_id() const;
		unsigned int auto_transfer_item_price() const;

	private:
		autoshop_data();

	private:
		bool enabled_;
		
		bool auto_pot_hp_restock_;
		unsigned short auto_pot_restock_hp_from_;
		unsigned short auto_pot_restock_hp_to_;
		
		bool auto_pot_mp_restock_;
		unsigned short auto_pot_restock_mp_from_;
		unsigned short auto_pot_restock_mp_to_;

		bool auto_start_bullet_restock_;

		bool auto_sell_;
		bool auto_sell_test_;

		bool auto_sell_type_equip_;
		bool auto_sell_type_etc_;
		bool auto_sell_type_use_;

		unsigned int auto_sell_delay_;

		bool auto_sell_filter_type_potential_;
		bool auto_sell_filter_type_stars_;
		bool auto_sell_filter_type_custom_;

		bool auto_transfer_mesos_;
		bool auto_transfer_mesos_test_;

		unsigned int auto_transfer_mesos_amount_;
		unsigned int auto_transfer_mesos_limit_;
		unsigned int auto_transfer_mesos_delay_;
		
		unsigned char auto_transfer_channel_;
		unsigned char auto_transfer_free_market_;
		
		unsigned int auto_transfer_shop_id_;
		unsigned int auto_transfer_item_id_;
		unsigned int auto_transfer_item_price_;
	};
}