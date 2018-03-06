#include "tabpage.hpp"
#include "seperator.hpp"

namespace firefly
{
	bool tabpage::is_child_class(HWND hWnd, std::string const& child_class_name)
	{
		char class_name[256];
		GetClassName(hWnd, class_name, sizeof(class_name));

		return (strcmp(class_name, child_class_name.c_str()) == 0);
	}

	tabpage::tabpage(window& parent_window, COLORREF background_color)
		: panel(parent_window)
	{
		this->set_background(background_color == 0 ? RGB(24, 24, 24) : background_color);
	}
			
	tabpage::~tabpage()
	{

	}
	
	bool tabpage::on_mouse_wheel(int delta)
	{
		if (this->scroll_bar)
			this->scroll_bar.get()->on_mouse_wheel(delta);

		return false;
	}

	void tabpage::store_control(std::shared_ptr<widget> control)
	{
		this->controls.push_back(control);
	}
	
	void tabpage::clear_controls()
	{
		this->controls.clear();
	}

	int tabpage::create_seperator(std::string const& caption, int y)
	{
		const int seperator_height = 32;

		RECT rc;
		GetClientRect(this->handle(), &rc);

		std::shared_ptr<seperator> tabpage_seperator = std::make_shared<seperator>(*this);
		tabpage_seperator.get()->create(caption, rectangle(STANDARD_INDENT, y, (rc.right - rc.left) - (TABPAGE_INDENT_LEFT + TABPAGE_INDENT_RIGHT), seperator_height));
		
		this->store_control(tabpage_seperator);
		return (y + seperator_height + STANDARD_INDENT);
	}
	
	void tabpage::reload_edit_controls()
	{
		WNDENUMPROC enum_proc = [](HWND hWnd, LPARAM lParam) -> BOOL
		{
			if (tabpage::is_child_class(hWnd, "Edit"))
				InvalidateRect(hWnd, NULL, TRUE);

			return TRUE;
		};

		EnumChildWindows(this->handle(), enum_proc, NULL);
	}
	
	void tabpage::reset_scrollbar(bool exclude_invisible)
	{
		RECT rc;
		GetClientRect(this->handle(), &rc);

		this->scroll_bar.get()->auto_reset(*this, this->last_child_y(rc.bottom - rc.top, exclude_invisible));
	}
	
	void tabpage::reset_scrollbar_position()
	{
		this->scroll_bar.get()->scroll_to(0);
	}

	void tabpage::create(rectangle& rect, bool show)
	{
		this->create_default_handlers();

		panel::create(rect, true, show);

		this->scroll_bar = std::make_shared<scrollbar>(*this);
		this->scroll_bar.get()->create(rectangle(rect.width - SCROLLBAR_WIDTH, 0, SCROLLBAR_WIDTH, rect.height));
		this->scroll_bar.get()->bind_control(this);
		
		this->create_controls(rect);
		this->reset_scrollbar();
	}
	
	void tabpage::scroll_to_control(HWND hWnd, RECT* rc_screen_control)
	{
		RECT rc_visible;
		SetRectEmpty(&rc_visible);

		if (this->scroll_bar.get()->get_visible_rect(&rc_visible))
		{
			ScreenToClient(hWnd, reinterpret_cast<POINT*>(&rc_screen_control->left));
			ScreenToClient(hWnd, reinterpret_cast<POINT*>(&rc_screen_control->right));

			this->scroll_bar.get()->map_points_relatively(rc_screen_control);

			if (rc_screen_control->top < rc_visible.top)
				this->scroll_bar.get()->scroll_to_coord(rc_screen_control->top - 10);
			else if (rc_screen_control->bottom > rc_visible.bottom)
				this->scroll_bar.get()->scroll_to_coord(rc_screen_control->bottom + 10, true);
		}
	}
	
	void tabpage::create_controls(rectangle& rect)
	{
		/* placeholder for virtual function */
	}

	void tabpage::create_handlers()
	{
		/* placeholder for virtual function */
	}
	
	int tabpage::last_child_y(int height, bool exclude_invisible)
	{
		typedef struct _enum_proc_data 
		{
			_enum_proc_data(int height, bool exclude_invisible) 
				: height(height), exclude_invisible(exclude_invisible)
			{
				this->scroll_button_down = NULL;
				this->scroll_button_up = NULL;
			}

			int height;
			bool exclude_invisible;
			HWND scroll_button_down;
			HWND scroll_button_up;
		} enum_proc_data;

		WNDENUMPROC enum_proc = [](HWND hWnd, LPARAM lParam) -> BOOL
		{
			enum_proc_data* proc_data = reinterpret_cast<enum_proc_data*>(lParam);

			if (!proc_data->exclude_invisible || IsWindowVisible(hWnd))
			{
				if (!tabpage::is_child_class(hWnd, "CustomScrollbar") && hWnd != proc_data->scroll_button_down && hWnd != proc_data->scroll_button_up)
				{
					RECT rc_child;
					GetWindowRect(hWnd, &rc_child);

					HWND hWndParent = GetParent(hWnd);
					ScreenToClient(hWndParent, reinterpret_cast<POINT*>(&rc_child.left));
					ScreenToClient(hWndParent, reinterpret_cast<POINT*>(&rc_child.right));

					proc_data->height = max(proc_data->height, rc_child.bottom + STANDARD_INDENT);
				}
			}

			return TRUE;
		};

		enum_proc_data proc_data(height, exclude_invisible);
		proc_data.scroll_button_down = this->scroll_bar.get()->get_scroll_button_down();
		proc_data.scroll_button_up = this->scroll_bar.get()->get_scroll_button_up();

		EnumChildWindows(this->handle(), enum_proc, reinterpret_cast<LPARAM>(&proc_data));
		return proc_data.height;
	}
	
	void tabpage::create_default_handlers()
	{
		this->add_message_handler(WM_COMMAND, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			switch (HIWORD(wParam))
			{
			case EN_SETFOCUS:
			case CBN_SETFOCUS:
				{
					RECT rc_widget;
					SetRectEmpty(&rc_widget);

					if (GetWindowRect(reinterpret_cast<HWND>(lParam), &rc_widget))
						this->scroll_to_control(hWnd, &rc_widget);
				}
				break;

			default:
				break;
			}
			
			return SendMessage(reinterpret_cast<HWND>(lParam), CUSTOM_COMMAND, wParam, lParam);
		});
		
		this->add_message_handler(WM_MEASUREITEM, [](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			MEASUREITEMSTRUCT* measure_item = reinterpret_cast<MEASUREITEMSTRUCT*>(lParam);

			if (measure_item->CtlType == ODT_COMBOBOX)
				measure_item->itemHeight = 18;

			return 0;
		});

		return this->create_handlers();
	}
}