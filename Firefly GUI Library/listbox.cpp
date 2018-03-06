#include "listbox.hpp"

#include <chrono>
#include <iomanip>

namespace firefly
{
	namespace my_time
	{
		std::string get_time()
		{
			std::chrono::system_clock::time_point time_point_now = std::chrono::system_clock::now();
			std::time_t time_now = std::chrono::system_clock::to_time_t(time_point_now);
			std::put_time(std::localtime(&time_now), "%F %T");

			tm _time_now = *std::localtime(&time_now);

			char time_buffer[32];
			memset(time_buffer, 0, sizeof(time_buffer));
			strftime(time_buffer, sizeof(time_buffer), "%H:%M:%S", &_time_now);

			return std::string(time_buffer);
		}
	}

	listbox::listbox(window& parent_window)
		: widget(parent_window)
	{
		this->use_time_stamp = false;
		this->function_on_left_click = nullptr;
		this->function_on_left_double_click = nullptr;

		this->set_default_message_handlers();
	}

	listbox::listbox(widget& parent_widget)
		: widget(parent_widget)
	{
		this->use_time_stamp = false;
		this->function_on_left_click = nullptr;
		this->function_on_left_double_click = nullptr;

		this->set_default_message_handlers();
	}

	listbox::~listbox()
	{

	}

	void listbox::set_event(listbox_event event_type, listbox_event_function_t event_function)
	{
		if (event_type == listbox_event::on_left_click)
			this->function_on_left_click = event_function;
		else if (event_type == listbox_event::on_left_double_click)
			this->function_on_left_double_click = event_function;
	}

	int listbox::add_string(const char* format, ...)
	{
		va_list va;
		va_start(va, format);

		int index = this->add_string(RGB(0, 0, 0), format, va);

		va_end(va);
		return index;
	}

	int listbox::add_string(COLORREF color, const char* format, ...)
	{
		va_list va;
		va_start(va, format);

		int index = this->add_string(color, format, va);

		va_end(va);
		return index;
	}

	void listbox::clear()
	{
		ListBox_ResetContent(this->handle());
	}

	void listbox::create(rectangle& rect, bool use_time_stamp, bool no_select)
	{
		if (!this->build(WC_LISTBOX, "", rect, WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT | LBS_HASSTRINGS | (no_select ? LBS_NOSEL : 0) | LBS_OWNERDRAWFIXED))
			throw std::exception("Failed to create 'listbox' widget");

		this->set_background(RGB(255, 255, 255));
		this->set_foreground(RGB(0, 0, 0));

		this->use_time_stamp = use_time_stamp;
	}

	int listbox::add_string(COLORREF color, const char* format, va_list va)
	{
		char buffer[1024];
		vsprintf(buffer, format, va);

		int index = ListBox_AddString(this->handle(), this->use_time_stamp ? ("[" + my_time::get_time() + "]" + " " + buffer).c_str() : buffer);
		
		if (index != LB_ERR && index != LB_ERRSPACE)
		{
			ListBox_SetItemData(this->handle(), index, color);
			ListBox_SetTopIndex(this->handle(), index);
		}

		return index;
	}

	void listbox::set_default_message_handlers()
	{
		this->add_message_handler(WM_ERASEBKGND, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			RECT rc_main_body;
			GetClientRect(hWnd, &rc_main_body);

			this->draw_rectangle(&rc_main_body, RGB(255, 255, 255));
			this->commit_drawing(reinterpret_cast<HDC>(wParam), &rc_main_body);
			return 1;
		});

		this->add_message_handler(CUSTOM_CTLCOLOR, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			SetDCBrushColor(reinterpret_cast<HDC>(wParam), RGB(255, 255, 255));
			return reinterpret_cast<LRESULT>(GetStockObject(DC_BRUSH));
		});

		this->add_message_handler(CUSTOM_COMMAND, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			if (HIWORD(wParam) == LBN_SELCHANGE)
			{
				if (this->function_on_left_click)
					this->function_on_left_click(ListBox_GetCurSel(hWnd));
			}
			else if (HIWORD(wParam) == LBN_DBLCLK)
			{
				if (this->function_on_left_double_click)
					this->function_on_left_double_click(ListBox_GetCurSel(hWnd));
			}

			return this->original_proc(hWnd, CUSTOM_COMMAND, wParam, lParam);
		});

		this->add_message_handler(OWNER_MEASUREITEM, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			reinterpret_cast<MEASUREITEMSTRUCT*>(lParam)->itemHeight = 15;
			return 1;
		});

		this->add_message_handler(OWNER_DRAWITEM, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			return this->draw_item(reinterpret_cast<DRAWITEMSTRUCT*>(lParam));
		});
	}

	LRESULT listbox::draw_item(DRAWITEMSTRUCT* owner_draw)
	{
		/* draw the item background*/
		COLORREF background_color = 0;

		if (owner_draw->itemState & ODS_SELECTED)
			background_color = GetSysColor(COLOR_HIGHLIGHT);
		else
			background_color = this->background();
		
		this->draw_rectangle(&owner_draw->rcItem, background_color);

		/* draw the item text label */
		RECT rc_text(owner_draw->rcItem);
		rc_text.left += 2;

		COLORREF text_color = 0;

		if (owner_draw->itemState & ODS_SELECTED)
			text_color = SetTextColor(owner_draw->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
		else if (owner_draw->itemData)
			text_color = SetTextColor(owner_draw->hDC, owner_draw->itemData);
		else
			text_color = SetTextColor(owner_draw->hDC, this->foreground());
				
		int text_length = ListBox_GetTextLen(owner_draw->hwndItem, owner_draw->itemID);
				
		char* text_buffer = new char[text_length + 1];
		memset(text_buffer, 0, text_length + 1);
				
		text_length = ListBox_GetText(owner_draw->hwndItem, owner_draw->itemID, text_buffer);
           
		this->draw_text(std::string(text_buffer), &owner_draw->rcItem, text_color, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

		delete[] text_buffer;

		if (owner_draw->itemState & ODS_SELECTED)
			this->draw_focus_rect(&owner_draw->rcItem);

		this->commit_drawing(owner_draw->hDC, &owner_draw->rcItem);
		return 0;
	}
}