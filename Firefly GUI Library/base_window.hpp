#pragma once

#include "native.hpp"

#include <string>

namespace firefly
{
	class base_window
	{
		friend class message_controller;

	public:
		HWND handle();
		void set_handle(HWND hwnd);

		HWND parent_handle();

		HINSTANCE instance();

		COLORREF foreground();
		void set_foreground(COLORREF color);

		COLORREF background();
		void set_background(COLORREF color);

		HFONT font();		
		void set_font(HFONT font);

		void show(bool show);
		void enable(bool state);

		void move(int x, int y);
		void resize(int width, int height, bool redraw = true);

	protected:
		base_window(HINSTANCE hinstance, HWND hwnd_parent);
		virtual ~base_window();
		
		HFONT create_font(HFONT font);
		HFONT create_font(std::string const& font_name, int height = 14, int width = 0, int weight = FW_DONTCARE);

	private:
		HWND hwnd;
		HWND hwnd_parent;

		HINSTANCE hinstance;
		
		COLORREF foreground_color;
		COLORREF background_color;

		HFONT font_object;
	};
}