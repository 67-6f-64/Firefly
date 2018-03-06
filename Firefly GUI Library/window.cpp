#include "window.hpp"
#include "system_button.hpp"

#include "resource.hpp"

namespace firefly
{
	window::window(HINSTANCE hinstance, std::string const& class_name, std::string const& window_name, char* icon_resource, rectangle& rect, bool show_in_taskbar, bool can_minimize, HWND hwnd_parent)
		: message_controller(hinstance, NULL, true)
	{
		this->set_font(this->create_font("Arial", 17));
		this->set_foreground(RGB(160, 160, 160));
		this->set_background(RGB(40, 40, 40));

		this->background_brush = CreateSolidBrush(this->background());

		if (!this->register_class(class_name, icon_resource))
			throw std::exception("failed to register window class!");

		if (!this->create_window(class_name, window_name, rect, show_in_taskbar, hwnd_parent))
			throw std::exception("failed to create window!");
		
		this->reset_graphics(rect);

		this->set_default_message_handlers();
		this->create_controls(rect, can_minimize);
	}
	
	window::~window()
	{
		if (this->background_brush)
			DeleteObject(this->background_brush);
	}
	
	void window::start(bool animate)
	{
		if (animate)
			AnimateWindow(this->handle(), 200, AW_BLEND | AW_ACTIVATE);
		else
			ShowWindow(this->handle(), SW_SHOW);

		UpdateWindow(this->handle());
	}
	
	bool window::register_class(std::string const& class_name, char* icon_resource)
	{
		WNDCLASSEX wcex;
		memset(&wcex, 0, sizeof(WNDCLASSEX));

		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.lpfnWndProc = message_controller::default_message_proc;
		wcex.hInstance = this->instance();
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hIcon = LoadIcon(this->instance(), icon_resource);
		wcex.hIconSm = LoadIcon(this->instance(), icon_resource);
		wcex.hbrBackground = this->background_brush;
		wcex.lpszClassName = class_name.c_str();

		return (RegisterClassEx(&wcex) != NULL);
	}

	bool window::create_window(std::string const& class_name, std::string const& window_name, rectangle& rect, bool show_in_taskbar, HWND hwnd_parent)
	{
		//unsigned int style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		unsigned int style = WS_POPUP;

		RECT rc_window = { rect.x, rect.y, rect.width, rect.height };
		AdjustWindowRect(&rc_window, style, FALSE);
			
		rect.width = rc_window.right - rc_window.left;
		rect.height = rc_window.bottom - rc_window.top;

		if (!rect.x && !rect.y)
		{
			RECT rc_system;
			SystemParametersInfo(SPI_GETWORKAREA, 0, &rc_system, 0);

			rect.x = (rc_system.right / 2) - (rect.width / 2);
			rect.y = (rc_system.bottom / 2) - (rect.height / 2);
		}
		
		this->set_handle(CreateWindowEx((show_in_taskbar ? 0 : WS_EX_TOOLWINDOW) | WS_EX_CONTROLPARENT, class_name.c_str(), window_name.c_str(), style, rect.x, rect.y,
			rect.width, rect.height, hwnd_parent, NULL, this->instance(), dynamic_cast<message_controller*>(this)));

		return (this->handle() != NULL);
	}
	
	void window::set_default_message_handlers()
	{
		this->add_message_handler(WM_ERASEBKGND, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			RECT rc;
			GetClientRect(hWnd, &rc);
			
			this->draw_rectangle(&rc, RGB(24, 24, 24));

			/* draw header */
			RECT rc_header(rc);
			InflateRect(&rc_header, -1, -1);

			rc_header.bottom = 36;

			this->draw_rectangle(&rc_header, this->background());

			/* draw header icon */
			this->draw_icon(hWnd, rectangle(8, 8, 20, 20));

			/* draw header label */
			rc_header.left += 36;

			int text_length = GetWindowTextLength(hWnd);

			char* text_buffer = new char[text_length + 1];
			memset(text_buffer, 0, text_length + 1);

			text_length = GetWindowText(hWnd, text_buffer, text_length + 1);
			
			this->draw_text(std::string(text_buffer), &rc_header, this->foreground(), DT_LEFT | DT_SINGLELINE | DT_VCENTER, this->font());

			delete[] text_buffer;

			/* draw seperator */
			RECT rc_seperator(rc);
			InflateRect(&rc_seperator, -1, -1);

			rc_seperator.top = rc_header.bottom;
			rc_seperator.bottom = rc_header.bottom + 1;

			this->draw_rectangle(&rc_seperator, RGB(24, 24, 24));
			
			/* draw body */
			RECT rc_body(rc);
			InflateRect(&rc_body, -1, -1);

			rc_body.top = rc_header.bottom + 1;

			this->draw_rectangle(&rc_body, RGB(24, 24, 24));

			this->commit_drawing(reinterpret_cast<HDC>(wParam), &rc);
			return 0;
		});

		this->add_message_handler(WM_NCHITTEST, [](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			POINT pt = { LOWORD(lParam), HIWORD(lParam) };
			ScreenToClient(hWnd, &pt);

			if (pt.y <= 36)
				return HTCAPTION;

			return DefWindowProc(hWnd, WM_NCHITTEST, wParam, lParam);
		});

		this->add_message_handler(WM_CLOSE, [](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			DestroyWindow(hWnd);
			return 0;
		});

		this->add_message_handler(WM_MEASUREITEM, [](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			return SendMessage(GetDlgItem(hWnd, wParam), OWNER_MEASUREITEM, wParam, lParam);
		});
		
		this->add_message_handler(WM_DRAWITEM, [](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			return SendMessage(reinterpret_cast<DRAWITEMSTRUCT*>(lParam)->hwndItem, OWNER_DRAWITEM, wParam, lParam);
		});

		this->add_message_handler(WM_NOTIFY, [](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			return SendMessage(reinterpret_cast<NMHDR*>(lParam)->hwndFrom, CUSTOM_NOTIFY, wParam, lParam);
		});
		
		this->add_message_handler(WM_COMMAND, [](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			return SendMessage(reinterpret_cast<HWND>(lParam), CUSTOM_COMMAND, wParam, lParam);
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
	}

	void window::create_controls(rectangle& rect, bool can_minimize)
	{
		this->close_button = std::make_unique<system_button>(*this);
		this->close_button.get()->create(system_button_type::close, rectangle(rect.width - 6 - 43, 6, 43, 20));
		this->close_button.get()->set_event(button_event::on_click, [this]() -> bool
		{
			PostMessage(this->handle(), WM_SYSCOMMAND, SC_CLOSE, 0);
			return true;
		});

		if (can_minimize)
		{
			this->minimize_button = std::make_unique<system_button>(*this);
			this->minimize_button.get()->create(system_button_type::minimize, rectangle(rect.width - 6 - 43 - 6 - 43, 6, 43, 20));
			this->minimize_button.get()->set_event(button_event::on_click, [this]() -> bool
			{
				PostMessage(this->handle(), WM_SYSCOMMAND, SC_MINIMIZE, 0);
				return true;
			});
		}
	}
}