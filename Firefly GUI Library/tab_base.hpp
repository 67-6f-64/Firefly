#pragma once	

#include "native.hpp"

#include "window.hpp"
#include "widget.hpp"

#include <functional>
#include <vector>

namespace firefly
{
	#define WC_CUSTOM_TABBASE "CustomTabbase"
	
	#define TBASE_HOVERED 0x01
	#define TBASE_SELECTED 0x02

	class tab_base_item
	{
	public:
		tab_base_item(std::string const& name, bool activated = true);
		virtual ~tab_base_item();
		
		tab_base_item& operator=(const tab_base_item& item);

	public:
		std::string const name;
		bool activated;
		std::vector<std::shared_ptr<widget>> relatives;
	};
	
	enum tab_base_event_type
	{
		on_selection_change,
		on_selection_changed,
	};
	
	template <class T = tab_base_item>
	class tab_base : public widget
	{
		typedef std::function<void(int)> tab_base_event_function_t;

	protected:
		typedef std::function<bool(std::shared_ptr<T>, int, RECT*)> tab_base_enum_func;

	private:
		void init()
		{
			SetRectEmpty(&this->rc_client);
			this->tab_base_items.clear();

			this->has_drawn_background = false;
			
			this->hover_index = -1;
			SetRectEmpty(&this->hover_rect);

			this->selection_index = -1;
			SetRectEmpty(&this->selection_rect);

			this->background_back_color = 0;
			this->background_front_color = 0;

			this->on_selection_change_event_function = nullptr;
		}

	public:
		virtual void remove_tab(int index)
		{
			this->tab_base_items.erase(this->tab_base_items.begin() + index);

			if (IsWindowVisible(this->handle()))
				this->redraw(&this->rc_client);
		}
		
		bool relate(int index, std::shared_ptr<widget> widget)
		{
			if (static_cast<int>(this->tab_base_items.size()) <= index)
				return false;

			this->tab_base_items.at(index).get()->relatives.push_back(widget);
			ShowWindow(widget.get()->handle(), SW_HIDE);
			return true;
		}

		void set_event(tab_base_event_type event_type, tab_base_event_function_t tab_base_event_function)
		{
			if (event_type == tab_base_event_type::on_selection_change)
				this->on_selection_change_event_function = tab_base_event_function;
			if (event_type == tab_base_event_type::on_selection_changed)
				this->on_selection_changed_event_function = tab_base_event_function;
		}

		bool set_selection(int index, RECT* rc = NULL)
		{
			if (index >= 0)
			{
				if (index != this->selection_index)
				{
					if (this->on_selection_change_event_function)
						this->on_selection_change_event_function(this->selection_index);

					this->reset_selection_state();

					if (!dynamic_cast<tab_base_item&>(*this->tab_base_items.at(index).get()).activated)
						return false;

					this->selection_index = index;

					if (rc)
					{
						this->selection_rect = { rc->left, rc->top, rc->right, rc->bottom };
					}
					else
					{
						this->enumerate_tabs([&, this](std::shared_ptr<T> item, int index, RECT* rc) -> bool
						{
							if (index == this->selection_index)
							{
								this->selection_rect = { rc->left, rc->top, rc->right, rc->bottom };
								return false;
							}

							return true;
						});
					}

					this->redraw(&this->selection_rect);
					this->show_relative(this->selection_index, true);

					if (this->on_selection_changed_event_function)
						this->on_selection_changed_event_function(this->selection_index);
				}
			}
			else if (index == -1)
			{
				if (this->on_selection_change_event_function && this->selection_index != index)
					this->on_selection_change_event_function(this->selection_index);

				this->reset_selection_state();
				
				if (this->on_selection_changed_event_function)
					this->on_selection_changed_event_function(this->selection_index);
			}
			
			return false;
		}
		
		template <typename... Args>
		int add_tab(std::string const& caption, bool activated, Args&&... args)
		{
			this->tab_base_items.push_back(std::make_shared<T>(caption, activated, std::forward<Args>(args)...));
			return (this->tab_base_items.size() - 1);
		}
		
		void set_background_colors(COLORREF back, COLORREF front)
		{
			this->background_back_color = back;
			this->background_front_color = front;
		}
		
	protected:
		tab_base(window& parent_window)
			: widget(parent_window)
		{
			this->init();
			this->set_default_message_handlers();
		}

		tab_base(widget& parent_widget)
			: widget(parent_widget)
		{
			this->init();
			this->set_default_message_handlers();
		}

		virtual ~tab_base()
		{

		}

		void create(rectangle& rect = rectangle())
		{
			if (!this->build(WC_CUSTOM_TABBASE, "", rect, WS_VISIBLE | WS_CHILD))
				throw std::exception("Failed to create 'tab_base' widget");

			if (IsRectEmpty(&this->rc_client))
				GetClientRect(this->handle(), &this->rc_client);
		}

		COLORREF get_background_back_color() const
		{
			return this->background_back_color;
		}

		COLORREF get_background_front_color() const
		{
			return this->background_front_color;
		}
		
		void redraw(RECT* rc)
		{
			InvalidateRect(this->handle(), rc, FALSE);
			//RedrawWindow(this->handle(), rc, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT);
		}
		
		int get_selection() const
		{
			return this->selection_index;
		}

		void reset_selection_state()
		{
			if (this->selection_index != -1)
			{
				this->show_relative(this->selection_index, false);
				this->selection_index = -1;
				this->redraw(&this->selection_rect);
			}
		}
		
	private:
		void on_mouse_leave()
		{
			if (this->hover_index != -1)
			{
				this->hover_index = -1;
				this->redraw(&this->hover_rect);
			}
		}

		void on_mouse_move(unsigned int keys, POINT pt)
		{
			if (this->enumerate_tabs([&, this](std::shared_ptr<T> item, int index, RECT* rc) -> bool
			{
				if (PtInRect(rc, pt))
				{
					if (index >= 0 && index != this->hover_index)
					{
						this->reset_hover_state();

						this->hover_index = index;
						this->hover_rect = { rc->left, rc->top, rc->right, rc->bottom };
						
						this->redraw(rc);
					}

					return false;
				}

				return true;
			}))
			{
				this->reset_hover_state();
			}
		}
		
		void on_mouse_left_down(HWND hWnd, unsigned int keys, POINT pt)
		{
			SetFocus(hWnd);
			SetCapture(hWnd);
		}
		
		void on_mouse_left_up(HWND hWnd, unsigned int keys, POINT pt)
		{
			if (PtInRect(&this->rc_client, pt))
			{
				if (this->enumerate_tabs([&, this](std::shared_ptr<T> item, int index, RECT* rc) -> bool
				{
					if (PtInRect(rc, pt))
						return this->set_selection(index, rc);
					
					return true;
				}))
				{
					this->set_selection(-1);
				}
			}

			ReleaseCapture();
		}

		void reset_hover_state()
		{
			if (this->hover_index != -1)
			{
				this->hover_index = -1;
				this->redraw(&this->hover_rect);
			}
		}

		void show_relative(int index, bool show)
		{
			for (std::shared_ptr<widget> relative : dynamic_cast<tab_base_item&>(*this->tab_base_items.at(index).get()).relatives)
				ShowWindow(relative.get()->handle(), show ? SW_SHOW : SW_HIDE);
		}

		void set_default_message_handlers()
		{
			this->add_message_handler(WM_ERASEBKGND, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
			{
				if (!this->has_drawn_background)
				{
					this->draw_rectangle(&this->rc_client, this->background_back_color);

					RECT rc_background_front = this->calculate_background_front_rect();

					this->draw_rectangle(&rc_background_front, this->background_front_color);
					this->commit_drawing(reinterpret_cast<HDC>(wParam), &this->rc_client);
				}

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
		}
		
		LRESULT paint_client_area(HDC hdc, RECT* rc_paint, bool erase)
		{
			this->draw_rectangle(&this->rc_client, this->background_back_color);

			RECT rc_background_front = this->calculate_background_front_rect();

			this->draw_rectangle(&rc_background_front, this->background_front_color);

			this->enumerate_tabs([&, this](std::shared_ptr<T> item, int index, RECT* rc) -> bool
			{
				RECT rc_intersection;

				if (!IntersectRect(&rc_intersection, rc, rc_paint))
					return true;
					
				bool hovered = (index == this->hover_index);
				bool selected = (index == this->selection_index);
				
				int flags = (hovered ? TBASE_HOVERED : 0) | (selected ? TBASE_SELECTED : 0);
				return this->draw_tab_item(item, index, flags, rc);
			});
		
			this->commit_drawing(hdc, rc_paint);
			return 0;
		}
		
	private:
		virtual bool enumerate_tabs(tab_base_enum_func enum_func) = 0;
		virtual bool draw_tab_item(std::shared_ptr<T> item, int index, int flags, RECT* rc) = 0;
		virtual RECT calculate_background_front_rect() = 0;
		
	protected:
		RECT rc_client;
		std::vector<std::shared_ptr<T>> tab_base_items;

	private:
		bool has_drawn_background;
		
		int hover_index;
		RECT hover_rect;

		int selection_index;
		RECT selection_rect;
		
		COLORREF background_back_color;
		COLORREF background_front_color;
		
		tab_base_event_function_t on_selection_change_event_function;
		tab_base_event_function_t on_selection_changed_event_function;
	};
}