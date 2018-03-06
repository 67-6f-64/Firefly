#include "list_view.hpp"

namespace firefly
{
	LRESULT CALLBACK list_view::default_menu_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		list_view* object = reinterpret_cast<list_view*>(GetWindowLong(hWnd, GWLP_USERDATA));

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

	list_view::list_view(window& parent_window)
		: list_view_base(parent_window)
	{
		this->set_default_message_handlers();
	}

	list_view::list_view(widget& parent_widget)
		: list_view_base(parent_widget)
	{
		this->set_default_message_handlers();
	}

	list_view::~list_view()
	{

	}

	int list_view::add_column(std::string const& caption, int width, bool center)
	{
		return this->header.get()->add_item(caption, width, center);
	}

	void list_view::remove_column(int index)
	{
		return this->header.get()->remove_item(index);
	}
	
	int list_view::add_menu_item(std::string const& menu_name, menu_event_function_t menu_function)
	{
		this->menu_items[this->menu_index + 1] = menu_item(menu_name, menu_function);

		if (!AppendMenu(this->menu, MF_STRING | MF_OWNERDRAW, this->menu_index + 1, this->menu_items[this->menu_index + 1].caption().c_str()))
			return false;

		this->menu_index++;
		return true;
	}

	bool list_view::remove_menu_item(int index)
	{
		if (!RemoveMenu(this->menu, index, MF_BYPOSITION))
			return false;
			
		this->menu_items.erase(index);
		this->menu_index--;
		return true;
	}

	void list_view::create(rectangle& rect, std::shared_ptr<tabpage> parent_tabpage)
	{
		this->parent_tabpage = parent_tabpage;

		this->list_view_base::create(rect);

		this->header = std::make_unique<list_view_header>(*this);
		this->header.get()->create(rectangle(0, 0, rect.width, LISTVIEW_COLUMN_HEIGHT));
		
		this->menu = CreatePopupMenu();
		return this->post_resize_list_view();
	}
	
	void list_view::resize_list_view(bool update_scrollbar)
	{
		this->pre_resize_list_view();
		this->post_resize_list_view(update_scrollbar);
	}

	int list_view::get_height()
	{
		return (this->rc_client.bottom - this->rc_client.top);
	}

	std::string list_view::get_item_data(int index, int subitem_index)
	{
		if (index >= static_cast<int>(this->items.size()))
			return std::string("");

		if (subitem_index >= static_cast<int>(this->items.at(index).subitems.size()))
			return std::string("");

		return this->items.at(index).subitems.at(subitem_index);
	}
		
	void list_view::set_item_data(int index, int subitem_index, std::string const& data)
	{
		if (index < static_cast<int>(this->items.size()) && subitem_index < static_cast<int>(this->items.at(index).subitems.size()))
			this->items.at(index).subitems.at(subitem_index) = data;
	}
	
	int list_view::get_item_integer(int index, int subitem_index)
	{
		try
		{
			return std::stoi(this->get_item_data(index, subitem_index), 0, 10);
		}
		catch (std::exception& e)
		{
#ifdef _DEBUG
			printf("exception in list_view::get_item_integer: %s\n", e.what());
#else
			UNREFERENCED_PARAMETER(e);
#endif
		}

		return -1;
	}
	
	RECT list_view::calculate_item_rect(int index)
	{
		RECT rc_item_rect;
		SetRectEmpty(&rc_item_rect);

		rc_item_rect.left = this->rc_client.left;
		rc_item_rect.top = this->rc_client.top + LISTVIEW_COLUMN_HEIGHT;
		rc_item_rect.right = this->rc_client.right;

		if (index >= 0)
		{
			rc_item_rect.top += (index * LISTVIEW_ITEM_HEIGHT);
			rc_item_rect.bottom = (rc_item_rect.top + LISTVIEW_ITEM_HEIGHT);
		}
		else
		{
			rc_item_rect.bottom = this->rc_client.bottom - LISTVIEW_COLUMN_HEIGHT;
		}

		return rc_item_rect;
	}
	
	void list_view::scroll_to_item(RECT* rc_screen_item)
	{
		this->parent_tabpage.get()->scroll_to_control(this->parent_tabpage.get()->handle(), rc_screen_item);
	}

	void list_view::pre_resize_list_view()
	{
		this->parent_tabpage.get()->reset_scrollbar_position();
	}

	void list_view::post_resize_list_view(bool update_scrollbar)
	{
		this->rc_client.bottom = LISTVIEW_COLUMN_HEIGHT + (this->items.size() * LISTVIEW_ITEM_HEIGHT);

		HDC hdc = GetDC(this->handle());

		int width = this->rc_client.right - this->rc_client.left;
		int height = this->rc_client.bottom - this->rc_client.top;

		this->resize(width, height);
		this->resize_graphics(hdc, width, height);

		ReleaseDC(this->handle(), hdc);

		if (update_scrollbar)
			this->parent_tabpage.get()->reset_scrollbar();
	}
	
	void list_view::set_default_message_handlers()
	{
		this->add_message_handler(WM_INITMENU, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			HWND menu_hwnd = FindWindow("#32768", 0);

			if (menu_hwnd)
			{
				SetWindowLong(menu_hwnd, GWLP_USERDATA, reinterpret_cast<LONG>(this));
				this->original_menu_window_proc = reinterpret_cast<WNDPROC>(SetWindowLong(menu_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG>(list_view::default_menu_proc)));
			}

			return 0;
		});

		this->add_message_handler(WM_MEASUREITEM, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			MEASUREITEMSTRUCT* measure_item = reinterpret_cast<MEASUREITEMSTRUCT*>(lParam);

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

			return TRUE;
		});
		
		this->add_message_handler(WM_DRAWITEM, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			DRAWITEMSTRUCT* owner_draw = reinterpret_cast<DRAWITEMSTRUCT*>(lParam);
			return (owner_draw->itemID >= 0 ? this->owner_draw_menu_item(owner_draw) : 0);
		});
	}
	
	void list_view::on_mouse_right_down(HWND hWnd, unsigned int keys, POINT pt)
	{
		SetFocus(hWnd);
		SetCapture(hWnd);
	}

	void list_view::on_mouse_right_up(HWND hWnd, unsigned int keys, POINT pt)
	{
		RECT rc_item;
		int index = this->find_item(pt, &rc_item);

		if (index >= 0)
		{
			this->set_selection(index, &rc_item);

			ClientToScreen(this->handle(), &pt);

			std::size_t selected_menu = static_cast<std::size_t>(TrackPopupMenu(this->menu, TPM_RETURNCMD, pt.x, pt.y, 0, this->handle(), NULL));
				
			if (selected_menu)
			{
				if (this->menu_items[selected_menu].function())
					this->menu_items[selected_menu].function()(index);
			}
		}
		else
			this->reset_selection_state();

		ReleaseCapture();
	}
	
	LRESULT list_view::menu_window_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		std::function<void(HWND, HDC)> draw_border = [this](HWND hWnd, HDC hdc)
		{
			RECT rc_border;
			GetClientRect(hWnd, &rc_border);
				
			rc_border.right += (menu_border_thickness * 2); 
			rc_border.bottom += (menu_border_thickness * 2);
			
			this->menu_drawer.get()->draw_hdc(hdc, &rc_border);
			this->menu_drawer.get()->draw_rectangle(&rc_border, NULL, true, this->background(), menu_border_thickness, reinterpret_cast<HBRUSH>(GetStockObject(NULL_BRUSH)));
			this->menu_drawer.get()->draw_rectangle(&rc_border, NULL, true, RGB(122, 156, 211), 1, reinterpret_cast<HBRUSH>(GetStockObject(NULL_BRUSH)));
			this->menu_drawer.get()->commit_drawing(hdc, &rc_border);
		};
		
		switch (uMsg)
		{
		case WM_SIZE:
			if (wParam == SIZE_RESTORED)
			{
				RECT rc_client;
				GetClientRect(hWnd, &rc_client);

				int width = LOWORD(lParam);
				int height = HIWORD(lParam);

				if (width != 0 && height != 0)
				{
					int offset_x = (menu_border_thickness * 2);
					int offset_y = (menu_border_thickness * 2);

					this->menu_drawer = std::make_unique<image_drawer>(this->instance(), hWnd);
					this->menu_drawer.get()->reset_graphics(rectangle(rc_client.left, rc_client.top, width + offset_x, height + offset_y));
				}
			}
			break;

		case WM_PRINT:
			{
				LRESULT result = CallWindowProc(this->original_menu_window_proc, hWnd, uMsg, wParam, lParam);
				draw_border(hWnd, reinterpret_cast<HDC>(wParam));
				return result;
			}

		case WM_NCPAINT:
			{
				LRESULT result = CallWindowProc(this->original_menu_window_proc, hWnd, uMsg, wParam, lParam);
				HDC hdc = GetWindowDC(hWnd);

				draw_border(hWnd, hdc);

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

	LRESULT list_view::owner_draw_menu_item(DRAWITEMSTRUCT* owner_draw)
	{
		/* draw item background */
		COLORREF background_color = this->background();

		if (owner_draw->itemState & ODS_SELECTED)
			background_color = RGB(51, 51, 51);

		this->menu_drawer.get()->draw_rectangle(&owner_draw->rcItem, background_color);

		RECT rc_seperator(owner_draw->rcItem);
		rc_seperator.left = 28;
		rc_seperator.right = rc_seperator.left + 1;

		this->menu_drawer.get()->draw_rectangle(&rc_seperator, RGB(70, 70, 70));
			
		/* draw item label */
		RECT rc_text(owner_draw->rcItem);
		rc_text.left += 35;

		this->menu_drawer.get()->draw_text(reinterpret_cast<char*>(owner_draw->itemData), &rc_text, RGB(255, 255, 255), DT_LEFT | DT_SINGLELINE | DT_VCENTER, this->font());
		this->menu_drawer.get()->commit_drawing(owner_draw->hDC, &owner_draw->rcItem);
		return TRUE;
	}

	void list_view::draw_item(list_view_item& item, int index, int flags, RECT* rc)
	{
		/* draw background */
		COLORREF background_color = this->background();
	
		if (flags & LVBASE_HOVERED)
			background_color = RGB(40, 40, 40);
		else if (flags & LVBASE_SELECTED)
			background_color = RGB(51, 51, 51);

		this->draw_rectangle(rc, background_color);
		
		/* draw seperator */
		RECT rc_seperator = { rc->left, rc->top, rc->right, rc->bottom };
		rc_seperator.top = rc_seperator.bottom - 1;

		this->draw_rectangle(&rc_seperator, RGB(40, 40, 40));		

		/* draw item labels */
		for (std::size_t i = 0, count = this->header.get()->get_item_count(); i < item.subitems.size() && i < count; i++)
		{
			list_view_header_item& column = this->header.get()->get_item_data(i);

			RECT rc_text = { rc->left, rc->top, rc->left + column.width, rc->bottom };
			InflateRect(&rc_text, -(LVIEW_INDENT / 2), 0);

			this->draw_text(item.subitems.at(i), &rc_text, RGB(255, 255, 255), (column.center ? DT_CENTER : DT_LEFT) | DT_MODIFYSTRING | DT_END_ELLIPSIS | DT_SINGLELINE | DT_VCENTER, this->font());
			rc->left += column.width;
		}
	}
}