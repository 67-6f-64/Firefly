#include "panel.hpp"

namespace firefly
{
	panel::panel(window& parent_window)
		: widget(parent_window)
	{
		this->set_default_message_handlers();
	}
			
	panel::panel(widget& parent_widget)
		: widget(parent_widget)
	{
		this->set_default_message_handlers();
	}

	panel::~panel()
	{

	}

	void panel::create(rectangle& rect, bool has_background, bool show)
	{
		this->has_background = has_background;

		if (!this->build(WC_STATIC, "", rect, (show ? WS_VISIBLE : NULL) | WS_CHILD | SS_OWNERDRAW | SS_NOTIFY))
			throw std::exception("Failed to create 'panel' widget");
	}
	
	void panel::on_mouse_left_down(HWND hWnd, unsigned int keys, POINT pt)
	{
		SetFocus(hWnd);
	}

	void panel::set_default_message_handlers()
	{
		this->add_message_handler(WM_ERASEBKGND, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			if (this->has_background)
			{
				RECT rc;
				GetClientRect(this->handle(), &rc);

				this->draw_rectangle(&rc, this->background());
				this->commit_drawing(reinterpret_cast<HDC>(wParam), &rc);
			}

			return TRUE;
		});
		
		this->add_message_handler(WM_MEASUREITEM, [](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			return SendMessage(GetDlgItem(hWnd, wParam), OWNER_MEASUREITEM, wParam, lParam);
		});
		
		this->add_message_handler(WM_DRAWITEM, [](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			return SendMessage(reinterpret_cast<DRAWITEMSTRUCT*>(lParam)->hwndItem, OWNER_DRAWITEM, wParam, lParam);
		});

		this->add_message_handler(WM_NOTIFY, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			return SendMessage(reinterpret_cast<NMHDR*>(lParam)->hwndFrom, CUSTOM_NOTIFY, wParam, lParam);
		});

		this->add_message_handler(WM_COMMAND, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			return SendMessage(reinterpret_cast<HWND>(lParam), CUSTOM_COMMAND, wParam, lParam);
		});
		
		this->add_message_handler(WM_HSCROLL, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			SendMessage(reinterpret_cast<HWND>(lParam), WM_HSCROLL, wParam, lParam);
			return DefWindowProc(hWnd, WM_HSCROLL, wParam, lParam);
		});

		this->add_message_handler(WM_VSCROLL, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			SendMessage(reinterpret_cast<HWND>(lParam), WM_VSCROLL, wParam, lParam);
			return DefWindowProc(hWnd, WM_VSCROLL, wParam, lParam);
		});

		this->add_message_handler(WM_CTLCOLORBTN, [](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			return SendMessage(reinterpret_cast<HWND>(lParam), CUSTOM_CTLCOLOR, wParam, lParam);
		});

		this->add_message_handler(WM_CTLCOLOREDIT, [](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			return SendMessage(reinterpret_cast<HWND>(lParam), CUSTOM_CTLCOLOR, wParam, lParam);
		});

		this->add_message_handler(WM_CTLCOLORLISTBOX, [](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			return SendMessage(reinterpret_cast<HWND>(lParam), CUSTOM_CTLCOLOR, wParam, lParam);
		});

		this->add_message_handler(WM_CTLCOLORSTATIC, [](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			return SendMessage(reinterpret_cast<HWND>(lParam), CUSTOM_CTLCOLOR, wParam, lParam);
		});

		this->add_message_handler(OWNER_DRAWITEM, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			return this->draw_item(hWnd, reinterpret_cast<DRAWITEMSTRUCT*>(lParam));
		});
	}
		
	LRESULT panel::draw_item(HWND hwnd, DRAWITEMSTRUCT* panel_draw)
	{
		return 1;
	}
}