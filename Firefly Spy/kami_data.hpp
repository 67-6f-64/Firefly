#pragma once

#include "native.hpp"
#include "net_buffer.hpp"
#include "net_opcodes.hpp"

namespace firefly
{
	class kami_data
	{
	public:
		static kami_data& get_instance()
		{
			static kami_data instance;
			return instance;
		}

		void handle_packet(net_buffer& packet);

		bool enabled() const;

		short horizontal_range() const;
		short vertical_range() const;
		
		kami_movement_type_ids movement_type() const;
		kami_target_type_ids target_type() const;

		bool enable_item_loot() const;
		unsigned short item_loot_value() const;
		kami_target_type_ids item_loot_target_type() const;

		bool is_item_looting() const;
		void set_item_looting(bool enable);

	private:
		kami_data();

	private:
		bool enabled_;
		
		unsigned short horizontal_range_;
		short vertical_range_;
		
		kami_movement_type_ids movement_type_;
		kami_target_type_ids target_type_;

		bool enable_item_loot_;
		unsigned short item_loot_value_;
		kami_target_type_ids item_loot_target_type_;

		bool item_looting_;
	};
}