#pragma once

#include "native.hpp"
#include "basic_types.hpp"

#include "image_drawer.hpp"
#include "message_controller.hpp"

#include <memory>
#include <string>

namespace firefly
{
	class system_button;

	class window : public message_controller
	{
	public:
		window(HINSTANCE hinstance, std::string const& class_name, std::string const& window_name, char* icon_resource, rectangle& rect, bool show_in_taskbar = true, bool can_minimize = true, HWND hwnd_parent = NULL);
		~window();
		
		void start(bool animate);

	private:
		bool register_class(std::string const& class_name, char* icon_resource);
		bool create_window(std::string const& class_name, std::string const& window_name, rectangle& rect, bool show_in_taskbar, HWND hwnd_parent);

		void set_default_message_handlers();

		void create_controls(rectangle& rect, bool can_minimize);

	private:
		HBRUSH background_brush;

		std::unique_ptr<system_button> minimize_button;
		std::unique_ptr<system_button> close_button;
	};
}