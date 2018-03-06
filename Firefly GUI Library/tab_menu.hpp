#pragma once	

#include "tab_base.hpp"

#include "button.hpp"
#include "togglebox.hpp"
#include "tabpage.hpp"

namespace firefly
{
	class tab_menu_item : public tab_base_item
	{
	public:
		tab_menu_item(std::string const name, bool activated, HBITMAP icon, bool has_button, bool is_add_button, button_event_function_t button_event_function);
		tab_menu_item(std::string const name, bool activated, HBITMAP icon, bool has_toggle_box, togglebox_event_function_t toggle_box_event_function);
		~tab_menu_item();
		
	public:
		HBITMAP icon;

		bool has_button;
		bool is_add_button;
		std::unique_ptr<system_button> tab_button;
		button_event_function_t tab_button_event_function;

		bool has_toggle_box;
		std::unique_ptr<togglebox> toggle_box;
		togglebox_event_function_t toggle_box_event_function;
	};

	class tab_menu : public tab_base<tab_menu_item>
	{
		void init();

	public:
		static const int togglebox_size = 16;
		static const int button_size = 24;

		tab_menu(window& parent_window);
		tab_menu(widget& parent_widget);
		~tab_menu();
		
		int add_tab(std::string const& caption, bool activated, int icon_resource_id = 0, bool has_button = false, bool is_add_button = false,
			button_event_function_t button_event_function = []() -> bool { return true; }, std::function<void()> post_update_func = []() -> void {  });
		
		int add_tab(std::string const& caption, bool activated, int icon_resource_id, bool has_toggle_box,
			togglebox_event_function_t toggle_box_event_function);

		void remove_tab(int index);

		void create(rectangle& rect = rectangle());

		bool on_mouse_wheel(int delta);
		
		int resize_control(int item_index = 0, RECT* rc_item = NULL, bool greedy = false);
		
		bool enumerate_relatives(std::function<bool(std::shared_ptr<widget>)> enum_func);
		
	private:
		void set_default_message_handlers();

		bool enumerate_tabs(tab_base_enum_func tab_func);
		bool draw_tab_item(std::shared_ptr<tab_menu_item> item, int index, int flags, RECT* rc);
		
		RECT calculate_background_front_rect();
		
		void update_button_function(int index, std::shared_ptr<tab_menu_item> item);

	private:
		HFONT font_section;
		HFONT font_tab_item;

		unsigned int width;
	};
}