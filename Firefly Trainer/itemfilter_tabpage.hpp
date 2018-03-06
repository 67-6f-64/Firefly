#pragma once

#include "native.hpp"
#include "custom_tabpage.hpp"

#include "combobox.hpp"
#include "list_view.hpp"
#include "tab_bar.hpp"
#include "textbox.hpp"
#include "togglebox.hpp"

#include "item_pool.hpp"

#include <memory>
#include <vector>

namespace firefly
{
	class itemfilter_tabpage : public custom_tabpage
	{
	public:
		itemfilter_tabpage(main_window* parent);
		~itemfilter_tabpage();
		
		void set_item_pool(item_pool* items);

		void save_to_profile(std::string const& profile_path);
		void load_from_profile(std::string const& profile_path);
		
		void toggle_functionality(bool checked);

	protected:
		void create_controls(rectangle& rect);
		
		void create_search_items_tab(rectangle& rect, int tab_width, int y);
		void create_drop_filter_tab(rectangle& rect, int tab_width, int y);
		void create_sell_filter_tab(rectangle& rect, int tab_width, int y);

	private:
		item_pool* items;
		int tab_bar_continue;

		std::unique_ptr<tab_bar> tabbar;

		std::shared_ptr<textbox> item_search_textbox;
		std::shared_ptr<list_view> item_search_listview;
		
		std::shared_ptr<combobox> drop_filter_type_combobox;
		std::shared_ptr<combobox> drop_filter_scheme_combobox;
		std::vector<std::shared_ptr<togglebox>> drop_filter_filter_by_type_togglebox_container;

		std::shared_ptr<togglebox> drop_filter_mesos_togglebox;
		std::shared_ptr<textbox> drop_filter_mesos_textbox;

		std::shared_ptr<list_view> drop_filter_listview;

		std::shared_ptr<list_view> sell_filter_listview;
	};
}