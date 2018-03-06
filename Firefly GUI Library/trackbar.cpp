#include "trackbar.hpp"

namespace firefly
{
	trackbar::trackbar(window& parent_window)
		: widget(parent_window)
	{
		this->minimum = 0;
		this->maximum = 0;

		this->function_on_drag = nullptr;

		this->set_default_message_handlers();
	}

	trackbar::trackbar(widget& parent_widget)
		: widget(parent_widget)
	{
		this->minimum = 0;
		this->maximum = 0;

		this->function_on_drag = nullptr;

		this->set_default_message_handlers();
	}

	trackbar::~trackbar()
	{

	}

	void trackbar::set_event(trackbar_event event_type, trackbar_event_function_t event_function)
	{
		if (event_type == trackbar_event::on_drag)
			this->function_on_drag = event_function;
	}

	void trackbar::set_range(short minimum, short maximum)
	{
		this->minimum = minimum;
		this->maximum = maximum;

		SendMessage(this->handle(), TBM_SETRANGE, FALSE, MAKELPARAM(minimum, maximum));
	}

	int trackbar::get_position()
	{
		return static_cast<int>(SendMessage(this->handle(), TBM_GETPOS, 0, 0));
	}

	void trackbar::set_position(short position, bool cause_event)
	{
		SendMessage(this->handle(), TBM_SETPOS, TRUE, position);

		if (cause_event)
			if (this->function_on_drag)
				this->function_on_drag(position);
	}

	void trackbar::create(rectangle& rect, bool horizontal)
	{
		if (!this->build(TRACKBAR_CLASS, "", rect, WS_VISIBLE | WS_CHILD | (horizontal ? (TBS_HORZ | TBS_BOTTOM) : (TBS_VERT | TBS_LEFT))))
			throw std::exception("Failed to create 'button' widget");
	}

	void trackbar::set_default_message_handlers()
	{
		this->add_message_handler(WM_HSCROLL, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			short position = 0;

			if (LOWORD(wParam) == TB_THUMBPOSITION || LOWORD(wParam) == TB_THUMBTRACK)
				position = HIWORD(wParam);
			else
				position = static_cast<short>(SendMessage(hWnd, TBM_GETPOS, 0, 0));

			if (this->function_on_drag)
				this->function_on_drag(position);

			return 0;
		});

		this->add_message_handler(WM_VSCROLL, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			short position = 0;

			if (LOWORD(wParam) == TB_THUMBPOSITION || LOWORD(wParam) == TB_THUMBTRACK)
				position = HIWORD(wParam);
			else
				position = static_cast<short>(SendMessage(hWnd, TBM_GETPOS, 0, 0));

			if (this->function_on_drag)
				this->function_on_drag(position);

			return 0;
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

	LRESULT trackbar::try_custom_draw_item(NMCUSTOMDRAW* custom_draw)
	{
		switch (custom_draw->dwDrawStage)
		{
		case CDDS_PREPAINT:
			return CDRF_NOTIFYITEMDRAW;
			
		case CDDS_ITEMPREPAINT:
			{
				switch (custom_draw->dwItemSpec)
				{
				case TBCD_CHANNEL:
					return this->try_custom_draw_channel(custom_draw);

				case TBCD_THUMB:
					return this->try_custom_draw_thumb(custom_draw);

				case TBCD_TICS:
					return this->try_custom_draw_ticks(custom_draw);
					
				default:
					break;
				}

				return CDRF_SKIPDEFAULT;
			}

		default:
			break;
		}

		return CDRF_DODEFAULT;
	}

	LRESULT trackbar::try_custom_draw_channel(NMCUSTOMDRAW* custom_draw)
	{
		if (IsRectEmpty(&this->rc_channel))
			CopyRect(&this->rc_channel, &custom_draw->rc);
	
		RECT rc_thumb;
		SendMessage(custom_draw->hdr.hwndFrom, TBM_GETTHUMBRECT, 0, reinterpret_cast<LPARAM>(&rc_thumb));

		RECT rc(this->rc_channel);
		rc.right = rc_thumb.left + ((rc_thumb.right - rc_thumb.left) / 2);
		this->draw_rectangle(&rc, RGB(30, 215, 96));

		rc.left = rc.right;
		rc.right = this->rc_channel.right;
		this->draw_rectangle(&rc, RGB(51, 51, 51));

		this->commit_drawing(custom_draw->hdc, &this->rc_channel);
		return CDRF_SKIPDEFAULT;
	}

	LRESULT trackbar::try_custom_draw_thumb(NMCUSTOMDRAW* custom_draw)
	{
		if (!IsRectEmpty(&this->rc_channel))
		{
			RedrawWindow(custom_draw->hdr.hwndFrom, &this->rc_channel, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT);
		
			int dy = this->rc_channel.top - custom_draw->rc.top;

			RECT rc_thumb(custom_draw->rc);
			rc_thumb.bottom = rc_thumb.top + (dy * 2) + (this->rc_channel.bottom - this->rc_channel.top);

			this->draw_rectangle(&rc_thumb, RGB(64, 64, 64));
			this->commit_drawing(custom_draw->hdc, &rc_thumb);
		}

		return CDRF_SKIPDEFAULT;
	}

	LRESULT trackbar::try_custom_draw_ticks(NMCUSTOMDRAW* custom_draw)
	{
		return CDRF_SKIPDEFAULT;
	}
}