#include "rusher_entry.hpp"

namespace firefly
{
	rusher_entry::rusher_entry()
		: x_(0), y_(0), map_id_(0)
	{

	}

	rusher_entry::rusher_entry(unsigned int map_id, int x, int y)
		: map_id_(map_id), x_(x), y_(y)
	{

	}

	unsigned int rusher_entry::map_id() const
	{
		return this->map_id_;
	}
	
	int rusher_entry::x() const
	{
		return this->x_;
	}

	int rusher_entry::y() const
	{
		return this->y_;
	}

	rusher_entry& rusher_entry::operator=(const rusher_entry& e)
	{
		this->map_id_ = e.map_id();
		this->x_ = e.x();
		this->y_ = e.y();
		return *this;
	}
}