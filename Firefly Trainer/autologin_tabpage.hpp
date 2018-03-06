#pragma once

#include "native.hpp"
#include "custom_tabpage.hpp"

#include "combobox.hpp"
#include "label.hpp"
#include "textbox.hpp"
#include "togglebox.hpp"

#include <memory>

namespace firefly
{
	class autologin_tabpage : public custom_tabpage
	{
	public:
		autologin_tabpage(main_window* parent);
		~autologin_tabpage();

		void save_to_profile(std::string const& profile_path);
		void load_from_profile(std::string const& profile_path);
		
		void toggle_functionality(bool checked);

		void on_accepted_game_connection(unsigned int process_id);
		void on_closed_game_connection(bool relaunch = true);

	protected:
		void create_controls(rectangle& rect);
		
	private:
		bool launch_game();
		void set_game_path(std::string const& path);

	private:
		std::unique_ptr<textbox> username_textbox;
		std::unique_ptr<textbox> password_textbox;
		std::unique_ptr<textbox> pic_textbox;
		
		std::unique_ptr<combobox> region_combobox;
		std::unique_ptr<combobox> world_combobox;
		std::unique_ptr<combobox> channel_combobox;
		std::unique_ptr<combobox> character_combobox;
		
		std::unique_ptr<togglebox> offline_mode_togglebox;
		std::unique_ptr<togglebox> randomize_hwid_togglebox;
		
		std::unique_ptr<togglebox> maplestory_relaunch_togglebox;
		std::unique_ptr<label> maplestory_status_label;
		std::unique_ptr<label> game_path_label;
	};
}