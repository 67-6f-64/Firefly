#pragma once

#include "native.hpp"

#include "window.hpp"
#include "widget.hpp"

#include "image_drawer.hpp"
#include "message_controller.hpp"

namespace firefly
{
	class tooltip : public message_controller
	{
	private:
		void init(base_window& base);

	public:
		tooltip(window& parent_window);
		tooltip(widget& parent_widget);
		~tooltip();

		bool create();
		bool bind_control(HWND handle, std::string const& caption);

		void reset_font();

	private:
		std::string get_text();

	private:
		void set_default_tooltip_handlers();
	
		LRESULT on_show_tooltip(NMHDR* custom_notify);
		LRESULT try_custom_draw(NMTTCUSTOMDRAW* custom_draw);

	private:
		RECT rc_client;
		HANDLE bound_handle;
	};
}