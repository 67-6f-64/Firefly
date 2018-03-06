#pragma once

#include "native.hpp"
#include "custom_tabpage.hpp"

#include "exclusive_types.hpp"

#include "combobox.hpp"
#include "textbox.hpp"
#include "togglebox.hpp"
#include "trackbar.hpp"

#include <memory>
#include <vector>

namespace firefly
{
	class exclusive_tabpage : public custom_tabpage
	{
	public:
		exclusive_tabpage(main_window* parent);
		~exclusive_tabpage();

		void save_to_profile(std::string const& profile_path);
		void load_from_profile(std::string const& profile_path);
		
		void toggle_functionality(bool checked);
		
		void on_fetch_dupex_data(net_buffer& packet);

	protected:
		void create_controls(rectangle& rect);

	private:
		bool toggle_func(exclusive_type type, bool enabled);
		void handle_type(exclusive_type type, net_buffer& packet, bool custom = false, bool enabled = false);

	private:
		std::unique_ptr<togglebox> pet_loot_togglebox;
		std::unique_ptr<trackbar> pet_loot_trackbar;
		std::unique_ptr<textbox> pet_loot_textbox;
		
		std::unique_ptr<togglebox> fullmap_attack_togglebox;
		std::unique_ptr<textbox> fullmap_attack_id_textbox;
		std::unique_ptr<combobox> fullmap_attack_id_combobox;

		std::unique_ptr<togglebox> skill_inject_togglebox;
		std::unique_ptr<textbox> skill_inject_id_textbox;
		std::unique_ptr<combobox> skill_inject_id_combobox;
		std::unique_ptr<trackbar> skill_inject_delay_trackbar;
		std::unique_ptr<textbox> skill_inject_delay_textbox;
		
		std::unique_ptr<togglebox> dupex_togglebox;
		std::unique_ptr<textbox> dupex_x_textbox;
		std::unique_ptr<textbox> dupex_y_textbox;
	};
}