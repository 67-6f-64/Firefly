#include "main_window.hpp"
#include "resource.hpp"

#include "autobotting_tabpage.hpp"
#include "autologin_tabpage.hpp"
#include "autoshop_tabpage.hpp"
#include "exclusive_tabpage.hpp"
#include "hacks_tabpage.hpp"
#include "itemfilter_tabpage.hpp"
#include "kami_tabpage.hpp"
#include "spawncontrol_tabpage.hpp"

#include "rusher_tabpage.hpp"
#include "scripting_tabpage.hpp"
#include "settings_tabpage.hpp"

#define window_width 839
#define window_height 750

#define tabpage_position 217

#ifdef _PRODUCTION
#include <VMProtectSDK.h>
#endif

namespace firefly
{
	main_window::main_window(HINSTANCE hinstance, std::string const& class_name, std::string const& window_name)
		: window(hinstance, class_name, window_name, MAKEINTRESOURCE(ico_firefly), rectangle(0, 0, window_width, window_height)), items(), maps()
	{
#ifndef _PRODUCTION
		this->is_logged_in = true;
#endif

		try
		{
			this->create_handlers();
			this->create_controls();
			//this->start();
		}
		catch (std::exception& e)
		{
#ifdef _DEBUG
			printf("An exception occured in main_window::main_window - %s\n", e.what());
#else
			UNREFERENCED_PARAMETER(e);
#endif
		}
	}

	main_window::~main_window()
	{

	}

	bool main_window::is_dialog_message(MSG* message)
	{
		if (LOWORD(message->message) == WM_KEYDOWN && message->wParam == VK_TAB)
		{			
			for (auto x : this->tabpage_container)
				if (IsDialogMessage(x.get()->handle(), message))
					return true;

			if (IsDialogMessage(this->handle(), message))
				return true;
		}

		return false;
	}
	
	void main_window::save_profile(std::string const& profile_path)
	{
		for (std::shared_ptr<custom_tabpage> tab : this->tabpage_container)
			tab.get()->save_to_profile(profile_path);
	}

	void main_window::load_profile(std::string const& profile_path)
	{
		for (std::shared_ptr<custom_tabpage> tab : this->tabpage_container)
			tab.get()->load_from_profile(profile_path);
	}
	
	void main_window::initialize()
	{
		dynamic_cast<settings_tabpage*>(this->tabpage_container.at(tabpage_ids::settings).get())->initialize_profile();
	}
	
#ifdef _PRODUCTION
#pragma optimize("g", off)
#endif
	bool main_window::try_authenticate(std::string const& email, std::string const& password)
	{
#ifdef _PRODUCTION
		VMProtectBeginVirtualization("main_window::try_authenticate");
#endif
		if (!email.empty() && !password.empty())
		{
			static unsigned int last_tick = 0;

			if (GetTickCount() - last_tick >= 5000)
			{
				last_tick = GetTickCount();
				dynamic_cast<settings_tabpage*>(this->tabpage_container.at(tabpage_ids::settings).get())->on_processing_authentication();

				this->is_logged_in = this->authenticate(email, password);

				if (this->is_logged_in)
					dynamic_cast<settings_tabpage*>(this->tabpage_container.at(tabpage_ids::settings).get())->on_successful_authentication();
				else
					dynamic_cast<settings_tabpage*>(this->tabpage_container.at(tabpage_ids::settings).get())->on_reset_authentication();
			}
			else
			{
				MessageBox(this->handle(), "You must wait atleast 5 seconds before re-authenticating.", "Connection Error", MB_OK | MB_ICONERROR | MB_TOPMOST | MB_SETFOREGROUND);
			}
		}

		return true;
#ifdef _PRODUCTION
		VMProtectEnd();
#endif
	}
	
	bool main_window::try_launch_game(std::string const& game_path, std::string const& username, std::string const& password)
	{
#ifdef _PRODUCTION
		VMProtectBeginVirtualization("main_window::try_launch_game");
#endif
		if (!this->is_logged_in)
		{
			MessageBox(this->handle(), "You must be logged in to use the trainer.", "Authority Error", MB_OK | MB_ICONERROR | MB_TOPMOST | MB_SETFOREGROUND);
			return false;
		}
		
		if (game_path.empty())
		{
			MessageBox(this->handle(), "You must set the MapleStory path.", "Launch Error", MB_OK | MB_ICONERROR | MB_TOPMOST | MB_SETFOREGROUND);
			return false;
		}

		if (username.empty() || password.empty())
		{
			MessageBox(this->handle(), "You must enter a username and password to launch MapleStory.", "Launch Error", MB_OK | MB_ICONERROR | MB_TOPMOST | MB_SETFOREGROUND);
			return false;
		}

		return this->launch_game(game_path, username, password);
#ifdef _PRODUCTION
		VMProtectEnd();
#endif
	}
#ifdef _PRODUCTION
#pragma optimize("g", on)
#endif

	void main_window::create_handlers()
	{
#ifdef _PRODUCTION
		this->add_message_handler(WM_CLOSE, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			if (MessageBox(hWnd, "Closing Firefly Trainer will also close the related MapleStory instance.\nAre you sure you want to close?", 
				"Closure", MB_ICONQUESTION | MB_YESNO | MB_TOPMOST | MB_SETFOREGROUND) == IDYES)
			{
				DestroyWindow(hWnd);
			}

			return 0;
		});
#endif
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
				for (std::shared_ptr<custom_tabpage>& tab_page : this->tabpage_container)
				{
					if (IsWindowVisible(tab_page.get()->handle()))
						tab_page.get()->reload_edit_controls();
				}
			}

			return 0;
		});

		this->add_message_handler(WM_MOUSEWHEEL, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			this->tabmenu.get()->on_mouse_wheel(GET_WHEEL_DELTA_WPARAM(wParam));
			return DefWindowProc(hWnd, WM_MOUSEWHEEL, wParam, lParam);
		});

		this->add_message_handler(WM_GAME_SOCKET, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			this->handle_select_message(static_cast<SOCKET>(wParam), WSAGETSELECTEVENT(lParam), WSAGETSELECTERROR(lParam), true);
			return 0;
		});

		this->add_message_handler(WM_MANAGER_SOCKET, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			this->handle_select_message(static_cast<SOCKET>(wParam), WSAGETSELECTEVENT(lParam), WSAGETSELECTERROR(lParam), false);
			return 0;
		});
	}

	void main_window::create_controls()
	{
		this->create_tab_control(37);
	}

	void main_window::create_tab_control(int y)
	{
		this->tabmenu = std::make_unique<tab_menu>(*this);
		this->tabmenu.get()->create(rectangle(1, y, 216, window_height - y));
			
		this->tabmenu.get()->add_tab("AUTOMATICS", false);
		this->create_autobotting_tab(y);
		this->create_autoshop_tab(y);
		this->create_autologin_tab(y);
		
		this->tabmenu.get()->add_tab("ACTIVATE", false);
		this->create_hacks_tab(y);
		this->create_kami_tab(y);
		this->create_itemfilter_tab(y);
		this->create_spawncontrol_tab(y);
		this->create_exclusive_tab(y);

		this->tabmenu.get()->add_tab("MAIN", false);
		this->create_rusher_tab(y);
		this->create_scripting_tab(y);
		this->create_settings_tab(y);
	}
	
	void main_window::create_autobotting_tab(int y)
	{
		int autobotting_tab_index = this->tabmenu.get()->add_tab("Auto Botting", true, png_button_autobotting, true,
			[this](bool checked) -> bool 
		{
			std::shared_ptr<custom_tabpage> tab_page = this->tabpage_container.at(tabpage_ids::autobotting);
			return (tab_page.get()->set_toggle(checked, this->is_logged_in), tab_page.get()->toggle(checked, this->is_logged_in)); 
		});
		
		std::shared_ptr<autobotting_tabpage> autobotting_tab_page = std::make_shared<autobotting_tabpage>(this);
		autobotting_tab_page.get()->create(autobotting_tab_page, rectangle(tabpage_position, y, window_width - tabpage_position - 1, window_height - y - 1));

		this->tabmenu.get()->relate(autobotting_tab_index, autobotting_tab_page);
		this->tabpage_container.push_back(autobotting_tab_page);
	}
	
	void main_window::create_autoshop_tab(int y)
	{
		int autoshop_tab_index = this->tabmenu.get()->add_tab("Auto Shop", true, png_button_autoshop, true,
			[this](bool checked) -> bool 
		{
			return false;
			std::shared_ptr<custom_tabpage> tab_page = this->tabpage_container.at(tabpage_ids::autoshop);
			return (tab_page.get()->set_toggle(checked, this->is_logged_in), tab_page.get()->toggle(checked, this->is_logged_in)); 
		});
		
		std::shared_ptr<autoshop_tabpage> autoshop_tab_page = std::make_shared<autoshop_tabpage>(this);
		autoshop_tab_page.get()->create(autoshop_tab_page, rectangle(tabpage_position, y, window_width - tabpage_position - 1, window_height - y - 1));
		autoshop_tab_page.get()->enable(false);

		this->tabmenu.get()->relate(autoshop_tab_index, autoshop_tab_page);
		this->tabpage_container.push_back(autoshop_tab_page);
	}

	void main_window::create_autologin_tab(int y)
	{
		int autologin_tab_index = this->tabmenu.get()->add_tab("Auto Login", true, png_button_autologin, true,
			[this](bool checked) -> bool 
		{
			std::shared_ptr<custom_tabpage> tab_page = this->tabpage_container.at(tabpage_ids::autologin);
			return (tab_page.get()->set_toggle(checked, this->is_logged_in), tab_page.get()->toggle(checked, this->is_logged_in)); 
		});
		
		std::shared_ptr<autologin_tabpage> autologin_tab_page = std::make_shared<autologin_tabpage>(this);
		autologin_tab_page.get()->create(autologin_tab_page, rectangle(tabpage_position, y, window_width - tabpage_position - 1, window_height - y - 1));

		this->tabmenu.get()->relate(autologin_tab_index, autologin_tab_page);
		this->tabpage_container.push_back(autologin_tab_page);
	}
	
	void main_window::create_hacks_tab(int y)
	{
		int hacks_tab_index = this->tabmenu.get()->add_tab("Hacks", true, png_button_hacks, true,
			[this](bool checked) -> bool 
		{
			std::shared_ptr<custom_tabpage> tab_page = this->tabpage_container.at(tabpage_ids::hacks);
			return (tab_page.get()->set_toggle(checked, this->is_logged_in), tab_page.get()->toggle(checked, this->is_logged_in)); 
		});
		
		std::shared_ptr<hacks_tabpage> hacks_tab_page = std::make_shared<hacks_tabpage>(this);
		hacks_tab_page.get()->create(hacks_tab_page, rectangle(tabpage_position, y, window_width - tabpage_position - 1, window_height - y - 1));

		this->tabmenu.get()->relate(hacks_tab_index, hacks_tab_page);
		this->tabpage_container.push_back(hacks_tab_page);
	}

	void main_window::create_kami_tab(int y)
	{
		int kami_tab_index = this->tabmenu.get()->add_tab("Kami", true, png_button_kami, true,
			[this](bool checked) -> bool 
		{
			std::shared_ptr<custom_tabpage> tab_page = this->tabpage_container.at(tabpage_ids::kami);
			return (tab_page.get()->set_toggle(checked, this->is_logged_in), tab_page.get()->toggle(checked, this->is_logged_in)); 
		});
		
		std::shared_ptr<kami_tabpage> kami_tab_page = std::make_shared<kami_tabpage>(this);
		kami_tab_page.get()->create(kami_tab_page, rectangle(tabpage_position, y, window_width - tabpage_position - 1, window_height - y - 1));

		this->tabmenu.get()->relate(kami_tab_index, kami_tab_page);
		this->tabpage_container.push_back(kami_tab_page);
	}

	void main_window::create_itemfilter_tab(int y)
	{
		int itemfilter_tab_index = this->tabmenu.get()->add_tab("Item Filter", true, png_button_itemfilter, true,
			[this](bool checked) -> bool 
		{
			std::shared_ptr<custom_tabpage> tab_page = this->tabpage_container.at(tabpage_ids::itemfilter);
			return (tab_page.get()->set_toggle(checked, this->is_logged_in), tab_page.get()->toggle(checked, this->is_logged_in)); 
		});
		
		std::shared_ptr<itemfilter_tabpage> itemfilter_tab_page = std::make_shared<itemfilter_tabpage>(this);
		itemfilter_tab_page.get()->create(itemfilter_tab_page, rectangle(tabpage_position, y, window_width - tabpage_position - 1, window_height - y - 1));
		itemfilter_tab_page.get()->set_item_pool(&this->items);

		this->tabmenu.get()->relate(itemfilter_tab_index, itemfilter_tab_page);
		this->tabpage_container.push_back(itemfilter_tab_page);
	}

	void main_window::create_spawncontrol_tab(int y)
	{
		int spawncontrol_tab_index = this->tabmenu.get()->add_tab("Spawn Control", true, png_button_spawncontrol, true,
			[this](bool checked) -> bool 
		{
			std::shared_ptr<custom_tabpage> tab_page = this->tabpage_container.at(tabpage_ids::spawncontrol);
			return (tab_page.get()->set_toggle(checked, this->is_logged_in), tab_page.get()->toggle(checked, this->is_logged_in)); 
		});
		
		std::shared_ptr<spawncontrol_tabpage> spawncontrol_tab_page = std::make_shared<spawncontrol_tabpage>(this);
		spawncontrol_tab_page.get()->create(spawncontrol_tab_page, rectangle(tabpage_position, y, window_width - tabpage_position - 1, window_height - y - 1));
		spawncontrol_tab_page.get()->set_map_pool(&this->maps);

		this->tabmenu.get()->relate(spawncontrol_tab_index, spawncontrol_tab_page);
		this->tabpage_container.push_back(spawncontrol_tab_page);
	}

	void main_window::create_exclusive_tab(int y)
	{
		int exclusive_tab_index = this->tabmenu.get()->add_tab("Exclusive", true, png_button_exclusive, true,
			[this](bool checked) -> bool 
		{
			std::shared_ptr<custom_tabpage> tab_page = this->tabpage_container.at(tabpage_ids::exclusive);
			return (tab_page.get()->set_toggle(checked, this->is_logged_in), tab_page.get()->toggle(checked, this->is_logged_in)); 
		});
		
		std::shared_ptr<exclusive_tabpage> exclusive_tab_page = std::make_shared<exclusive_tabpage>(this);
		exclusive_tab_page.get()->create(exclusive_tab_page, rectangle(tabpage_position, y, window_width - tabpage_position - 1, window_height - y - 1));

		this->tabmenu.get()->relate(exclusive_tab_index, exclusive_tab_page);
		this->tabpage_container.push_back(exclusive_tab_page);
	}
	
	void main_window::create_rusher_tab(int y)
	{
		int rusher_tab_index = this->tabmenu.get()->add_tab("Map Rusher", true, png_button_rusher);
		
		std::shared_ptr<rusher_tabpage> rusher_tab_page = std::make_shared<rusher_tabpage>(this);
		rusher_tab_page.get()->create(rusher_tab_page, rectangle(tabpage_position, y, window_width - tabpage_position - 1, window_height - y - 1));
		rusher_tab_page.get()->set_map_pool(&this->maps);

		this->tabmenu.get()->relate(rusher_tab_index, rusher_tab_page);
		this->tabpage_container.push_back(rusher_tab_page);
	}

	void main_window::create_scripting_tab(int y)
	{
		int scripting_tab_index = this->tabmenu.get()->add_tab("Scripting", true, png_button_scripting);
		
		std::shared_ptr<scripting_tabpage> scripting_tab_page = std::make_shared<scripting_tabpage>(this);
		scripting_tab_page.get()->create(scripting_tab_page, rectangle(tabpage_position, y, window_width - tabpage_position - 1, window_height - y - 1));

		this->tabmenu.get()->relate(scripting_tab_index, scripting_tab_page);
		this->tabpage_container.push_back(scripting_tab_page);
	}

	void main_window::create_settings_tab(int y)
	{
		int settings_tab_index = this->tabmenu.get()->add_tab("Settings", true, png_button_settings);
		
		std::shared_ptr<settings_tabpage> settings_tab_page = std::make_shared<settings_tabpage>(this);
		settings_tab_page.get()->create(settings_tab_page, rectangle(tabpage_position, y, window_width - tabpage_position - 1, window_height - y - 1));

		this->tabmenu.get()->relate(settings_tab_index, settings_tab_page);
		this->tabpage_container.push_back(settings_tab_page);
	}
	
	void main_window::on_fetch_data(net_buffer& packet)
	{
		fetch_data_ids fetch_data_id = static_cast<fetch_data_ids>(packet.read1());

		switch (fetch_data_id)
		{
		case fetch_data_ids::spawncontrol_data:
			return dynamic_cast<spawncontrol_tabpage*>(this->tabpage_container.at(tabpage_ids::spawncontrol).get())->on_fetch_spawncontrol_data(packet);

		case fetch_data_ids::rusher_data:
			return dynamic_cast<rusher_tabpage*>(this->tabpage_container.at(tabpage_ids::rusher).get())->on_fetch_rusher_data(packet);

		case fetch_data_ids::dupex_data:
			return dynamic_cast<exclusive_tabpage*>(this->tabpage_container.at(tabpage_ids::exclusive).get())->on_fetch_dupex_data(packet);

		default:
			break;
		}
	}

	void main_window::on_accepted_game_connection(unsigned int process_id)
	{
		for (std::shared_ptr<custom_tabpage>& tab_page : this->tabpage_container)
		{
			if (tab_page.get()->get_toggle())
				tab_page.get()->toggle(true);
		}

		dynamic_cast<autologin_tabpage*>(this->tabpage_container.at(tabpage_ids::autologin).get())->on_accepted_game_connection(process_id);
	}

	void main_window::on_closed_game_connection()
	{
		dynamic_cast<autologin_tabpage*>(this->tabpage_container.at(tabpage_ids::autologin).get())->on_closed_game_connection();
	}

	void main_window::on_accepted_manager_connection()
	{
		dynamic_cast<settings_tabpage*>(this->tabpage_container.at(tabpage_ids::settings).get())->on_accepted_manager_connection();
	}

	void main_window::on_closed_manager_connection()
	{
		dynamic_cast<settings_tabpage*>(this->tabpage_container.at(tabpage_ids::settings).get())->on_closed_manager_connection();
	}
}