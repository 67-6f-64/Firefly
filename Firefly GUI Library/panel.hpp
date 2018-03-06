#pragma once

#include "native.hpp"
#include "widget.hpp"

namespace firefly
{
	class panel : public widget
	{
	public:
		panel(window& parent_window);
		panel(widget& parent_widget);
		virtual ~panel();

		void create(rectangle& rect = rectangle(), bool has_background = false, bool show = true);

	protected:
		void on_mouse_left_down(HWND hWnd, unsigned int keys, POINT pt);

	private:
		void set_default_message_handlers();
		
		LRESULT draw_item(HWND hwnd, DRAWITEMSTRUCT* panel_draw);

	private:
		bool has_background;
	};
}