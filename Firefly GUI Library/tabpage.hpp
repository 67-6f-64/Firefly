#pragma once

#include "native.hpp"
#include "widget.hpp"

#include "panel.hpp"
#include "scrollbar.hpp"

#include <functional>
#include <vector>

namespace firefly
{
	#define TABPAGE_INDENT_LEFT	STANDARD_INDENT
	#define TABPAGE_INDENT_RIGHT (STANDARD_INDENT + (SCROLLBAR_WIDTH + 1))

	class tabpage : public panel
	{
		static bool is_child_class(HWND hWnd, std::string const& child_class_name);

	public:
		tabpage(window& parent_window, COLORREF background_color = 0);
		~tabpage();
		
		bool on_mouse_wheel(int delta);

		void store_control(std::shared_ptr<widget> control);
		void clear_controls();

		int create_seperator(std::string const& caption, int y);

		void reload_edit_controls();

		void reset_scrollbar(bool exclude_invisible = false);
		void reset_scrollbar_position();

		void create(rectangle& rect = rectangle(), bool show = false);
		
		void scroll_to_control(HWND hWnd, RECT* rc_screen_control);

	protected:
		virtual void create_controls(rectangle& rect);
		virtual void create_handlers();
		
	private:
		int last_child_y(int height, bool exclude_invisible);

		void create_default_handlers();

		LRESULT paint_client_area(HDC hdc, RECT* rc_paint, bool erase);
		
	protected:
		std::shared_ptr<scrollbar> scroll_bar;

	private:
		std::vector<std::shared_ptr<widget>> controls;
	};
}