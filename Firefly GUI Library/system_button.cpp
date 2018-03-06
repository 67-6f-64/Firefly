#pragma once

#include "system_button.hpp"

namespace firefly
{
	system_button::system_button(window& parent_window)
		: button(parent_window)
	{
		this->set_default_message_handlers();
	}

	system_button::system_button(widget& parent_widget)
		: button(parent_widget)
	{
		this->set_default_message_handlers();
	}

	system_button::~system_button()
	{
		if (this->arrow_font)
			DeleteFont(this->arrow_font);
	}

	void system_button::create(system_button_type type, rectangle& rect)
	{
		button::create("", rect);

		this->type = type;

		if (type == system_button_type::scrollbar_up || type == system_button_type::scrollbar_down)
			this->arrow_font = this->create_font("Arial", 12, 6, FW_DONTCARE);
		else
			this->arrow_font = NULL;
	}
	
	void system_button::set_default_message_handlers()
	{
		this->add_message_handler(CUSTOM_NOTIFY, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			NMHDR* custom_notify = reinterpret_cast<NMHDR*>(lParam);

			if (custom_notify->code == NM_CUSTOMDRAW)
				return this->try_custom_draw_item(reinterpret_cast<NMCUSTOMDRAW*>(lParam));

			return 0;
		});
	}

	LRESULT system_button::try_custom_draw_item(NMCUSTOMDRAW* custom_draw)
	{
		if (custom_draw->dwDrawStage != CDDS_PREPAINT)
			return CDRF_DODEFAULT;

		/* Draw background */
		COLORREF background_color = 0;
		COLORREF foreground_color = 0;

		switch (this->type)
		{
		case system_button_type::minimize:
		case system_button_type::add:
		case system_button_type::remove:
			{
				background_color = this->background();

				if (custom_draw->uItemState & CDIS_SELECTED)
					foreground_color = RGB(99, 100, 102);
				else if (custom_draw->uItemState & CDIS_HOT)
					foreground_color = RGB(223, 225, 229);
				else
					foreground_color = RGB(136, 137, 140);
			}
			break;

		case system_button_type::close:
			{
				if (custom_draw->uItemState & CDIS_SELECTED)
					background_color = RGB(94, 25, 0);
				else if (custom_draw->uItemState & CDIS_HOT)
					background_color = RGB(189, 50, 0);
				else
					background_color = RGB(143, 38, 0);

				foreground_color = RGB(255, 255, 255);
			}
			break;

		case system_button_type::scrollbar_up:
		case system_button_type::scrollbar_down:
			{
				background_color = this->background();

				if (custom_draw->uItemState & CDIS_SELECTED)
					foreground_color = RGB(99, 100, 102);
				else if (custom_draw->uItemState & CDIS_HOT)
					foreground_color = RGB(223, 225, 229);
				else
					foreground_color = RGB(136, 137, 140);
			}
			break;

		default:
			break;
		}
		
		this->draw_rectangle(&custom_draw->rc, background_color);

		/* Draw text label */
		wchar_t* p = 0;

		switch (this->type)
		{
		case system_button_type::minimize:
			p = L"\u0336";
			break;

		case system_button_type::close:
		case system_button_type::remove:
			p = L"\u00D7";
			break;
			
		case system_button_type::scrollbar_up:
			p = L"\u25B2";
			break;

		case system_button_type::scrollbar_down:
			p = L"\u25BC";
			break;

		case system_button_type::add:
			p = L"\uFF0B";
			break;

		default:
			break;
		}

		if (this->arrow_font)
			this->draw_text_wide(p, &custom_draw->rc, foreground_color, DT_CENTER | DT_SINGLELINE | DT_VCENTER, this->arrow_font);
		else
			this->draw_text_wide(p, &custom_draw->rc, foreground_color, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

		this->commit_drawing(custom_draw->hdc, &custom_draw->rc);
		return CDRF_SKIPDEFAULT;
	}
}