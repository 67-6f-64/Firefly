#pragma once

#include "native.hpp"
#include "widget.hpp"

namespace firefly
{
	class seperator : public widget
	{
	public:
		seperator(window& parent_window);
		seperator(widget& parent_widget);
		~seperator();

		void create(std::string const& caption, rectangle& rect = rectangle());

	private:
		void set_default_message_handlers();
		
		LRESULT draw_item(HWND hWnd, DRAWITEMSTRUCT* owner_draw);

	private:
		bool has_background;
	};
}