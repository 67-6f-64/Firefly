#pragma once	

#include "list_view_base.hpp"
#include "list_view_header.hpp"

#include "list_view_item.hpp"
#include "menu_item.hpp"

#include "tabpage.hpp"

namespace firefly
{
	#define LISTVIEW_ITEM_HEIGHT 40

	class list_view : public list_view_base<list_view_item>
	{
		const int menu_border_thickness = 3;

		static LRESULT CALLBACK default_menu_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		
	public:
		list_view(window& parent_window);
		list_view(widget& parent_widget);
		~list_view();

		int add_column(std::string const& caption, int width, bool center = false);
		void remove_column(int index);
		
		template <typename... Args>
		int add_item(Args&&... args);
		
		template <typename... Args>
		int add_item_manual(Args&&... args);
		
		int add_menu_item(std::string const& menu_name, menu_event_function_t menu_function);
		bool remove_menu_item(int index);

		void create(rectangle& rect = rectangle(), std::shared_ptr<tabpage> parent_tabpage = nullptr);

		void resize_list_view(bool update_scrollbar = true);

		int get_height();
		
		std::string get_item_data(int index, int subitem_index);
		void set_item_data(int index, int subitem_index, std::string const& data);

		int get_item_integer(int index, int subitem_index);
		
		RECT calculate_item_rect(int index);
		
	protected:
		void scroll_to_item(RECT* rc_screen_item);

		void pre_resize_list_view();
		void post_resize_list_view(bool update_scrollbar = true);

	private:
		void set_default_message_handlers();

		void on_mouse_right_down(HWND hWnd, unsigned int keys, POINT pt);
		void on_mouse_right_up(HWND hWnd, unsigned int keys, POINT pt);
		
		LRESULT menu_window_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT owner_draw_menu_item(DRAWITEMSTRUCT* owner_draw);

		void draw_item(list_view_item& item, int index, int flags, RECT* rc);

	private:
		std::unique_ptr<list_view_header> header;
		std::shared_ptr<tabpage> parent_tabpage;
		
		HMENU menu;
		std::size_t menu_index;
		std::map<int, menu_item> menu_items;

		WNDPROC original_menu_window_proc;
		std::unique_ptr<image_drawer> menu_drawer;
	};
	
	template <typename... Args>
	int list_view::add_item(Args&&... args)
	{
		this->items.push_back(list_view_item());
		this->items.back().bind_data(std::forward<Args>(args)...);
		
		this->pre_resize_list_view();

		if (IsWindowVisible(this->handle()))
			this->redraw(&this->rc_client);
		
		this->post_resize_list_view();
		return (this->items.size() - 1);
	}
	
	template <typename... Args>
	int list_view::add_item_manual(Args&&... args)
	{
		this->items.push_back(list_view_item());
		this->items.back().bind_data(std::forward<Args>(args)...);
		
		if (IsWindowVisible(this->handle()))
			this->redraw(&this->rc_client);

		return (this->items.size() - 1);
	}
}