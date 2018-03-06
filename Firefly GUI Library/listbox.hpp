#pragma once

#include "native.hpp"

#include "window.hpp"
#include "widget.hpp"

namespace firefly
{
	typedef std::function<void(int)> listbox_event_function_t;

	enum class listbox_event
	{
		on_left_click,
		on_left_double_click
	};

	class listbox : public widget
	{
	public:
		listbox(window& parent_window);
		listbox(widget& parent_widget);
		~listbox();

		void set_event(listbox_event event_type, listbox_event_function_t event_function = [](int) -> void {});

		int add_string(const char* format, ...);
		int add_string(COLORREF color, const char* format, ...);
		void clear();

		void create(rectangle& rect = rectangle(), bool use_time_stamp = false, bool no_select = true);

	private:
		int add_string(COLORREF color, const char* format, va_list va);

	private:
		void set_default_message_handlers();

		LRESULT draw_item(DRAWITEMSTRUCT* owner_draw);
			
	private:
		bool use_time_stamp;
		listbox_event_function_t function_on_left_click;
		listbox_event_function_t function_on_left_double_click;
	};
}