#pragma once

#include "native.hpp"
#include "custom_tabpage.hpp"

#include "combobox.hpp"
#include "textbox.hpp"
#include "togglebox.hpp"

#include <memory>
#include <vector>

namespace firefly
{
	class autoshop_tabpage : public custom_tabpage
	{
	public:
		autoshop_tabpage(main_window* parent);
		~autoshop_tabpage();

		void save_to_profile(std::string const& profile_path);
		void load_from_profile(std::string const& profile_path);
		
		void toggle_functionality(bool checked);

	protected:
		void create_controls(rectangle& rect);

	private:
		std::unique_ptr<togglebox> auto_potion_restock_hp_togglebox;
		std::unique_ptr<textbox> auto_potion_restock_hp_from_textbox;
		std::unique_ptr<textbox> auto_potion_restock_hp_to_textbox;

		std::unique_ptr<togglebox> auto_potion_restock_mp_togglebox;
		std::unique_ptr<textbox> auto_potion_restock_mp_from_textbox;
		std::unique_ptr<textbox> auto_potion_restock_mp_to_textbox;
		
		std::unique_ptr<togglebox> auto_restock_stars_bullets_togglebox;
		
		std::unique_ptr<togglebox> auto_sell_togglebox;
		std::vector<std::shared_ptr<togglebox>> auto_sell_type_togglebox_container;
		std::unique_ptr<textbox> auto_sell_delay_textbox;
		std::vector<std::shared_ptr<togglebox>> auto_sell_filter_type_togglebox_container;

		std::unique_ptr<togglebox> auto_transfer_mesos_togglebox;
		std::unique_ptr<textbox> auto_transfer_mesos_textbox;
		std::unique_ptr<textbox> auto_transfer_mesos_limit_textbox;
		std::unique_ptr<textbox> auto_transfer_mesos_delay_textbox;

		std::unique_ptr<combobox> auto_transfer_channel_combobox;
		std::unique_ptr<combobox> auto_transfer_free_market_combobox;
		std::unique_ptr<textbox> auto_transfer_shop_id_textbox;
		std::unique_ptr<textbox> auto_transfer_item_textbox;
		std::unique_ptr<textbox> auto_transfer_item_price_textbox;
	};
}