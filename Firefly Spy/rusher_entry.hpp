#pragma once

#include "native.hpp"

namespace firefly
{
	class rusher_entry
	{
	public:
		rusher_entry();
		rusher_entry(unsigned int map_id, int x, int y);
		
		unsigned int map_id() const;

		int x() const;
		int y() const;

		rusher_entry& operator=(const rusher_entry& e);

	private:
		unsigned int map_id_;
		int x_;
		int y_;
	};
}