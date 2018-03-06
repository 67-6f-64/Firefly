#pragma once

#include "native.hpp"
#include "custom_tabpage.hpp"

#include "combobox.hpp"
#include "list_view.hpp"
#include "textbox.hpp"
#include "togglebox.hpp"

#include "map_pool.hpp"

#include <memory>
#include <vector>

namespace firefly
{
	class spawncontrol_tabpage : public custom_tabpage
	{
		const int level_width = 60;
		const int level_between_width = 10;
		
		const int togglebox_width = 136;

	public:
		spawncontrol_tabpage(main_window* parent);
		~spawncontrol_tabpage();
		
		void set_map_pool(map_pool* maps);

		void save_to_profile(std::string const& profile_path);
		void load_from_profile(std::string const& profile_path);
		
		void toggle_functionality(bool checked);

		void on_fetch_spawncontrol_data(net_buffer& packet);

	protected:
		void create_controls(rectangle& rect);
		void create_map_lock_tab(rectangle& rect, int tab_width, int y);
		
	private:
		void reset_list_numbering();

	private:
		map_pool* maps;
		
		std::shared_ptr<textbox> map_lock_map_id_textbox;
		
		std::shared_ptr<togglebox> map_lock_coordinate_togglebox;
		std::shared_ptr<textbox> map_lock_x_coordinate_textbox;
		std::shared_ptr<textbox> map_lock_y_coordinate_textbox;

		std::shared_ptr<togglebox> map_lock_level_togglebox;
		std::shared_ptr<textbox> map_lock_level_minimum_textbox;
		std::shared_ptr<textbox> map_lock_level_maximum_textbox;
		
		std::shared_ptr<list_view> map_lock_list;
	};
}