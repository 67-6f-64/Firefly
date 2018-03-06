#pragma once

#include "native.hpp"
#include "net_buffer.hpp"
#include "net_opcodes.hpp"

#include "spawncontrol_entry.hpp"

namespace firefly
{
	class spawncontrol_data
	{
	public:
		static spawncontrol_data& get_instance()
		{
			static spawncontrol_data instance;
			return instance;
		}

		void handle_packet(net_buffer& packet);
		bool get_entry(unsigned int map_id, short level, spawncontrol_entry& entry);

		bool enabled() const;
		
	private:
		spawncontrol_data();

	private:
		bool enabled_;
		std::vector<spawncontrol_entry> spawn_list_;
	};
}