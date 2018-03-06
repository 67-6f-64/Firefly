#pragma once

#include "native.hpp"
#include "net_buffer.hpp"
#include "net_opcodes.hpp"

#include "itemfilter_list.hpp"

namespace firefly
{
	class itemfilter_data
	{
	public:
		static itemfilter_data& get_instance()
		{
			static itemfilter_data instance;
			return instance;
		}

		void handle_packet(net_buffer& packet);

		bool enabled() const;
		itemfilter_list& drop_list();
		itemfilter_list& sell_list();

		itemfilter_block_type_ids itemfilter_type() const;
		itemfilter_type_ids itemfilter_scheme() const;
		
		bool block_type_equip() const;
		bool block_type_etc() const;
		bool block_type_use() const;
		
		bool enable_mesos_filter() const;
		unsigned short mesos_filter_value() const;

	private:
		itemfilter_data();

	private:
		bool enabled_;
		itemfilter_list drop_list_;
		itemfilter_list sell_list_;

		itemfilter_block_type_ids itemfilter_type_;
		itemfilter_type_ids itemfilter_scheme_;

		bool block_type_equip_;
		bool block_type_etc_;
		bool block_type_use_;

		bool enable_mesos_filter_;
		unsigned short mesos_filter_value_;
	};
}