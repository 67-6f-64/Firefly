#include "list_view_header_item.hpp"

#include <string>

namespace firefly
{
	list_view_header_item::list_view_header_item(std::string const& name, int width, bool center)
		: list_view_base_item()
	{
		this->name = name;
		this->width = width;
		this->center = center;
	}
	
	list_view_header_item::~list_view_header_item()
	{

	}
}