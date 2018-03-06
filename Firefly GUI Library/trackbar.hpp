#pragma once

#include "native.hpp"

#include "window.hpp"
#include "widget.hpp"

#include <functional>

namespace firefly
{
	typedef std::function<void(int)> trackbar_event_function_t;

	enum class trackbar_event
	{
		on_drag,
	};

	class trackbar : public widget
	{
	public:
		trackbar(window& parent_window);
		trackbar(widget& parent_widget);
		~trackbar();

		void set_event(trackbar_event event_type, trackbar_event_function_t event_function = []() -> bool { return true; });
		
		void set_range(short minimum, short maximum);

		int get_position();
		void set_position(short position, bool cause_event = false);

		void create(rectangle& rect = rectangle(), bool horizontal = true);

	private:
		void set_default_message_handlers();

		LRESULT try_custom_draw_item(NMCUSTOMDRAW* custom_draw);

		LRESULT try_custom_draw_channel(NMCUSTOMDRAW* custom_draw);
		LRESULT try_custom_draw_thumb(NMCUSTOMDRAW* custom_draw);
		LRESULT try_custom_draw_ticks(NMCUSTOMDRAW* custom_draw);

	private:
		short minimum;
		short maximum;

		RECT rc_channel;

		trackbar_event_function_t function_on_drag;
	};
}