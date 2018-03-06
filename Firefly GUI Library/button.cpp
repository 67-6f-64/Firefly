#include "button.hpp"

namespace firefly
{
	button::button(window& parent_window)
		: widget(parent_window)
	{
		this->image = NULL;
		this->function_on_click = nullptr;

		this->set_default_message_handlers();
	}
			
	button::button(widget& parent_widget)
		: widget(parent_widget)
	{
		this->image = NULL;
		this->function_on_click = nullptr;

		this->set_default_message_handlers();
	}

	button::~button()
	{
		this->replace_bitmap();
	}
		
	void button::set_event(button_event event_type, button_event_function_t event_function)
	{
		if (event_type == button_event::on_click)
			this->function_on_click = event_function;
	}

	void button::set_bitmap(unsigned short resource_id)
	{
		unsigned int style = GetWindowLong(this->handle(), GWL_STYLE);

		if (style)
		{
			if (style & BS_PUSHBUTTON)
				style &= ~BS_PUSHBUTTON;

			if (!(style & BS_BITMAP))
				style |= BS_BITMAP;

			SetWindowLong(this->handle(), GWL_STYLE, style);
			this->replace_bitmap(this->make_bitmap_from_png(this->instance(), resource_id));
		}
	}

	void button::set_text(std::string const& caption)
	{
		unsigned int style = GetWindowLong(this->handle(), GWL_STYLE);

		if (style)
		{
			if (style & BS_BITMAP)
			{
				style &= ~BS_BITMAP;
				this->replace_bitmap();
			}

			if (!(style & BS_PUSHBUTTON))
				style |= BS_PUSHBUTTON;

			SetWindowLong(this->handle(), GWL_STYLE, style);
			Button_SetText(this->handle(), caption.c_str());
		}
	}

	void button::create(std::string const& caption, rectangle& rect)
	{
		if (!this->build(WC_BUTTON, caption, rect, WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON))
			throw std::exception("Failed to create 'button' widget");

		this->set_foreground(RGB(255, 255, 255));
	}

	void button::replace_bitmap(HBITMAP bitmap)
	{
		HBITMAP old_bitmap = reinterpret_cast<HBITMAP>(SendMessage(this->handle(), BM_GETIMAGE, IMAGE_BITMAP, NULL));
	
		if (old_bitmap)
			DeleteObject(old_bitmap);

		SendMessage(this->handle(), BM_SETIMAGE, IMAGE_BITMAP, reinterpret_cast<LPARAM>(bitmap));
	}

	void button::set_default_message_handlers()
	{
		this->add_message_handler(WM_LBUTTONDBLCLK, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			return SendMessage(hWnd, WM_LBUTTONDOWN, wParam, lParam);
		});

		this->add_message_handler(CUSTOM_COMMAND, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			if (HIWORD(wParam) == BN_CLICKED && this->function_on_click)
				return static_cast<LRESULT>(this->function_on_click());

			return this->original_proc(hWnd, WM_COMMAND, wParam, lParam);
		});

		this->add_message_handler(CUSTOM_CTLCOLOR, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			SetDCBrushColor(reinterpret_cast<HDC>(wParam), this->background());
			return reinterpret_cast<LRESULT>(GetStockObject(DC_BRUSH));
		});

		this->add_message_handler(CUSTOM_NOTIFY, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			NMHDR* custom_notify = reinterpret_cast<NMHDR*>(lParam);

			if (custom_notify->code == NM_CUSTOMDRAW)
				return this->try_custom_draw_item(reinterpret_cast<NMCUSTOMDRAW*>(lParam));

			return 0;
		});
	}

	LRESULT button::try_custom_draw_item(NMCUSTOMDRAW* custom_draw)
	{
		if (custom_draw->dwDrawStage != CDDS_PREPAINT)
			return CDRF_DODEFAULT;

		unsigned int style = GetWindowLong(custom_draw->hdr.hwndFrom, GWL_STYLE);

		if (style & BS_BITMAP)
		{
			/* Draw background */
			this->draw_rectangle(&custom_draw->rc, this->background());

			/* Draw image */
			HBITMAP bitmap = reinterpret_cast<HBITMAP>(SendMessage(custom_draw->hdr.hwndFrom, BM_GETIMAGE, IMAGE_BITMAP, NULL));

			BITMAP bitmap_info;
			GetObject(bitmap, sizeof(bitmap_info), &bitmap_info);

			if (custom_draw->uItemState & CDIS_HOT)
			{
				this->draw_alpha_bitmap_recolored(bitmap, 30, point(
					((custom_draw->rc.right - custom_draw->rc.left) - bitmap_info.bmWidth) / 2,
					((custom_draw->rc.bottom - custom_draw->rc.top) - bitmap_info.bmHeight) / 2));
			}
			else
			{
				this->draw_alpha_bitmap(bitmap, point(
					((custom_draw->rc.right - custom_draw->rc.left) - bitmap_info.bmWidth) / 2,
					((custom_draw->rc.bottom - custom_draw->rc.top) - bitmap_info.bmHeight) / 2));
			}
		}
		else
		{
			/* Draw background */
			COLORREF background_color = 0;

			if (custom_draw->uItemState & CDIS_SELECTED)
				background_color = RGB(51, 51, 51);
			else if (custom_draw->uItemState & CDIS_HOT)
				background_color = RGB(64, 64, 64);
			else
				background_color = RGB(40, 40, 40);
			
			this->draw_rectangle(&custom_draw->rc, this->background());
			this->draw_round_rectangle(&custom_draw->rc, size(20, 20), background_color);

			/* Draw text label */
			int text_length = Button_GetTextLength(custom_draw->hdr.hwndFrom);

			char* text_buffer = new char[text_length + 1];
			text_length = Button_GetText(custom_draw->hdr.hwndFrom, text_buffer, text_length + 1);

			this->draw_text(std::string(text_buffer), &custom_draw->rc, ((custom_draw->uItemState & CDIS_SELECTED) ? RGB(165, 165, 165) : this->foreground()), DT_CENTER | DT_SINGLELINE | DT_VCENTER);

			delete[] text_buffer;
		}

		this->commit_drawing(custom_draw->hdc, &custom_draw->rc);
		return CDRF_SKIPDEFAULT;
	}
}