#pragma once	

#include "native.hpp"

#include <functional>
#include <vector>

namespace firefly
{
	class list_view_base_item
	{
	public:
		list_view_base_item();
		virtual ~list_view_base_item();

		list_view_base_item& operator=(const list_view_base_item& item);
	};
}