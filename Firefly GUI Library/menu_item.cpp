#include "menu_item.hpp"

namespace firefly
{
	menu_item::menu_item()
	{

	}

	menu_item::menu_item(std::string const& caption, menu_event_function_t function)
		: caption_(caption), function_(function)
	{

	}

	menu_item::~menu_item()
	{

	}
	
	std::string const& menu_item::caption()
	{
		return this->caption_;
	}

	menu_event_function_t const& menu_item::function()
	{
		return this->function_;
	}
}