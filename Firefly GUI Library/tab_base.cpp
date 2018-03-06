#include "tab_base.hpp"

namespace firefly
{
	tab_base_item::tab_base_item(std::string const& name, bool activated)
		: name(name), activated(activated)
	{
		this->relatives.clear();
	}

	tab_base_item::~tab_base_item()
	{
		this->relatives.clear();
	}
		
	tab_base_item& tab_base_item::operator=(const tab_base_item& item)
	{
		return *this;
	}
}