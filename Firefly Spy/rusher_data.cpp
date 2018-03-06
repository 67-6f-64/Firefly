#include "rusher_data.hpp"
#include "autobotting_hooks.hpp"

#include <algorithm>

namespace firefly
{
	void rusher_data::handle_packet(net_buffer& packet)
	{
		this->rusher_list_.clear();

		for (unsigned int i = 0, map_count = packet.read4(); i < map_count; i++)
		{
			unsigned int map_id = packet.read4();
			short x = packet.read2();
			short y = packet.read2();
				
			this->rusher_list_.push_back(rusher_entry(map_id, x, y));
		}

		this->enabled_ = true;
		this->stage_ = rusher_stage::changing_channel;
	}
	
	bool rusher_data::get_entry(unsigned int map_id, rusher_entry& entry)
	{
		std::vector<rusher_entry>::iterator iter = std::find_if(this->rusher_list_.begin(), this->rusher_list_.end(), [&](const rusher_entry& e) -> bool
		{
			return (e.map_id() == map_id);
		});

		if (iter == this->rusher_list_.end())
			return false;

		entry = *iter;
		return true;
	}

	bool rusher_data::enabled() const
	{
		return this->enabled_;
	}
	
	void rusher_data::disable()
	{
		this->enabled_ = false;
	}
	
	rusher_stage rusher_data::stage() const
	{
		return this->stage_;
	}

	void rusher_data::set_stage(rusher_stage stage)
	{
		this->stage_ = stage;
	}

	rusher_data::rusher_data()
	{
		this->disable();
	}
}