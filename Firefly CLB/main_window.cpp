#include "main_window.hpp"
#include "resource.hpp"

#include "instance_tabpage.hpp"

#include "main_tabpage.hpp"
#include "settings_tabpage.hpp"

namespace firefly
{
	main_window::main_window(HINSTANCE hinstance, std::string const& class_name, std::string const& window_name)
		: window(hinstance, class_name, window_name, MAKEINTRESOURCE(ico_firefly), rectangle(0, 0, window_width, window_height))
	{
		try
		{
			this->create_handlers();
			this->create_controls();
		}
		catch (std::exception& e)
		{
			printf("An exception occured in main_window::main_window - %s\n", e.what());
		}
	}

	main_window::~main_window()
	{

	}

	bool main_window::is_dialog_message(MSG* message)
	{
		if (LOWORD(message->message) == WM_KEYDOWN && message->wParam == VK_TAB)
		{			
			this->tabmenu.get()->enumerate_relatives([&](std::shared_ptr<widget> relative) -> bool
			{
				return (IsDialogMessage(relative.get()->handle(), message) != FALSE);
			});

			if (IsDialogMessage(this->handle(), message))
				return true;
		}

		return false;
	}
	
	void main_window::create_client_instance()
	{
		this->tabmenu_panel.get()->reset_scrollbar_position();

		this->create_tab_page<instance_tabpage>("Instance #1" + std::to_string(rand() % 255), png_button_instance, true, false, nullptr,
			[this]() -> void
		{
			this->tabmenu_panel.get()->reset_scrollbar();
		});
	}

	void main_window::create_handlers()
	{
		this->add_message_handler(WM_DESTROY, [](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			PostQuitMessage(0);
			return 0;
		});

		this->add_message_handler(WM_MOVE, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			RECT rc_window;
			GetClientRect(hWnd, &rc_window);
			
			RECT rc_desktop;
			GetWindowRect(GetDesktopWindow(), &rc_desktop);
			
			if (GET_X_LPARAM(lParam) + (rc_window.right - rc_window.left) > rc_desktop.right ||
				GET_Y_LPARAM(lParam) + (rc_window.bottom - rc_window.top) > rc_desktop.bottom)
			{
				this->tabmenu.get()->enumerate_relatives([&](std::shared_ptr<widget> relative) -> bool
				{
					std::shared_ptr<custom_tabpage> tabpage_relative = std::dynamic_pointer_cast<custom_tabpage>(relative);

					if (tabpage_relative)
					{
						if (IsWindowVisible(relative.get()->handle()))
							tabpage_relative.get()->reload_edit_controls();
					}

					return false;
				});
			}

			return 0;
		});

		this->add_message_handler(WM_MOUSEWHEEL, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			this->tabmenu.get()->on_mouse_wheel(GET_WHEEL_DELTA_WPARAM(wParam));
			return DefWindowProc(hWnd, WM_MOUSEWHEEL, wParam, lParam);
		});
	}

	void main_window::create_controls()
	{
		this->create_tab_control();
	}

	void main_window::create_tab_control()
	{
		int panel_width = 216 + SCROLLBAR_WIDTH;
		int panel_height = window_height - (dy + 1);

		this->tabmenu_panel = std::make_unique<tabpage>(*this, this->background());
		this->tabmenu_panel.get()->create(rectangle(1, dy, panel_width, panel_height), true);

		this->tabmenu = std::make_unique<tab_menu>(*this->tabmenu_panel.get());
		this->tabmenu.get()->create(rectangle(0, 0, 216, 0));
		this->tabmenu.get()->set_background_colors(RGB(24, 24, 24), this->background());
			
		this->tabmenu.get()->add_tab("INTERFACES", false);
		this->create_tab_page<main_tabpage>("Main", png_button_main);
		this->create_tab_page<settings_tabpage>("Settings", png_button_settings);

		this->tabmenu.get()->add_tab("INSTANCES", false, 0, true, true,
			[this]() -> bool 
		{
			this->create_client_instance();
			return true;
		});
	}
}