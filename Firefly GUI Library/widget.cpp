#include "widget.hpp"
#include "resource.hpp"

#include "list_view_base.hpp"
#include "scrollbar.hpp"
#include "tab_base.hpp"

#include "tooltip.hpp"

namespace firefly
{
	namespace custom_control
	{
		LRESULT CALLBACK default_control_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}

		bool register_control(const char* class_name, bool double_click = false)
		{
			WNDCLASS wc;
			memset(&wc, 0, sizeof(WNDCLASS));

			wc.style = CS_GLOBALCLASS | CS_HREDRAW | CS_VREDRAW | (double_click ? CS_DBLCLKS : 0);
			wc.lpfnWndProc = default_control_proc;
			wc.hCursor = LoadCursor(NULL, IDC_ARROW);
			wc.lpszClassName = class_name;

			return (RegisterClass(&wc) != FALSE);
		}

		bool unregister_control(const char* class_name)
		{
			return (UnregisterClass(class_name, 0) != FALSE);
		}

		void initialize()
		{
			register_control(WC_CUSTOM_LIST_VIEW_BASE, true);
			register_control(WC_CUSTOM_SCROLLBAR);
			register_control(WC_CUSTOM_TABBASE);
		}
		
		void uninitialize()
		{
			unregister_control(WC_CUSTOM_LIST_VIEW_BASE);
			unregister_control(WC_CUSTOM_SCROLLBAR);
			unregister_control(WC_CUSTOM_TABBASE);
		}
	}

	void widget::init(base_window& base)
	{
		this->is_focused = false;
		this->is_hovered = false;

		this->set_font(this->create_font("Arial", 15));
		this->set_foreground(base.foreground());
		this->set_background(base.background());
	}

	bool widget::set_tooltip(std::string const& caption)
	{
		return (this->infotip.get()->create() && this->infotip.get()->bind_control(this->handle(), caption));
	}
	
	void widget::redraw(RECT* rc, bool erase)
	{
		InvalidateRect(this->handle(), rc, erase ? TRUE : FALSE);
	}

	widget::widget(window& parent_window)
		: message_controller(parent_window.instance(), parent_window.handle(), false)
	{
		this->init(parent_window);
		this->infotip = std::make_unique<tooltip>(parent_window);

		this->set_default_widget_handlers();
	}

	widget::widget(widget& parent_widget)
		: message_controller(parent_widget.instance(), parent_widget.handle(), false)
	{
		this->init(parent_widget);
		this->infotip = std::make_unique<tooltip>(parent_widget);

		this->set_default_widget_handlers();
	}

	widget::~widget()
	{
		if (this->infotip)
			this->infotip.reset();
	}
	
	bool widget::build(std::string const& class_name, std::string const& widget_name, rectangle& rect, unsigned int style, unsigned int ex_style)
	{
		this->set_handle(CreateWindowEx(ex_style, class_name.c_str(), widget_name.c_str(), style, rect.x, rect.y, 
			rect.width, rect.height, this->parent_handle(), NULL, this->instance(), NULL));

		if (!this->handle())
			return false;
		
		this->reset_graphics(rect);
		this->reset_font();
		
		return this->initialize_widget(this->handle());
	}
	
	void widget::reset_font()
	{
		SendMessage(this->handle(), WM_SETFONT, reinterpret_cast<WPARAM>(this->font()), TRUE);
	}
	
	bool widget::on_mouse_wheel(int delta)
	{
		/* placeholder for virtual function */
		return false;
	}
	
	void widget::on_mouse_enter(unsigned int keys, POINT pt)
	{
		/* placeholder for virtual function */
	}
	
	void widget::on_mouse_move(unsigned int keys, POINT pt)
	{
		/* placeholder for virtual function */
	}
	
	void widget::on_mouse_leave()
	{
		/* placeholder for virtual function */
	}
	
	void widget::on_mouse_left_down(HWND hWnd, unsigned int keys, POINT pt)
	{
		/* placeholder for virtual function */
	}
	
	void widget::on_mouse_left_up(HWND hWnd, unsigned int keys, POINT pt)
	{
		/* placeholder for virtual function */
	}
	
	void widget::on_mouse_left_double_click(HWND hWnd, unsigned int keys, POINT pt)
	{
		/* placeholder for virtual function */
	}

	void widget::on_mouse_right_down(HWND hWnd, unsigned int keys, POINT pt)
	{
		/* placeholder for virtual function */
	}

	void widget::on_mouse_right_up(HWND hWnd, unsigned int keys, POINT pt)
	{
		/* placeholder for virtual function */
	}
		
	void widget::on_mouse_right_double_click(HWND hWnd, unsigned int keys, POINT pt)
	{
		/* placeholder for virtual function */
	}
	
	void widget::on_set_focus(HWND old_focus)
	{
		/* placeholder for virtual function */
	}

	void widget::on_kill_focus(HWND new_focus)
	{
		/* placeholder for virtual function */
	}
	
	void widget::on_measure_item(HWND hWnd, MEASUREITEMSTRUCT* measure_item)
	{
		/* placeholder for virtual function */
	}

	void widget::on_draw_item(HWND hWnd, DRAWITEMSTRUCT* draw_item)
	{
		/* placeholder for virtual function */
	}
	
	void widget::set_default_widget_handlers()
	{
		this->add_message_handler(WM_MOUSEWHEEL, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			if (this->on_mouse_wheel(GET_WHEEL_DELTA_WPARAM(wParam)))
				return TRUE;
			
			return this->original_proc(hWnd, WM_MOUSEWHEEL, wParam, lParam);
		});

		this->add_message_handler(WM_MOUSEMOVE, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

			if (!this->is_hovered)
			{
				TRACKMOUSEEVENT tme;
				memset(&tme, 0, sizeof(TRACKMOUSEEVENT));

				tme.cbSize = sizeof(TRACKMOUSEEVENT);
				tme.dwFlags = TME_LEAVE;
				tme.hwndTrack = hWnd;

				if (TrackMouseEvent(&tme))
				{
					this->is_hovered = true;
					this->on_mouse_enter(static_cast<unsigned int>(wParam), pt);
				}
			}
			
			this->on_mouse_move(static_cast<unsigned int>(wParam), pt);
			return this->original_proc(hWnd, WM_MOUSEMOVE, wParam, lParam);
		});
		
		this->add_message_handler(WM_MOUSELEAVE, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			this->is_hovered = false;
			this->on_mouse_leave();
			return this->original_proc(hWnd, WM_MOUSELEAVE, wParam, lParam);
		});
		
		this->add_message_handler(WM_LBUTTONDOWN, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			this->on_mouse_left_down(hWnd, static_cast<unsigned int>(wParam), pt);

			return this->original_proc(hWnd, WM_LBUTTONDOWN, wParam, lParam);
		});
		
		this->add_message_handler(WM_LBUTTONUP, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			this->on_mouse_left_up(hWnd, static_cast<unsigned int>(wParam), pt);
			
			return this->original_proc(hWnd, WM_LBUTTONUP, wParam, lParam);
		});
		
		this->add_message_handler(WM_LBUTTONDBLCLK, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			this->on_mouse_left_double_click(hWnd, static_cast<unsigned int>(wParam), pt);
			
			return this->original_proc(hWnd, WM_LBUTTONDBLCLK, wParam, lParam);
		});

		this->add_message_handler(WM_RBUTTONDOWN, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			this->on_mouse_right_down(hWnd, static_cast<unsigned int>(wParam), pt);

			return this->original_proc(hWnd, WM_RBUTTONDOWN, wParam, lParam);
		});
		
		this->add_message_handler(WM_RBUTTONUP, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			this->on_mouse_right_up(hWnd, static_cast<unsigned int>(wParam), pt);
			
			return this->original_proc(hWnd, WM_RBUTTONUP, wParam, lParam);
		});
		
		this->add_message_handler(WM_RBUTTONDBLCLK, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			this->on_mouse_right_double_click(hWnd, static_cast<unsigned int>(wParam), pt);
			
			return this->original_proc(hWnd, WM_RBUTTONDBLCLK, wParam, lParam);
		});
		
		this->add_message_handler(WM_SETFOCUS, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			this->is_focused = true;
			this->on_set_focus(reinterpret_cast<HWND>(wParam));
			return this->original_proc(hWnd, WM_SETFOCUS, wParam, lParam);
		});

		this->add_message_handler(WM_KILLFOCUS, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			this->is_focused = false;
			this->on_kill_focus(reinterpret_cast<HWND>(wParam));
			return this->original_proc(hWnd, WM_KILLFOCUS, wParam, lParam);
		});

		this->add_message_handler(WM_MEASUREITEM, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			this->on_measure_item(hWnd, reinterpret_cast<MEASUREITEMSTRUCT*>(lParam));
			return TRUE;
		});
		
		//this->add_message_handler(WM_DRAWITEM, [](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		//{
		//	return SendMessage(reinterpret_cast<DRAWITEMSTRUCT*>(lParam)->hwndItem, OWNER_DRAWITEM, wParam, lParam);
		//});
		
		this->add_message_handler(WM_NOTIFY, [](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			return SendMessage(reinterpret_cast<NMHDR*>(lParam)->hwndFrom, CUSTOM_NOTIFY, wParam, lParam);
		});
		
		//this->add_message_handler(WM_COMMAND, [](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		//{
		//	return SendMessage(reinterpret_cast<HWND>(lParam), CUSTOM_COMMAND, wParam, lParam);
		//});
	}
}