#pragma once	

#include <functional>
#include <string>

namespace firefly
{
	typedef std::function<bool(int)> menu_event_function_t;

	class menu_item
	{
	public:
		menu_item();
		menu_item(std::string const& caption, menu_event_function_t function);
		~menu_item();

		std::string const& caption();
		menu_event_function_t const& function();

	public:
		std::string caption_;
		menu_event_function_t function_;
	};
}