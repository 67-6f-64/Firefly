#include "tooltip.hpp"
#include "widget.hpp"

namespace firefly
{
	void tooltip::init(base_window& base)
	{
		SetRectEmpty(&this->rc_client);
		this->bound_handle = NULL;

		this->set_font(this->create_font("Arial", 17));
		this->set_foreground(base.foreground());
		this->set_background(base.background());
	}

	tooltip::tooltip(window& parent_window)
		: message_controller(parent_window.instance(), parent_window.handle(), false)
	{
		this->init(parent_window);
		this->set_default_tooltip_handlers();
	}

	tooltip::tooltip(widget& parent_widget)
		: message_controller(parent_widget.instance(), parent_widget.handle(), false)
	{
		this->init(parent_widget);
		this->set_default_tooltip_handlers();
	}

	tooltip::~tooltip()
	{

	}
	
	bool tooltip::create()
	{
		this->set_handle(CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP | TTS_BALLOON, 
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, this->parent_handle(), NULL, this->instance(), NULL));

		if (!this->handle())
			return false;

		this->reset_font();
		
		return this->initialize_widget(this->handle());
	}

	bool tooltip::bind_control(HWND handle, std::string const& caption)
	{
		this->bound_handle = handle;

		TOOLINFO tool_info;
		memset(&tool_info, 0, sizeof(TOOLINFO));

		tool_info.cbSize = sizeof(TOOLINFO);
		tool_info.hwnd = this->parent_handle();
		tool_info.uFlags = TTF_IDISHWND | TTF_SUBCLASS | TTF_TRANSPARENT; // TTF_CENTERTIP
		tool_info.uId = reinterpret_cast<UINT_PTR>(this->bound_handle);
		tool_info.lpszText = const_cast<LPSTR>(caption.c_str());

		return (SendMessage(this->handle(), TTM_ADDTOOL, 0, reinterpret_cast<LPARAM>(&tool_info)) != FALSE) &&
			(SendMessage(this->handle(), TTM_SETMAXTIPWIDTH, 0, SHORT_MAX) != FALSE);
	}

	void tooltip::reset_font()
	{
		SendMessage(this->handle(), WM_SETFONT, reinterpret_cast<WPARAM>(this->font()), TRUE);
	}
	
	std::string tooltip::get_text()
	{
		char text_buffer[2048];
		memset(text_buffer, 0, sizeof(text_buffer));

		TOOLINFO tool_info;
		memset(&tool_info, 0, sizeof(TOOLINFO));

		tool_info.cbSize = sizeof(TOOLINFO);
		tool_info.hwnd = this->parent_handle();
		tool_info.uId = reinterpret_cast<UINT_PTR>(this->bound_handle);
		tool_info.lpszText = text_buffer;

		SendMessage(this->handle(), TTM_GETTEXT, sizeof(text_buffer), reinterpret_cast<LPARAM>(&tool_info));

		return std::string(text_buffer);
	}

	void tooltip::set_default_tooltip_handlers()
	{
		this->add_message_handler(CUSTOM_NOTIFY, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			NMHDR* custom_notify = reinterpret_cast<NMHDR*>(lParam);

			switch (custom_notify->code)
			{
			case TTN_SHOW:
				return this->on_show_tooltip(custom_notify);

			case NM_CUSTOMDRAW:
				return this->try_custom_draw(reinterpret_cast<NMTTCUSTOMDRAW*>(lParam));

			default:
				break;
			}

			return 0;
		});
	}
	
	LRESULT tooltip::on_show_tooltip(NMHDR* custom_notify)
	{
		GetClientRect(custom_notify->hwndFrom, &this->rc_client);
		return 0;
	}

	LRESULT tooltip::try_custom_draw(NMTTCUSTOMDRAW* custom_draw)
	{
		if (custom_draw->nmcd.dwDrawStage == CDDS_PREPAINT)
		{
			if (!IsRectEmpty(&this->rc_client))
			{
				this->reset_graphics(rectangle(this->rc_client.left, this->rc_client.top, this->rc_client.right - this->rc_client.left, this->rc_client.bottom - this->rc_client.top));
				this->draw_rectangle(&this->rc_client, RGB(64, 64, 64));

				custom_draw->nmcd.rc.top -= 2;
				this->draw_text(this->get_text(), &custom_draw->nmcd.rc, this->foreground(), DT_CENTER);

				this->commit_drawing(custom_draw->nmcd.hdc, &this->rc_client);
				return CDRF_SKIPDEFAULT;
			}
		}

		return CDRF_DODEFAULT;
	}
}