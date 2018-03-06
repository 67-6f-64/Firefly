#include "spawncontrol_data.hpp"

#include <algorithm>

namespace firefly
{
	void spawncontrol_data::handle_packet(net_buffer& packet)
	{
		this->enabled_ = (packet.read1() != FALSE);

		if (this->enabled_)
		{
			this->spawn_list_.clear();

			for (unsigned int i = 0, spawn_points = packet.read4(); i < spawn_points; i++)
			{
				short min_level = packet.read2();
				short max_level = packet.read2();

				int x = packet.read4();
				int y = packet.read4();

				unsigned int map_id = packet.read4();

				this->spawn_list_.push_back(spawncontrol_entry(min_level, max_level, x, y, map_id));
			}
		}
	}
	
	bool spawncontrol_data::get_entry(unsigned int map_id, short level, spawncontrol_entry& entry)
	{
		std::vector<spawncontrol_entry>::iterator iter = std::find_if(this->spawn_list_.begin(), this->spawn_list_.end(), [&](const spawncontrol_entry& e) -> bool
		{
			return (e.map_id() == map_id && (e.min_level() == -1 ? true : (level >= e.min_level())) && (e.max_level() == -1 ? true : (level <= e.max_level())));
		});

		if (iter == this->spawn_list_.end())
			return false;

		entry = *iter;
		return true;
	}

	bool spawncontrol_data::enabled() const
	{
		return this->enabled_;
	}
	
	spawncontrol_data::spawncontrol_data()
	{
		this->enabled_ = false;
	}
}