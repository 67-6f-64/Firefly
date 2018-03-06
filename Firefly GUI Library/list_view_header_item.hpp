#pragma once	

#include "list_view_base_item.hpp"

namespace firefly
{
	class list_view_header_item : public list_view_base_item
	{
	public:
		list_view_header_item(std::string const& name, int width, bool center);
		~list_view_header_item();
		
	public:
		std::string name;
		int width;
		bool center;
	};
}