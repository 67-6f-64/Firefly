#pragma once

#include "native.hpp"
#include "custom_tabpage.hpp"

#include "combobox.hpp"
#include "textbox.hpp"
#include "togglebox.hpp"
#include "trackbar.hpp"

#include <memory>
#include <vector>

namespace firefly
{
	class kami_tabpage : public custom_tabpage
	{
	public:
		kami_tabpage(main_window* parent);
		~kami_tabpage();

		void save_to_profile(std::string const& profile_path);
		void load_from_profile(std::string const& profile_path);
		
		void toggle_functionality(bool checked);

	protected:
		void create_controls(rectangle& rect);

	private:
		std::unique_ptr<trackbar> vertical_range_trackbar;
		std::unique_ptr<textbox> vertical_range_textbox;

		std::unique_ptr<trackbar> horizontal_range_trackbar;
		std::unique_ptr<textbox> horizontal_range_textbox;

		std::unique_ptr<combobox> kami_type_combobox;
		std::unique_ptr<combobox> kami_target_combobox;

		std::unique_ptr<togglebox> kami_item_loot_checkbox;
		std::unique_ptr<textbox> kami_item_loot_textbox;
		std::unique_ptr<combobox> kami_loot_target_combobox;
	};
}