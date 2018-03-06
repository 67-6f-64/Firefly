#pragma once

#include "native.hpp"
#include "widget.hpp"

namespace firefly
{
	typedef std::function<bool(bool)> togglebox_event_function_t;

	enum class togglebox_type
	{
		checkbox,
		radiobutton,
		threestate,
	};

	enum class togglebox_event
	{
		on_state_change,
	};

	class togglebox : public widget
	{
	public:
		togglebox(window& parent_window);
		togglebox(widget& parent_widget);
		~togglebox();

		void toggle();

		void set_check(bool checked);
		bool get_check();

		std::string get_text();

		void set_event(togglebox_event event_type, togglebox_event_function_t event_function = [](bool) -> bool { return true; });

		void create(std::string const& caption = "", rectangle& rect = rectangle(), togglebox_type type = togglebox_type::checkbox);

	private:
		void set_default_message_handlers();

		LRESULT try_custom_draw_item(NMCUSTOMDRAW* custom_draw);

	private:
		togglebox_type type;
		togglebox_event_function_t function_on_state_change;
	};
}