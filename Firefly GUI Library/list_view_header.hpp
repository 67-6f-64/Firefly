#pragma once	

#include "list_view_base.hpp"
#include "list_view_header_item.hpp"

namespace firefly
{
	#define LISTVIEW_COLUMN_HEIGHT 30

	class list_view_header : public list_view_base<list_view_header_item>
	{
	public:
		list_view_header(window& parent_window);
		list_view_header(widget& parent_widget);
		~list_view_header();

		int add_item(std::string const& caption, int width, bool center = false);

		void create(rectangle& rect = rectangle());
				
		RECT calculate_item_rect(int index);

	private:
		void draw_item(list_view_header_item& item, int index, int flags, RECT* rc);
	};
}