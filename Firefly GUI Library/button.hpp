#pragma once

#include "native.hpp"

#include "window.hpp"
#include "widget.hpp"

#include <functional>

namespace firefly
{
	typedef std::function<bool()> button_event_function_t;

	enum class button_event
	{
		on_click,
	};

	class button : public widget
	{
	public:
		button(window& parent_window);
		button(widget& parent_widget);
		virtual ~button();
		
		void set_event(button_event event_type, button_event_function_t event_function = []() -> bool { return true; });
		void set_bitmap(unsigned short resource_id);
		void set_text(std::string const& caption);

		void create(std::string const& caption = "", rectangle& rect = rectangle());

	private:
		void replace_bitmap(HBITMAP bitmap = NULL);

	private:
		void set_default_message_handlers();

		LRESULT try_custom_draw_item(NMCUSTOMDRAW* custom_draw);

	private:
		HBITMAP image;
		button_event_function_t function_on_click;
	};
}