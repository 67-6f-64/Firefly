#pragma once

#include "native.hpp"

namespace firefly
{
	class spawncontrol_entry
	{
	public:
		spawncontrol_entry();
		spawncontrol_entry(unsigned short min_level, unsigned short max_level, int x, int y, unsigned int map_id);

		short min_level() const;
		short max_level() const;

		int x() const;
		int y() const;

		unsigned int map_id() const;

		spawncontrol_entry& operator=(const spawncontrol_entry& e);

	private:
		short min_level_;
		short max_level_;

		int x_;
		int y_;

		unsigned int map_id_;
	};
}