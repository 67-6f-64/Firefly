#include "combobox.hpp"
#include "resource.hpp"

#include <strsafe.h>
#include <regex>

namespace firefly
{
	LRESULT CALLBACK combobox::default_list_window_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		combobox* object = reinterpret_cast<combobox*>(GetWindowLong(hWnd, GWLP_USERDATA));
		
		if (uMsg == WM_NCDESTROY)
		{
			SetWindowLong(hWnd, GWLP_USERDATA, reinterpret_cast<LONG>(object->original_list_window_proc));
			return 0;
		}
		else
		{
			if (object)
				return object->list_window_proc(hWnd, uMsg, wParam, lParam);
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	void combobox::init()
	{
		this->center = false;

		SetRectEmpty(&this->rc_client);
		this->arrow_font = NULL;

		this->function_on_selection_change = nullptr;
		this->original_list_window_proc = nullptr;
	}

	combobox::combobox(window& parent_window)
		: menu_clickable(parent_window)
	{
		this->init();
		this->set_default_message_handlers();
	}
			
	combobox::combobox(widget& parent_widget)
		: menu_clickable(parent_widget)
	{
		this->init();
		this->set_default_message_handlers();
	}

	combobox::~combobox()
	{
		if (this->arrow_font)
			DeleteFont(this->arrow_font);
	}
		
	void combobox::set_event(combobox_event event_type, combobox_event_function_t event_function)
	{
		if (event_type == combobox_event::on_selection_change)
			this->function_on_selection_change = event_function;
	}

	int combobox::add_item(std::string const& caption, COLORREF color)
	{
		int index = ComboBox_AddString(this->handle(), caption.c_str());
		
		if (index != CB_ERR)
		{
			if (ComboBox_SetItemData(this->handle(), index, color) != CB_ERR)
				return index;
		}

		return CB_ERR;
	}

	bool combobox::delete_item(int index)
	{
		return (ComboBox_DeleteString(this->handle(), index) != CB_ERR);
	}
	
	void combobox::clear_items()
	{
		ComboBox_ResetContent(this->handle());
	}
	
	int combobox::get_count()
	{
		return ComboBox_GetCount(this->handle());
	}

	int combobox::get_selection()
	{
		return ComboBox_GetCurSel(this->handle());
	}

	bool combobox::set_selection(int index, bool handle_event)
	{
		if (ComboBox_SetCurSel(this->handle(), index) != CB_ERR)
		{
			if (handle_event)
				if (this->function_on_selection_change)
					this->function_on_selection_change(index);

			return true;
		}

		return false;
	}
	
	std::string combobox::get_text()
	{
		int text_length = ComboBox_GetTextLength(this->handle());

		char* text_buffer = new char[text_length + 1];
		memset(text_buffer, 0, text_length + 1);

		text_length = ComboBox_GetText(this->handle(), text_buffer, text_length + 1);

		std::string result(text_buffer);
		delete[] text_buffer;
		return result;
	}

	bool combobox::set_text(std::string const& caption)
	{
		return this->set_selection(this->add_item(caption), false);
	}

	std::string combobox::get_item_text(int index)
	{
		int text_length = ComboBox_GetLBTextLen(this->handle(), index);

		char* text_buffer = new char[text_length + 1];
		memset(text_buffer, 0, text_length + 1);

		text_length = ComboBox_GetLBText(this->handle(), index, text_buffer);

		std::string result(text_buffer);
		delete[] text_buffer;
		return result;
	}

	std::vector<std::string> combobox::get_items()
	{
		std::vector<std::string> results;

		for (int i = 0, count = ComboBox_GetCount(this->handle()); i < count; i++)
			results.push_back(this->get_item_text(i));

		return results;
	}

	void combobox::add_token_width(int width)
	{
		this->token_widths.push_back(width);
	}

	void combobox::clear_token_widths()
	{
		this->token_widths.clear();
	}

	void combobox::create(rectangle& rect, bool center)
	{
		this->center = center;

		if (!this->build(WC_COMBOBOX, "", rect, WS_VISIBLE | WS_CHILD | WS_TABSTOP | CBS_HASSTRINGS | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED))
			throw std::exception("Failed to create 'combobox' widget");

		this->set_background(RGB(51, 51, 51));
		this->set_foreground(RGB(255, 255, 255));

		if (IsRectEmpty(&this->rc_client))
			GetClientRect(this->handle(), &this->rc_client);

		ComboBox_SetMinVisible(this->handle(), 40);
		
		this->arrow_font = this->create_font("Arial", 12, 6, FW_DONTCARE);
		this->subclass_listbox_control();
	}

	int combobox::on_show_menu(POINT pt)
	{
		int index = this->get_selection();

		if (index == CB_ERR)
			index = 0;

		return index;
	}

	void combobox::subclass_listbox_control()
	{
		COMBOBOXINFO combobox_info;
		memset(&combobox_info, 0, sizeof(COMBOBOXINFO));

		combobox_info.cbSize = sizeof(COMBOBOXINFO);

		if (GetComboBoxInfo(this->handle(), &combobox_info))
		{
			/* resize original drawing space */
			HDC hdc = GetDC(this->handle());
					
			int offset_x = combobox_info.rcItem.left - this->rc_client.left;
			int offset_y = combobox_info.rcItem.top - this->rc_client.top;

			this->resize_graphics(hdc, this->rc_client.right - this->rc_client.left, this->rc_client.bottom - this->rc_client.top, offset_x, offset_y);

			ReleaseDC(this->handle(), hdc);

			/* subclass listbox control */
			SetWindowLong(combobox_info.hwndList, GWLP_USERDATA, reinterpret_cast<LONG>(this));
			this->original_list_window_proc = reinterpret_cast<WNDPROC>(SetWindowLong(combobox_info.hwndList, GWLP_WNDPROC, reinterpret_cast<LONG>(default_list_window_proc)));
			
			RECT rc_listbox;
			GetClientRect(combobox_info.hwndList, &rc_listbox);
			
			this->list_drawer = std::make_unique<image_drawer>(this->instance(), combobox_info.hwndList);
			this->list_drawer.get()->reset_graphics(rectangle(rc_listbox.left, rc_listbox.top, rc_listbox.right - rc_listbox.left, rc_listbox.bottom - rc_listbox.top));
		}
	}
	
	void combobox::set_default_message_handlers()
	{
		this->add_message_handler(CUSTOM_CTLCOLOR, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			COMBOBOXINFO combobox_info;
			memset(&combobox_info, 0, sizeof(COMBOBOXINFO));

			combobox_info.cbSize = sizeof(COMBOBOXINFO);

			if (GetComboBoxInfo(hWnd, &combobox_info) && !IsRectEmpty(&this->rc_client))
			{
				int dx = combobox_info.rcItem.left - this->rc_client.left;
				int dy = combobox_info.rcItem.top - this->rc_client.top;

				InflateRect(&combobox_info.rcItem, dx, dy);
				combobox_info.rcItem.right = this->rc_client.right;

				/* draw edit part */
				COLORREF background_color = (IsWindowEnabled(this->handle()) ? this->background() : RGB(40, 40, 40));
				
				this->draw_rectangle(&combobox_info.rcItem, background_color);
				this->draw_round_rectangle(&combobox_info.rcItem, size(2, 2), background_color, this->is_focused, RGB(63, 97, 152), 2);
				
				/* draw dropdown arrow */
				this->draw_text_wide(std::wstring(L"\u25BC"), &combobox_info.rcButton, RGB(0, 0, 0), DT_CENTER | DT_SINGLELINE | DT_VCENTER, this->arrow_font);
				this->commit_drawing(reinterpret_cast<HDC>(wParam), &combobox_info.rcItem);
			}

			return reinterpret_cast<LRESULT>(GetStockObject(NULL_BRUSH));
		});
		
		this->add_message_handler(CUSTOM_COMMAND, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			if (HIWORD(wParam) == CBN_SELCHANGE)
			{
				if (this->function_on_selection_change)
					this->function_on_selection_change(ComboBox_GetCurSel(hWnd));
			}

			return this->original_proc(hWnd, CUSTOM_COMMAND, wParam, lParam);
		});
		
		this->add_message_handler(OWNER_DRAWITEM, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			DRAWITEMSTRUCT* draw_item = reinterpret_cast<DRAWITEMSTRUCT*>(lParam);
			
			if (draw_item->CtlType == ODT_COMBOBOX)
				return this->owner_draw_list_item(draw_item);

			return 0;
		});
		
		this->add_message_handler(WM_DRAWITEM, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			DRAWITEMSTRUCT* draw_item = reinterpret_cast<DRAWITEMSTRUCT*>(lParam);

			if (draw_item->CtlType == ODT_MENU)
			{
				this->menu_clickable::on_draw_item(hWnd, draw_item);
				return TRUE;
			}
			
			return this->original_proc(hWnd, WM_DRAWITEM, wParam, lParam);
		});
	}

	LRESULT combobox::list_window_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_SIZE:
			if (wParam == SIZE_RESTORED)
			{
				int width = LOWORD(lParam);
				int height = (this->get_count() > 0 ? HIWORD(lParam) : 3);
				
				if (width != 0 && height != 0)
				{
					HDC hdc = GetDC(hWnd);
					
					int offset_x = GetSystemMetrics(SM_CXBORDER) * 2;
					int offset_y = GetSystemMetrics(SM_CYBORDER) * 2;

					this->list_drawer.get()->resize_graphics(hdc, width, height, offset_x, offset_y);

					ReleaseDC(hWnd, hdc);
				}
			}
			break;

		case WM_NCPAINT:
			{
				RECT rc_border;
				GetClientRect(hWnd, &rc_border);

				rc_border.right += GetSystemMetrics(SM_CXBORDER) * 2;
				rc_border.bottom += GetSystemMetrics(SM_CYBORDER) * 2;

				HDC hdc = GetDCEx(hWnd, reinterpret_cast<HRGN>(wParam), DCX_WINDOW | DCX_PARENTCLIP);

				this->list_drawer.get()->draw_hdc(hdc, &rc_border);
				this->list_drawer.get()->draw_rectangle(&rc_border, NULL, true, RGB(122, 156, 211), 0, reinterpret_cast<HBRUSH>(GetStockObject(NULL_BRUSH)));
				this->list_drawer.get()->commit_drawing(hdc, &rc_border);

				ReleaseDC(hWnd, hdc);
				return 0;
			}

		case WM_ERASEBKGND:
			return TRUE;
			
		case WM_CAPTURECHANGED:
			return 0;

		case WM_RBUTTONDOWN:
			SetFocus(hWnd);
			SetCapture(hWnd);
			break;

		case WM_RBUTTONUP:
			{
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				this->menu_clickable::show_menu(hWnd, pt);
			}
			break;

		case WM_INITMENU:
			{
				this->menu_clickable::on_init_menu();
				return 0;
			}
			
		case WM_MEASUREITEM:
			{
				this->menu_clickable::on_measure_item(hWnd, reinterpret_cast<MEASUREITEMSTRUCT*>(lParam));
				return TRUE;
			}
		
		case WM_DRAWITEM:
			{
				this->menu_clickable::on_draw_item(hWnd, reinterpret_cast<DRAWITEMSTRUCT*>(lParam));
				return TRUE;
			}

		default:
			break;
		}

		return CallWindowProc(this->original_list_window_proc, hWnd, uMsg, wParam, lParam);
	}

	LRESULT combobox::owner_draw_list_item(DRAWITEMSTRUCT* owner_draw)
	{
		if (owner_draw->itemID == -1 || (owner_draw->itemAction & ODA_FOCUS))
			return 0;

		/* draw item background*/
		COLORREF background_color = 0;
		
		if (!IsWindowEnabled(this->handle()))
			background_color = RGB(40, 40, 40);
		else if ((owner_draw->itemState & ODS_SELECTED) && !(owner_draw->itemState & ODS_COMBOBOXEDIT))
			background_color = GetSysColor(COLOR_HIGHLIGHT);
		else
			background_color = this->background();

		this->list_drawer.get()->draw_rectangle(&owner_draw->rcItem, background_color);
		
		/* draw item label */
		COLORREF text_color = 0;
		
		if (owner_draw->itemState & ODS_SELECTED)
			text_color = GetSysColor(COLOR_HIGHLIGHTTEXT);
		else if (owner_draw->itemData)
			text_color = owner_draw->itemData;
		else
			text_color = this->foreground();
		
		int text_length = ComboBox_GetLBTextLen(owner_draw->hwndItem, owner_draw->itemID);

		char* text_buffer = new char[text_length + 1];
		memset(text_buffer, 0, text_length + 1);

		text_length = ComboBox_GetLBText(owner_draw->hwndItem, owner_draw->itemID, text_buffer);

		std::vector<std::string> tabbed_strings;
		this->split_string(text_buffer, tabbed_strings);
		
		RECT rc_text(owner_draw->rcItem);
		
		if (this->center)
		{
			rc_text.left = this->rc_client.left;
			rc_text.right = this->rc_client.right;
		}
		else
		{
			rc_text.left += 2;
		}

		if (tabbed_strings.size() <= 1 || (owner_draw->itemState & ODS_COMBOBOXEDIT))
		{
			this->list_drawer.get()->draw_text(tabbed_strings.back(), &rc_text, text_color, (this->center ? DT_CENTER : DT_LEFT) | DT_SINGLELINE | DT_VCENTER, this->font());
		}
		else
		{
			for (std::size_t i = 0, indent = 0; i < tabbed_strings.size(); i++)
			{
				int column_width = ((this->rc_client.right - this->rc_client.left) * this->token_widths[i] / 100);

				rc_text.left += indent;
				rc_text.right = rc_text.left + column_width;

				indent = column_width;

				this->list_drawer.get()->draw_text(tabbed_strings[i], &rc_text, text_color, (this->center ? DT_CENTER : DT_LEFT) | DT_SINGLELINE | DT_VCENTER, this->font());
			}
		}

		delete[] text_buffer;
		
		this->list_drawer.get()->commit_drawing(owner_draw->hDC, &owner_draw->rcItem);
		return 0;
	}
	
	void combobox::split_string(std::string const& str, std::vector<std::string>& output)
	{
		output.clear();

		for (char* token = strtok(const_cast<char*>(str.c_str()), "\t\0"); token != NULL; token = strtok(NULL, "\t\0"))
		{
			output.push_back(std::string(token));
		}
	}
}