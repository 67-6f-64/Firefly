#include "itemfilter_data.hpp"

namespace firefly
{
	void itemfilter_data::handle_packet(net_buffer& packet)
	{
		this->enabled_ = (packet.read1() != FALSE);

		if (this->enabled_)
		{
			this->drop_list_.clear();
			this->sell_list_.clear();

			this->itemfilter_type_ = static_cast<itemfilter_block_type_ids>(packet.read1());
			this->itemfilter_scheme_ = static_cast<itemfilter_type_ids>(packet.read1());
			
			this->enable_mesos_filter_ = (packet.read1() != FALSE);

			if (this->enable_mesos_filter_)
				this->mesos_filter_value_ = packet.read4();

			switch (this->itemfilter_scheme_)
			{
			case itemfilter_type_ids::filter_by_type:
				this->block_type_equip_ = (packet.read1() != FALSE);
				this->block_type_etc_ = (packet.read1() != FALSE);
				this->block_type_use_ = (packet.read1() != FALSE);
				break;

			case itemfilter_type_ids::custom_filter:
				this->block_type_equip_ = (packet.read1() != FALSE);
				this->block_type_etc_ = (packet.read1() != FALSE);
				this->block_type_use_ = (packet.read1() != FALSE);
				
				for (unsigned int i = 0, count = packet.read4(); i < count; i++)
					this->drop_list_.add(packet.read4());

				break;
				
			default:
				break;
			}
			
			for (unsigned int i = 0, count = packet.read4(); i < count; i++)
				this->sell_list_.add(packet.read4());
		}
	}

	bool itemfilter_data::enabled() const
	{
		return this->enabled_;
	}
		
	itemfilter_list& itemfilter_data::drop_list()
	{
		return this->drop_list_;
	}
		
	itemfilter_list& itemfilter_data::sell_list()
	{
		return this->sell_list_;
	}
	
	itemfilter_block_type_ids itemfilter_data::itemfilter_type() const
	{
		return this->itemfilter_type_;
	}

	itemfilter_type_ids itemfilter_data::itemfilter_scheme() const
	{
		return this->itemfilter_scheme_;
	}
		
	bool itemfilter_data::block_type_equip() const
	{
		return this->block_type_equip_;
	}

	bool itemfilter_data::block_type_etc() const
	{
		return this->block_type_etc_;
	}

	bool itemfilter_data::block_type_use() const
	{
		return this->block_type_use_;
	}
		
	bool itemfilter_data::enable_mesos_filter() const
	{
		return this->enable_mesos_filter_;
	}

	unsigned short itemfilter_data::mesos_filter_value() const
	{
		return this->mesos_filter_value_;
	}

	itemfilter_data::itemfilter_data()
		: drop_list_(), sell_list_()
	{
		this->enabled_ = false;
		this->drop_list_.clear();
		this->sell_list_.clear();

		this->itemfilter_type_ = itemfilter_block_type_ids::blacklist;
		this->itemfilter_scheme_ = itemfilter_type_ids::filter_everything;
		
		this->block_type_equip_ = false;
		this->block_type_etc_ = false;
		this->block_type_use_ = false;
		
		this->enable_mesos_filter_ = false;
		this->mesos_filter_value_ = 0;
	}
}