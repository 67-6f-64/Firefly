#pragma once

#include "button.hpp"

namespace firefly
{
	enum class system_button_type
	{
		minimize,
		close,

		scrollbar_up,
		scrollbar_down,

		add,
		remove,
	};

	class system_button : public button
	{
	public:
		system_button(window& parent_window);
		system_button(widget& parent_widget);
		~system_button();

		void create(system_button_type type, rectangle& rect = rectangle());

	private:
		void set_default_message_handlers();

		LRESULT try_custom_draw_item(NMCUSTOMDRAW* custom_draw);

	private:
		system_button_type type;

		HFONT arrow_font;
	};
}