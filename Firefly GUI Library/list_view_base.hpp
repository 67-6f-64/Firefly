#pragma once	

#include "native.hpp"

#include "window.hpp"
#include "widget.hpp"

#include "list_view_base_item.hpp"

#include <strsafe.h>

#include <functional>
#include <vector>

namespace firefly
{
	#define WC_CUSTOM_LIST_VIEW_BASE "CustomListviewBase"
	
	#define LVBASE_HOVERED 0x01
	#define LVBASE_SELECTED 0x02

	#define LVIEW_INDENT 10

	namespace custom_control
	{
		bool register_list_view_base_control();
		bool unregister_list_view_base_control();
	}

	#define LISTVIEW_COLUMN_HEIGHT 30
	
	enum list_view_event_type
	{
		on_selection_change_,
		on_left_double_click
	};
	
	typedef std::function<bool(int)> list_view_event_function_t;

	template <typename T = list_view_base_item>
	class list_view_base : public widget
	{
		void init()
		{
			SetRectEmpty(&this->rc_client);
			this->items.clear();

			this->hover_index = -1;
			this->selection_index = -1;

			this->on_left_double_click_event_function = nullptr;
		}
		
	public:
		void remove_item(int index)
		{
			this->pre_resize_list_view();
			this->items.erase(this->items.begin() + index);

			if (IsWindowVisible(this->handle()))
				this->redraw(&this->rc_client);
			
			this->post_resize_list_view();
		}

		void clear_items()
		{
			this->pre_resize_list_view();
			this->items.clear();
			
			if (IsWindowVisible(this->handle()))
				this->redraw(&this->rc_client);

			this->post_resize_list_view();
		}

		void swap_items(int index1, int index2, std::function<void(int, int)> swap_func = [](int index1, int index2) { })
		{
			std::iter_swap(this->items.begin() + index1, this->items.begin() + index2);
			
			RECT rc_item_1 = this->calculate_item_rect(index1);
			RECT rc_item_2 = this->calculate_item_rect(index2);

			swap_func(index1, index2);
			
			RECT rc_union;
			UnionRect(&rc_union, &rc_item_1, &rc_item_2);

			this->redraw(&rc_union);
		}

		void set_event(list_view_event_type event_type, list_view_event_function_t list_view_event_function)
		{
			if (event_type == list_view_event_type::on_left_double_click)
				this->on_left_double_click_event_function = list_view_event_function;
		}
		
		void redraw(RECT* rc)
		{
			InvalidateRect(this->handle(), rc, FALSE);
		}
		
		int get_item_count()
		{
			return this->items.size();
		}

		T& get_item_data(int index)
		{
			return this->items.at(index);
		}

		int get_selection()
		{
			return this->selection_index;
		}
		
		void set_selection(int index, RECT* rc = NULL)
		{
			if (index != this->selection_index)
			{
				this->reset_selection_state();
				this->selection_index = index;
				
				if (!rc)
					rc = &this->calculate_item_rect(index);
				
				this->redraw(rc);

				ClientToScreen(this->handle(), reinterpret_cast<POINT*>(&rc->left));
				ClientToScreen(this->handle(), reinterpret_cast<POINT*>(&rc->right));
					
				this->scroll_to_item(rc);
			}
		}
		
		virtual RECT calculate_item_rect(int index) = 0;
		
	protected:
		list_view_base(window& parent_window)
			: widget(parent_window)
		{
			this->init();
			this->set_default_message_handlers();
		}

		list_view_base(widget& parent_widget)
			: widget(parent_widget)
		{
			this->init();
			this->set_default_message_handlers();
		}

		~list_view_base()
		{

		}
		
		void create(rectangle& rect = rectangle())
		{
			if (!this->build(WC_CUSTOM_LIST_VIEW_BASE, "", rect, WS_VISIBLE | WS_CHILD))
				throw std::exception("Failed to create 'list_view_base' widget");
				
			this->set_font(this->create_font("Arial", 16, 7, FW_REGULAR));

			if (IsRectEmpty(&this->rc_client))
				GetClientRect(this->handle(), &this->rc_client);
		}
		
		int find_item(POINT pt, RECT* rc)
		{
			for (std::size_t i = 0; i < this->items.size(); i++)
			{
				RECT rc_item = this->calculate_item_rect(i);
			
				if (PtInRect(&rc_item, pt))
				{
					CopyRect(rc, &rc_item);
					return static_cast<int>(i);
				}
			}

			return -1;
		}
		
		void reset_selection_state()
		{
			if (this->selection_index != -1)
			{
				RECT rc_selection = this->calculate_item_rect(this->selection_index);

				this->selection_index = -1;
				this->redraw(&rc_selection);
			}
		}
		
		virtual void scroll_to_item(RECT* rc_screen_item)
		{
			/* placeholder for virtual function */
		}

		virtual void pre_resize_list_view()
		{
			/* placeholder for virtual function */
		}

		virtual void post_resize_list_view(bool update_scrollbar = true)
		{
			/* placeholder for virtual function */
		}
		
	private:
		void on_mouse_leave()
		{
			this->reset_hover_state();
		}

		void on_mouse_move(unsigned int keys, POINT pt)
		{
			RECT rc_item;
			int index = this->find_item(pt, &rc_item);

			if (index >= 0)
				this->set_new_hover_state(index, &rc_item);
			else
				this->reset_hover_state();
		}
		
		void on_mouse_left_down(HWND hWnd, unsigned int keys, POINT pt)
		{
			SetFocus(hWnd);
			SetCapture(hWnd);
		}

		void on_mouse_left_up(HWND hWnd, unsigned int keys, POINT pt)
		{
			RECT rc_item;
			int index = this->find_item(pt, &rc_item);

			if (index >= 0)
				this->set_selection(index, &rc_item);
			else
				this->reset_selection_state();

			ReleaseCapture();
		}

		void on_mouse_left_double_click(HWND hWnd, unsigned int keys, POINT pt)
		{
			RECT rc_item;
			int index = this->find_item(pt, &rc_item);
			
			if (index >= 0)
				this->set_selection(index, &rc_item);
			else
				this->reset_selection_state();

			if (this->on_left_double_click_event_function)
				this->on_left_double_click_event_function(index);
		}
		
		void set_new_hover_state(int new_index, RECT* rc)
		{
			if (new_index != this->hover_index)
			{
				this->reset_hover_state();
				
				this->hover_index = new_index;
				this->redraw(rc);
			}
		}
		
		void reset_hover_state()
		{
			if (this->hover_index != -1)
			{
				RECT rc_hover = this->calculate_item_rect(this->hover_index);

				this->hover_index = -1;
				this->redraw(&rc_hover);
			}
		}

		void set_default_message_handlers()
		{
			this->add_message_handler(WM_ERASEBKGND, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
			{
				RECT rc_header = this->calculate_item_rect(-1);
				this->draw_rectangle(&rc_header, this->background());
				this->commit_drawing(reinterpret_cast<HDC>(wParam), &rc_header);
				return TRUE;
			});

			this->add_message_handler(WM_PAINT, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hWnd, &ps);

				this->paint_client_area(hdc, &ps.rcPaint, ps.fErase != FALSE);

				EndPaint(hWnd, &ps);
				return 0;
			});
			
			this->add_message_handler(WM_PRINTCLIENT, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
			{
				RECT rc;
				GetClientRect(this->handle(), &rc);

				this->paint_client_area(reinterpret_cast<HDC>(wParam), &rc, true);
				return 0;
			});
			
			this->add_message_handler(WM_KILLFOCUS, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
			{
				this->reset_hover_state();
				this->reset_selection_state();		
				return this->original_proc(hWnd, WM_KILLFOCUS, wParam, lParam);
			});

			this->add_message_handler(WM_KEYDOWN, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
			{
				switch (wParam)
				{
				case VK_UP:
					if (this->selection_index > 0)
						this->set_selection(this->selection_index - 1);

					break;

				case VK_DOWN:
					if (this->selection_index < static_cast<int>(this->items.size() - 1))
						this->set_selection(this->selection_index + 1);

					break;

				default:
					break;
				}
			
				return this->original_proc(hWnd, WM_KEYDOWN, wParam, lParam);
			});
		}
		
		LRESULT paint_client_area(HDC hdc, RECT* rc_paint, bool erase)
		{
			RECT rc_intersection;
			RECT rc_item_panel = this->calculate_item_rect(-1);

			if (IntersectRect(&rc_intersection, &rc_item_panel, rc_paint))
			{
				for (std::size_t i = 0; i < this->items.size(); i++)
				{
					rc_item_panel = this->calculate_item_rect(i);

					if (IntersectRect(&rc_intersection, &rc_item_panel, rc_paint))
					{
						bool hovered = (static_cast<int>(i) == this->hover_index);
						bool selected = (static_cast<int>(i) == this->selection_index);

						int flags = (hovered ? LVBASE_HOVERED : 0) | (selected ? LVBASE_SELECTED : 0);
						this->draw_item(this->items.at(i), static_cast<int>(i), flags, &rc_item_panel);
					}
				}
			}
		
			this->commit_drawing(hdc, rc_paint);
			return 0;
		}
		
		virtual void draw_item(T& item, int index, int flags, RECT* rc) = 0;

	protected:
		RECT rc_client;
		std::vector<T> items;

	private:
		int hover_index;
		int selection_index;
		
		list_view_event_function_t on_left_double_click_event_function;
	};
}