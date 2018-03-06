#pragma once	

#include "native.hpp"

#include "window.hpp"
#include "widget.hpp"

#include "menu_item.hpp"

namespace firefly
{
	class menu_clickable : public widget
	{
		friend class combobox;

		const int menu_border_thickness = 3;

		static LRESULT CALLBACK default_menu_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		
	public:
		menu_clickable(window& parent_window);
		menu_clickable(widget& parent_widget);
		~menu_clickable();

		int add_menu_item(std::string const& name, menu_event_function_t function);
		bool remove_menu_item(int index);

	protected:
		void show_menu(HWND hWnd, POINT pt);

		void on_init_menu();

		void on_measure_item(HWND hWnd, MEASUREITEMSTRUCT* measure_item);
		void on_draw_item(HWND hWnd, DRAWITEMSTRUCT* draw_item);
		
		virtual int on_show_menu(POINT pt) = 0;
		
	private:
		void set_default_message_handlers();
		
		void on_mouse_right_down(HWND hWnd, unsigned int keys, POINT pt);
		void on_mouse_right_up(HWND hWnd, unsigned int keys, POINT pt);
		
		LRESULT menu_window_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		void on_menu_size(HWND hWnd, short width, short height);
		void draw_menu_border(HWND hWnd, HDC hdc);

	private:
		HMENU menu;
		std::size_t menu_index;
		std::map<int, menu_item> menu_items;

		WNDPROC original_menu_window_proc;
		std::unique_ptr<image_drawer> menu_drawer;
	};
}