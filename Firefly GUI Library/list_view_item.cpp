#include "list_view_item.hpp"

#include <string>

namespace firefly
{
	list_view_item::list_view_item()
		: list_view_base_item()
	{
			
	}

	list_view_item::~list_view_item()
	{

	}
	
	bool list_view_item::bind_helper(int index, std::string const& value)
	{
		this->subitems.push_back(value);
		return true;
	}

	bool list_view_item::bind_helper(int index, char const* value)
	{
		this->subitems.push_back(std::string(value));
		return true;
	}

	bool list_view_item::bind_helper(int index, int value)
	{
		this->subitems.push_back(std::to_string(value));
		return true;
	}
}