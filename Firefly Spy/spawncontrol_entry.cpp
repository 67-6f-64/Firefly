#include "spawncontrol_entry.hpp"

namespace firefly
{
	spawncontrol_entry::spawncontrol_entry()
		: min_level_(0), max_level_(0), x_(0), y_(0), map_id_(0)
	{

	}

	spawncontrol_entry::spawncontrol_entry(unsigned short min_level, unsigned short max_level, int x, int y, unsigned int map_id)
		: min_level_(min_level), max_level_(max_level), x_(x), y_(y), map_id_(map_id)
	{

	}
	
	short spawncontrol_entry::min_level() const
	{
		return this->min_level_;
	}

	short spawncontrol_entry::max_level() const
	{
		return this->max_level_;
	}

	int spawncontrol_entry::x() const
	{
		return this->x_;
	}

	int spawncontrol_entry::y() const
	{
		return this->y_;
	}

	unsigned int spawncontrol_entry::map_id() const
	{
		return this->map_id_;
	}

	spawncontrol_entry& spawncontrol_entry::operator=(const spawncontrol_entry& e)
	{
		this->min_level_ = e.min_level();
		this->max_level_ = e.max_level();

		this->x_ = e.x();
		this->y_ = e.y();

		this->map_id_ = e.map_id();
		return *this;
	}
}