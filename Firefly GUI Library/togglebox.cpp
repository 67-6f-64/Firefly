#include "togglebox.hpp"
#include "resource.hpp"

namespace firefly
{
	togglebox::togglebox(window& parent_window)
		: widget(parent_window)
	{
		this->function_on_state_change = nullptr;

		this->set_default_message_handlers();
	}

	togglebox::togglebox(widget& parent_widget)
		: widget(parent_widget)
	{
		this->function_on_state_change = nullptr;

		this->set_default_message_handlers();
	}

	togglebox::~togglebox()
	{

	}

	void togglebox::toggle()
	{
		this->set_check(this->get_check() ^ true);
	}

	void togglebox::set_check(bool checked)
	{
		Button_SetCheck(this->handle(), checked ? BST_CHECKED : BST_UNCHECKED);
		RedrawWindow(this->handle(), NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT);
	}

	bool togglebox::get_check()
	{
		return (Button_GetCheck(this->handle()) == BST_CHECKED);
	}
	
	std::string togglebox::get_text()
	{
		int text_length = Button_GetTextLength(this->handle());
				
		char* text_buffer = new char[text_length + 1];
		memset(text_buffer, 0, text_length + 1);
				
		text_length = Button_GetText(this->handle(), text_buffer, text_length + 1);

		if (text_length <= 0)
		{
			delete[] text_buffer;
			return std::string();
		}

		std::string text_string(text_buffer);

		delete[] text_buffer;
		return text_string;
	}

	void togglebox::set_event(togglebox_event event_type, togglebox_event_function_t event_function)
	{
		if (event_type == togglebox_event::on_state_change)
			this->function_on_state_change = event_function;
	}

	void togglebox::create(std::string const& caption, rectangle& rect, togglebox_type type)
	{
		unsigned int type_flag = 0;

		switch (type)
		{
		case togglebox_type::checkbox:
			type_flag = BS_AUTOCHECKBOX;
			break;

		case togglebox_type::radiobutton:
			type_flag = BS_AUTORADIOBUTTON;
			break;

		case togglebox_type::threestate:
			type_flag = BS_AUTO3STATE;
			break;

		default:
			break;
		}

		if (!this->build(WC_BUTTON, caption, rect, WS_VISIBLE | WS_CHILD | type_flag))
			throw std::exception("Failed to create 'checkbox' widget");

		this->type = type;
	}

	void togglebox::set_default_message_handlers()
	{
		this->add_message_handler(BM_SETCHECK, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			if (this->function_on_state_change)
				if (!this->function_on_state_change(wParam == BST_CHECKED))
					return 0;

			return this->original_proc(hWnd, BM_SETCHECK, wParam, lParam);
		});

		this->add_message_handler(CUSTOM_CTLCOLOR, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			SetDCBrushColor(reinterpret_cast<HDC>(wParam), RGB(255, 0, 0));
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

	LRESULT togglebox::try_custom_draw_item(NMCUSTOMDRAW* custom_draw)
	{
		const int box_size = 16;

		if (custom_draw->dwDrawStage != CDDS_PREPAINT)
			return CDRF_DODEFAULT;

		/* draw background */
		this->draw_rectangle(&custom_draw->rc, this->background());

		/* draw box */
		COLORREF background_color = 0;

		if (custom_draw->uItemState & CDIS_DISABLED)
			background_color = RGB(200, 200, 200);
		else if (custom_draw->uItemState & CDIS_SELECTED)
			background_color = RGB(0, 0, 0);
		else if (custom_draw->uItemState & CDIS_HOT)
			background_color = RGB(0, 50, 50);
		else
			background_color = RGB(76, 81, 87);

		int offset = ((custom_draw->rc.bottom - custom_draw->rc.top) - box_size) / 2;
		RECT rc_box = { 0, offset, box_size, offset + box_size };

		if (this->type == togglebox_type::checkbox || this->type == togglebox_type::threestate)
			this->draw_round_rectangle(&rc_box, size(2, 2), background_color, true, RGB(160, 160, 160));
		else if (this->type == togglebox_type::radiobutton)
			this->draw_ellipse(&rc_box, background_color, true, RGB(160, 160, 160));

		InflateRect(&rc_box, -2, -2);

		if (this->type == togglebox_type::checkbox || this->type == togglebox_type::threestate)
		{
			if (!(custom_draw->uItemState & CDIS_DISABLED))
			{
				int check_state = Button_GetCheck(custom_draw->hdr.hwndFrom);

				if (check_state == BST_CHECKED)
				{
					HBITMAP bitmap = LoadBitmap(this->instance(), MAKEINTRESOURCE(bmp_checkbox_check));
					this->draw_bitmap(bitmap, point(rc_box.left, rc_box.top));
					DeleteObject(bitmap);
				}
				else if (check_state == BST_INDETERMINATE)
				{
					/* todo: implement drawing for threestate */
				}
			}
		}
		else if (this->type == togglebox_type::radiobutton)
		{
			if (Button_GetCheck(custom_draw->hdr.hwndFrom) == BST_CHECKED)
			{
				InflateRect(&rc_box, -1, -1);
				this->draw_ellipse(&rc_box, RGB(206, 206, 206));
			}
		}

		/* draw label */
		RECT rc_text(custom_draw->rc);
		rc_text.left += (box_size + 5);

		int text_length = Button_GetTextLength(custom_draw->hdr.hwndFrom);

		char* text_buffer = new char[text_length + 1];
		memset(text_buffer, 0, text_length + 1);

		text_length = Button_GetText(custom_draw->hdr.hwndFrom, text_buffer, text_length + 1);

		this->draw_text(std::string(text_buffer), &rc_text, (custom_draw->uItemState & CDIS_DISABLED) ? RGB(175, 175, 175) : this->foreground(), DT_LEFT | DT_SINGLELINE | DT_VCENTER);

		delete[] text_buffer;

		this->commit_drawing(custom_draw->hdc, &custom_draw->rc);
		return CDRF_SKIPDEFAULT;
	}
}