#pragma once

#include "native.hpp"

#include "window.hpp"
#include "widget.hpp"

namespace firefly
{
	class label : public widget
	{
	public:
		label(window& parent_window);
		label(widget& parent_widget);
		~label();

		std::string get_text();
		void set_text(std::string const& caption);

		void create(std::string const& caption = "", rectangle& rect = rectangle(), bool center = false, bool vertical_center = true, bool path_ellipsis = false);

	private:
		void set_default_message_handlers();
	};
}