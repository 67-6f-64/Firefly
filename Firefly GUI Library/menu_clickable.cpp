#include "menu_clickable.hpp"

#include <strsafe.h>

namespace firefly
{
	LRESULT CALLBACK menu_clickable::default_menu_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		menu_clickable* object = reinterpret_cast<menu_clickable*>(GetWindowLong(hWnd, GWLP_USERDATA));

		if (uMsg == WM_NCDESTROY)
		{
			SetWindowLong(hWnd, GWLP_USERDATA, reinterpret_cast<LONG>(object->original_menu_window_proc));
			return 0;
		}
		else
		{
			if (object)
				return object->menu_window_proc(hWnd, uMsg, wParam, lParam);
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	menu_clickable::menu_clickable(window& parent_window)
		: widget(parent_window)
	{
		this->menu = CreatePopupMenu();
		this->set_default_message_handlers();
	}

	menu_clickable::menu_clickable(widget& parent_widget)
		: widget(parent_widget)
	{
		this->menu = CreatePopupMenu();
		this->set_default_message_handlers();
	}

	menu_clickable::~menu_clickable()
	{
		if (this->menu)
			DestroyMenu(this->menu);
	}

	int menu_clickable::add_menu_item(std::string const& name, menu_event_function_t function)
	{
		this->menu_items[this->menu_index + 1] = menu_item(name, function);

		if (!AppendMenu(this->menu, MF_STRING | MF_OWNERDRAW, this->menu_index + 1, this->menu_items[this->menu_index + 1].caption().c_str()))
			return false;

		this->menu_index++;
		return true;
	}

	bool menu_clickable::remove_menu_item(int index)
	{
		if (!RemoveMenu(this->menu, index, MF_BYPOSITION))
			return false;
			
		this->menu_items.erase(index);
		this->menu_index--;
		return true;
	}
	
	void menu_clickable::show_menu(HWND hWnd, POINT pt)
	{
		int index = this->on_show_menu(pt);

		if (index >= 0)
		{	
			ClientToScreen(hWnd, &pt);
			std::size_t selected_menu = static_cast<std::size_t>(TrackPopupMenu(this->menu, TPM_RETURNCMD, pt.x, pt.y, 0, hWnd, NULL));
				
			if (selected_menu)
			{
				if (this->menu_items[selected_menu].function())
					this->menu_items[selected_menu].function()(index);
			}
		}
	}

	void menu_clickable::on_init_menu()
	{
		HWND menu_hwnd = FindWindow("#32768", 0);
		
		if (menu_hwnd)
		{
			SetWindowLong(menu_hwnd, GWLP_USERDATA, reinterpret_cast<LONG>(this));
			this->original_menu_window_proc = reinterpret_cast<WNDPROC>(SetWindowLong(menu_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG>(menu_clickable::default_menu_proc)));
		}
	}

	void menu_clickable::on_measure_item(HWND hWnd, MEASUREITEMSTRUCT* measure_item)
	{
		if (measure_item->CtlType == ODT_MENU)
		{
			HDC hdc = GetDC(hWnd);
			HGDIOBJ old_font = SelectObject(hdc, this->font());

			std::size_t max_length;

			if (StringCchLength(reinterpret_cast<char*>(measure_item->itemData), STRSAFE_MAX_CCH, &max_length) == S_OK)
			{
				SIZE string_size;

				if (GetTextExtentPoint32(hdc, reinterpret_cast<char*>(measure_item->itemData), max_length, &string_size))
				{
					measure_item->itemWidth = 28 + 1 + 6 + (string_size.cx + 40);
					measure_item->itemHeight = 24; // string_size.cy + 4;
				}
			}

			SelectObject(hdc, old_font);
			ReleaseDC(hWnd, hdc);
		}
	}

	void menu_clickable::on_draw_item(HWND hWnd, DRAWITEMSTRUCT* draw_item)
	{
		/* draw item background */
		//COLORREF background_color = this->background();

		//if (draw_item->itemState & ODS_SELECTED)
		//	background_color = RGB(51, 51, 51);
		
		COLORREF background_color = RGB(24, 24, 24);

		if (draw_item->itemState & ODS_SELECTED)
			background_color = this->background();

		this->menu_drawer.get()->draw_rectangle(&draw_item->rcItem, background_color);

		RECT rc_seperator(draw_item->rcItem);
		rc_seperator.left = 28;
		rc_seperator.right = rc_seperator.left + 1;

		this->menu_drawer.get()->draw_rectangle(&rc_seperator, RGB(70, 70, 70));
			
		/* draw item label */
		RECT rc_text(draw_item->rcItem);
		rc_text.left += 35;

		this->menu_drawer.get()->draw_text(reinterpret_cast<char*>(draw_item->itemData), &rc_text, RGB(255, 255, 255), DT_LEFT | DT_SINGLELINE | DT_VCENTER, this->font());
		this->menu_drawer.get()->commit_drawing(draw_item->hDC, &draw_item->rcItem);
	}
	
	void menu_clickable::set_default_message_handlers()
	{
		this->add_message_handler(WM_INITMENU, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			this->on_init_menu();
			return 0;
		});
	}
	
	void menu_clickable::on_mouse_right_down(HWND hWnd, unsigned int keys, POINT pt)
	{
		SetFocus(hWnd);
		SetCapture(hWnd);
	}

	void menu_clickable::on_mouse_right_up(HWND hWnd, unsigned int keys, POINT pt)
	{
		this->show_menu(hWnd, pt);
		ReleaseCapture();
	}
	
	LRESULT menu_clickable::menu_window_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_SIZE:
			if (wParam == SIZE_RESTORED)
				this->on_menu_size(hWnd, LOWORD(lParam), HIWORD(lParam));

			break;

		case WM_PRINT:
			{
				LRESULT result = CallWindowProc(this->original_menu_window_proc, hWnd, uMsg, wParam, lParam);
				this->draw_menu_border(hWnd, reinterpret_cast<HDC>(wParam));
				return result;
			}

		case WM_NCPAINT:
			{
				LRESULT result = CallWindowProc(this->original_menu_window_proc, hWnd, uMsg, wParam, lParam);
				HDC hdc = GetWindowDC(hWnd);

				this->draw_menu_border(hWnd, hdc);

				ReleaseDC(hWnd, hdc);
				return result;
			}

		case WM_ERASEBKGND:
			return TRUE;

		default:
			break;
		}

		return CallWindowProc(this->original_menu_window_proc, hWnd, uMsg, wParam, lParam);
	}
	
	void menu_clickable::on_menu_size(HWND hWnd, short width, short height)
	{
		RECT rc_client;
		GetClientRect(hWnd, &rc_client);

		if (width != 0 && height != 0)
		{
			int offset_x = (menu_border_thickness * 2);
			int offset_y = (menu_border_thickness * 2);

			this->menu_drawer = std::make_unique<image_drawer>(this->instance(), hWnd);
			this->menu_drawer.get()->reset_graphics(rectangle(rc_client.left, rc_client.top, width + offset_x, height + offset_y));
		}
	}
	
	void menu_clickable::draw_menu_border(HWND hWnd, HDC hdc)
	{
		RECT rc_border;
		GetClientRect(hWnd, &rc_border);
				
		rc_border.right += (menu_border_thickness * 2); 
		rc_border.bottom += (menu_border_thickness * 2);
			
		this->menu_drawer.get()->draw_hdc(hdc, &rc_border);
		this->menu_drawer.get()->draw_rectangle(&rc_border, NULL, true, this->background(), menu_border_thickness, reinterpret_cast<HBRUSH>(GetStockObject(NULL_BRUSH)));
		this->menu_drawer.get()->draw_rectangle(&rc_border, NULL, true, RGB(122, 156, 211), 1, reinterpret_cast<HBRUSH>(GetStockObject(NULL_BRUSH)));
		this->menu_drawer.get()->commit_drawing(hdc, &rc_border);
	}
}