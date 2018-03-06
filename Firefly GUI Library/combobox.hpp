#pragma once

#include "native.hpp"

#include "window.hpp"
#include "widget.hpp"

#include "menu_clickable.hpp"

#include <functional>
#include <vector>

namespace firefly
{
	typedef std::function<void(int)> combobox_event_function_t;

	enum class combobox_event
	{
		on_selection_change,
	};

	class combobox : public menu_clickable
	{
		const int menu_border_thickness = 3;

		static LRESULT CALLBACK default_list_window_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		void init();

	public:
		combobox(window& parent_window);
		combobox(widget& parent_widget);
		~combobox();
		
		void set_event(combobox_event event_type, combobox_event_function_t event_function = []() -> bool { return true; });

		int add_item(std::string const& caption, COLORREF color = 0);
		bool delete_item(int index);
		void clear_items();
		
		int get_count();

		int get_selection();
		bool set_selection(int index, bool handle_event = true);

		std::string get_text();
		bool set_text(std::string const& caption);

		std::string get_item_text(int index);
		std::vector<std::string> get_items();

		void add_token_width(int width);
		void clear_token_widths();

		void create(rectangle& rect = rectangle(), bool center = false);
		
	protected:
		int on_show_menu(POINT pt);

	private:
		void subclass_listbox_control();

		void set_default_message_handlers();
		
		LRESULT list_window_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT owner_draw_list_item(DRAWITEMSTRUCT* owner_draw);
		
		LRESULT owner_draw_menu_item(DRAWITEMSTRUCT* owner_draw);

		void split_string(std::string const& str, std::vector<std::string>& output);

	private:
		bool center;

		RECT rc_client;
		HFONT arrow_font;

		std::vector<int> token_widths;
		combobox_event_function_t function_on_selection_change;

		WNDPROC original_list_window_proc;
		std::unique_ptr<image_drawer> list_drawer;
	};
}