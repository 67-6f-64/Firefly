#pragma once

#include "native.hpp"

#include "window.hpp"
#include "widget.hpp"

#include "tab_menu.hpp"
#include "custom_tabpage.hpp"

#include "net_opcodes.hpp"

#include "button.hpp"
#include "combobox.hpp"
#include "label.hpp"
#include "togglebox.hpp"

#include "item_pool.hpp"
#include "map_pool.hpp"

#include <memory>
#include <vector>

namespace firefly
{
	class main_window : public window
	{
		friend class custom_tabpage;

	public:
		main_window(HINSTANCE hinstance, std::string const& class_name, std::string const& window_name);
		~main_window();

		bool is_dialog_message(MSG* message);
		
		void save_profile(std::string const& profile_path);
		void load_profile(std::string const& profile_path);

		void initialize();

		bool try_authenticate(std::string const& email, std::string const& password);
		bool try_launch_game(std::string const& file_path, std::string const& username, std::string const& password);

	private:
		void create_handlers();
		void create_controls();

		void create_tab_control(int y);

		void create_autobotting_tab(int y);
		void create_autoshop_tab(int y);
		void create_autologin_tab(int y);
		void create_hacks_tab(int y);
		void create_kami_tab(int y);
		void create_itemfilter_tab(int y);
		void create_spawncontrol_tab(int y);
		void create_exclusive_tab(int y);

		void create_rusher_tab(int y);
		void create_scripting_tab(int y);
		void create_settings_tab(int y);

	protected:
		virtual bool authenticate(std::string const& email, std::string const& password) = 0;
		virtual bool launch_game(std::string const& directory, std::string const& username, std::string const& password) = 0;

		virtual void handle_select_message(SOCKET sock, unsigned short select_event, unsigned short select_error, bool game) = 0;
	
		void on_fetch_data(net_buffer& packet);

		void on_accepted_game_connection(unsigned int process_id);
		void on_closed_game_connection();

		void on_accepted_manager_connection();
		void on_closed_manager_connection();

	private:
		virtual bool send_game_packet(net_buffer& packet) = 0;

	protected:
		item_pool items;
		map_pool maps;

		std::vector<std::shared_ptr<custom_tabpage>> tabpage_container;

	private:
		bool is_logged_in;
		std::unique_ptr<tab_menu> tabmenu;
	};
}