#pragma once

#include "native.hpp"
#include "widget.hpp"

#include <functional>

namespace firefly
{
	typedef std::function<void(std::string const&)> textbox_event_function_t;

	enum class textbox_event
	{
		on_return,
		on_text_change,
	};

	class textbox : public widget
	{
		void init();

	public:
		textbox(window& parent_window);
		textbox(widget& parent_widget);
		~textbox();
		
		void set_event(textbox_event event_type, textbox_event_function_t event_function = []() -> bool { return true; });
		
		bool set_cue_banner(std::string const& caption);
		void set_maximum_length(std::size_t max_length);
		bool set_readonly(bool read_only);
		
		bool set_text(std::string const& text);
		std::string get_text();

		int get_integer();
		int get_text_length();

		void create(rectangle& rect = rectangle(), bool center = false, bool numeric = false, bool password = false);

	private:
		void on_mouse_enter(unsigned int keys, POINT pt);

	private:
		void set_default_message_handlers();
		
	private:
		bool is_readonly;
		std::string cue_banner;

		RECT rc_client;
		RECT rc_nonclient;
		RECT rc_original;

		COLORREF background_readonly;
		COLORREF background_focused;
		COLORREF background_normal;

		textbox_event_function_t function_on_return;
		textbox_event_function_t function_on_text_change;
	};
}