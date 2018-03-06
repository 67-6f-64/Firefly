#include "itemfilter_list.hpp"

namespace firefly
{
	itemfilter_list::itemfilter_list()
		: list_()
	{
		this->clear();
	}
	
	void itemfilter_list::add(unsigned int id)
	{
		this->list_.insert(id);
	}

	void itemfilter_list::clear()
	{
		this->list_.clear();
	}

	bool itemfilter_list::exists(unsigned int id)
	{
		if (this->list_.empty())
			return false;

		return (this->list_.find(id) != this->list_.end());
	}
}