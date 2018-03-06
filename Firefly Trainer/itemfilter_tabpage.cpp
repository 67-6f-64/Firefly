#include "itemfilter_tabpage.hpp"
#include "resource.hpp"

#include "button.hpp"
#include "label.hpp"

namespace firefly
{
	itemfilter_tabpage::itemfilter_tabpage(main_window* parent)
		: custom_tabpage(parent)
	{
		this->items = nullptr;
		this->tab_bar_continue = 0;
	}

	itemfilter_tabpage::~itemfilter_tabpage()
	{

	}
	
	void itemfilter_tabpage::set_item_pool(item_pool* items)
	{
		this->items = items;
	}

	void itemfilter_tabpage::save_to_profile(std::string const& profile_path)
	{
		this->profile_writer.set_path(profile_path);
		
		this->profile_writer.write_int("ItemFilter", "DropFilterTypeValue", this->drop_filter_type_combobox.get()->get_selection());
		this->profile_writer.write_int("ItemFilter", "DropFilterSchemeValue", this->drop_filter_scheme_combobox.get()->get_selection());

		this->profile_writer.write_int("ItemFilter", "DropFilterTypeCount", this->drop_filter_filter_by_type_togglebox_container.size());
		
		for (std::size_t i = 0; i < this->drop_filter_filter_by_type_togglebox_container.size(); i++)
			this->profile_writer.write_int("ItemFilter", "DropFilterType" + std::to_string(i) + "Check", this->drop_filter_filter_by_type_togglebox_container.at(i).get()->get_check());

		this->profile_writer.write_int("ItemFilter", "DropFilterMesosCheck", this->drop_filter_mesos_togglebox.get()->get_check());
		this->profile_writer.write_string("ItemFilter", "DropFilterMesosValue", this->drop_filter_mesos_textbox.get()->get_text());

		this->profile_writer.write_int("ItemFilter", "DropFilterCount", this->drop_filter_listview.get()->get_item_count());

		for (int i = 0; i < this->drop_filter_listview.get()->get_item_count(); i++)
			this->profile_writer.write_string("ItemFilter", "DropFilter" + std::to_string(i) + "Value", this->drop_filter_listview.get()->get_item_data(i, 0));

		this->profile_writer.write_int("ItemFilter", "SellFilterCount", this->sell_filter_listview.get()->get_item_count());
		
		for (int i = 0; i < this->sell_filter_listview.get()->get_item_count(); i++)
			this->profile_writer.write_string("ItemFilter", "SellFilter" + std::to_string(i) + "Value", this->sell_filter_listview.get()->get_item_data(i, 0));
	}

	void itemfilter_tabpage::load_from_profile(std::string const& profile_path)
	{
		this->profile_reader.set_path(profile_path);
		
		this->drop_filter_type_combobox.get()->set_selection(this->profile_reader.get_int("ItemFilter", "DropFilterTypeValue", 0));
		this->drop_filter_scheme_combobox.get()->set_selection(this->profile_reader.get_int("ItemFilter", "DropFilterSchemeValue", 0));

		int drop_filter_type_count = this->profile_reader.get_int("ItemFilter", "DropFilterTypeCount", 0);

		for (int i = 0; i < drop_filter_type_count; i++)
			this->drop_filter_filter_by_type_togglebox_container.at(i).get()->set_check(this->profile_reader.get_int("ItemFilter", "DropFilterType" + std::to_string(i) + "Check", FALSE) != FALSE);
		
		this->drop_filter_mesos_togglebox.get()->set_check(this->profile_reader.get_int("ItemFilter", "DropFilterMesosCheck", FALSE) != FALSE);
		this->drop_filter_mesos_textbox.get()->set_text(this->profile_reader.get_string("ItemFilter", "DropFilterMesosValue", ""));

		int drop_filter_count = this->profile_reader.get_int("ItemFilter", "DropFilterCount", 0);

		for (int i = 0; i < drop_filter_count; i++)
		{
			std::string item_id = this->profile_reader.get_string("ItemFilter", "DropFilter" + std::to_string(i) + "Value", "0");
			std::string item_name = this->items->get_item_name(item_id);

			this->drop_filter_listview.get()->add_item_manual(item_id, item_name);
		}
			
		this->drop_filter_listview.get()->resize_list_view(false);

		int sell_filter_count = this->profile_reader.get_int("ItemFilter", "SellFilterCount", 0);
		
		for (int i = 0; i < sell_filter_count; i++)
		{
			std::string item_id = this->profile_reader.get_string("ItemFilter", "SellFilter" + std::to_string(i) + "Value", "0");
			std::string item_name = this->items->get_item_name(item_id);

			this->sell_filter_listview.get()->add_item_manual(item_id, item_name);
		}
		
		this->sell_filter_listview.get()->resize_list_view(false);
	}
	
	void itemfilter_tabpage::toggle_functionality(bool checked)
	{
		net_buffer packet(trainer_tabpage_toggle);
		packet.write1(tabpage_ids::itemfilter);
		
		if (packet.write1(checked))
		{
			packet.write1(this->drop_filter_type_combobox.get()->get_selection());
			int scheme = packet.write1(this->drop_filter_scheme_combobox.get()->get_selection());
			
			if (packet.write1(this->drop_filter_mesos_togglebox.get()->get_check()))
				packet.write4(this->drop_filter_mesos_textbox.get()->get_integer());

			if (scheme == itemfilter_type_ids::filter_by_type || scheme == itemfilter_type_ids::custom_filter)
			{
				for (std::shared_ptr<togglebox> filter_by_type_togglebox : this->drop_filter_filter_by_type_togglebox_container)
					packet.write1(filter_by_type_togglebox.get()->get_check());
			}
			
			if (scheme == itemfilter_type_ids::custom_filter)
			{
				int drop_filter_count = packet.write4(this->drop_filter_listview.get()->get_item_count());

				for (int i = 0; i < drop_filter_count; i++)
					packet.write4(this->drop_filter_listview.get()->get_item_integer(i, 0));
			}

			int sell_filter_count = packet.write4(this->drop_filter_listview.get()->get_item_count());

			for (int i = 0; i < sell_filter_count; i++)
				packet.write4(this->sell_filter_listview.get()->get_item_integer(i, 0));
		}

		this->send_game_packet(packet);
	}

	void itemfilter_tabpage::create_controls(rectangle& rect)
	{
		/* section image */
		std::shared_ptr<button> section_image = std::make_shared<button>(*this);
		section_image.get()->create("", rectangle((rect.width / 2) - (image_size / 2), image_offset, image_size, image_size));
		section_image.get()->set_bitmap(png_button_itemfilter);
		this->store_control(section_image);

		/* tab bar */
		int tab_width = (rect.width - (TABPAGE_INDENT_LEFT + TABPAGE_INDENT_RIGHT) - (TAB_H_INDENT * 2)) / 3;

		this->tabbar = std::make_unique<tab_bar>(*this);
		this->tab_bar_continue = this->tabbar.get()->create(rectangle(STANDARD_INDENT, image_offset + image_size + image_offset, rect.width - (TABPAGE_INDENT_LEFT + TABPAGE_INDENT_RIGHT), 0));
		this->tabbar.get()->set_event(tab_base_event_type::on_selection_change, [this](int index) -> void
		{
			this->scroll_bar.get()->scroll_to(0);
		});
		this->tabbar.get()->set_event(tab_base_event_type::on_selection_changed, [this](int index) -> void
		{
			this->reset_scrollbar(true);
		});

		this->create_search_items_tab(rect, tab_width, this->tab_bar_continue);
		this->create_drop_filter_tab(rect, tab_width, this->tab_bar_continue);
		this->create_sell_filter_tab(rect, tab_width, this->tab_bar_continue);
		
		this->reset_scrollbar(true);
	}
	
	void itemfilter_tabpage::create_search_items_tab(rectangle& rect, int tab_width, int y)
	{
		int search_items_tab_index = this->tabbar.get()->add_tab("SEARCH ITEMS", tab_width);

		this->item_search_textbox = std::make_shared<textbox>(*this);
		this->item_search_textbox.get()->create(rectangle(TABPAGE_INDENT_LEFT, y, rect.width - (TABPAGE_INDENT_LEFT + TABPAGE_INDENT_RIGHT), control_height), true);
		this->item_search_textbox.get()->set_cue_banner("Enter search query (item name or id)...");
		this->item_search_textbox.get()->set_event(textbox_event::on_return, [this](std::string const& text) -> void
		{
			if (text.length() > 0)
			{
				this->item_search_listview.get()->clear_items();

				std::vector<const item_data> results;
				
				if (this->items->query_items(text, results) && results.size() <= 999)
				{
					for (std::size_t i = 0; i < results.size(); i++)
						this->item_search_listview.get()->add_item_manual(static_cast<int>(i + 1), results.at(i).id, results.at(i).name);
					
					this->item_search_listview.get()->resize_list_view();
				}
			}
		});

		this->tabbar.get()->relate(search_items_tab_index, this->item_search_textbox);

		this->item_search_listview = std::make_shared<list_view>(*this);
		this->item_search_listview.get()->create(rectangle(TABPAGE_INDENT_LEFT, y + control_height + STANDARD_INDENT, rect.width - (TABPAGE_INDENT_LEFT + TABPAGE_INDENT_RIGHT), 400), this->self);
		this->item_search_listview.get()->resize_list_view(false);
		
		this->item_search_listview.get()->add_column("#", (STANDARD_INDENT * 2) + 25);
		this->item_search_listview.get()->add_column("Item Id", 100, true);
		this->item_search_listview.get()->add_column("Item Name", 423);
		
		this->item_search_listview.get()->add_menu_item("Add to Drop Filter", [this](int index) -> bool 
		{ 
			int item_id = 0;

			try
			{
				item_id = std::stoi(this->item_search_listview.get()->get_item_data(index, 1));
			}
			catch (std::exception& e)
			{
				UNREFERENCED_PARAMETER(e);
				return false;
			}

			for (int i = 0; i < this->drop_filter_listview.get()->get_item_count(); i++)
				if (this->drop_filter_listview.get()->get_item_integer(i, 0) == item_id)
					return false;
			
			std::string item_name = this->item_search_listview.get()->get_item_data(index, 2);

			this->drop_filter_listview.get()->add_item(item_id, item_name);
			return true;
		});

		this->item_search_listview.get()->add_menu_item("Add to Sell Filter", [this](int index) -> bool 
		{ 
			int item_id = 0;

			try
			{
				item_id = std::stoi(this->item_search_listview.get()->get_item_data(index, 1));
			}
			catch (std::exception& e)
			{
				UNREFERENCED_PARAMETER(e);
				return false;
			}

			for (int i = 0; i < this->sell_filter_listview.get()->get_item_count(); i++)
				if (this->sell_filter_listview.get()->get_item_integer(i, 0) == item_id)
					return false;
			
			std::string item_name = this->item_search_listview.get()->get_item_data(index, 2);

			this->sell_filter_listview.get()->add_item(item_id, item_name);
			return true;
		});

		this->tabbar.get()->relate(search_items_tab_index, this->item_search_listview);
	}

	void itemfilter_tabpage::create_drop_filter_tab(rectangle& rect, int tab_width, int y)
	{
		int drop_filter_tab_index = this->tabbar.get()->add_tab("DROP FILTER", tab_width);
		
		std::shared_ptr<label> drop_filter_type_label = std::make_shared<label>(*this);
		drop_filter_type_label.get()->create("Drop Filter Type:", rectangle(TABPAGE_INDENT_LEFT, y, control_width, control_height), false, true);
		this->tabbar.get()->relate(drop_filter_tab_index, drop_filter_type_label);

		this->drop_filter_type_combobox = std::make_unique<combobox>(*this);
		this->drop_filter_type_combobox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), y, control_width + key_indent + key_width, control_height), true);
		this->drop_filter_type_combobox.get()->add_item("Blacklist");
		this->drop_filter_type_combobox.get()->add_item("Whitelist");
		this->drop_filter_type_combobox.get()->set_selection(0);
		this->tabbar.get()->relate(drop_filter_tab_index, this->drop_filter_type_combobox);
		
		std::shared_ptr<label> drop_filter_scheme_label = std::make_shared<label>(*this);
		drop_filter_scheme_label.get()->create("Drop Filter Scheme:", rectangle(TABPAGE_INDENT_LEFT, y += ((control_height + control_indent) + 2), control_width, control_height), false, true);
		this->tabbar.get()->relate(drop_filter_tab_index, drop_filter_scheme_label);

		this->drop_filter_scheme_combobox = std::make_unique<combobox>(*this);
		this->drop_filter_scheme_combobox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), y, control_width + key_indent + key_width, control_height), true);
		this->drop_filter_scheme_combobox.get()->add_item("Filter everything");
		this->drop_filter_scheme_combobox.get()->add_item("Filter by type");
		this->drop_filter_scheme_combobox.get()->add_item("Custom filter (type + list below)");
		this->drop_filter_scheme_combobox.get()->set_event(combobox_event::on_selection_change, [this](int index) -> void
		{
			for (std::shared_ptr<togglebox> filter_by_type_togglebox : this->drop_filter_filter_by_type_togglebox_container)
				filter_by_type_togglebox.get()->enable(index == itemfilter_type_ids::filter_by_type || index == itemfilter_type_ids::custom_filter);
		});
		this->tabbar.get()->relate(drop_filter_tab_index, this->drop_filter_scheme_combobox);

		y += (control_height + control_indent) + 3;

		std::string auto_drop_by_type_types[] = { "Equip", "Etc", "Use" };

		for (int i = 0, count = _countof(auto_drop_by_type_types); i < count; i++)
		{
			const int togglebox_width = 16;

			int x = rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT);
			int cx = ((rect.width - TABPAGE_INDENT_RIGHT) - x) / count;
			int dx = ((cx / 2) - (togglebox_width / 2));

			std::shared_ptr<togglebox> drop_filter_by_type_togglebox = std::make_shared<togglebox>(*this);
			drop_filter_by_type_togglebox.get()->create("", rectangle(x + (cx * i) + dx, y, 16, control_height));
			this->drop_filter_filter_by_type_togglebox_container.push_back(drop_filter_by_type_togglebox);
			this->tabbar.get()->relate(drop_filter_tab_index, drop_filter_by_type_togglebox);
			
			std::shared_ptr<label> drop_filter_by_type_label = std::make_shared<label>(*this);
			drop_filter_by_type_label.get()->create(auto_drop_by_type_types[i], rectangle(x + (cx * i), y + control_height, cx, control_height), true);
			this->tabbar.get()->relate(drop_filter_tab_index, drop_filter_by_type_label);
		}
		
		this->drop_filter_scheme_combobox.get()->set_selection(0);

		this->drop_filter_mesos_togglebox = std::make_unique<togglebox>(*this);
		this->drop_filter_mesos_togglebox.get()->create("Filter Mesos <", rectangle(TABPAGE_INDENT_LEFT, y += (((control_height * 2) + control_indent) - 4), control_width, control_height));
		this->tabbar.get()->relate(drop_filter_tab_index, this->drop_filter_mesos_togglebox);
		
		this->drop_filter_mesos_textbox = std::make_unique<textbox>(*this);
		this->drop_filter_mesos_textbox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), y, control_width + key_indent + key_width, control_height), true, true);
		this->drop_filter_mesos_textbox.get()->set_maximum_length(5);
		this->drop_filter_mesos_textbox.get()->set_cue_banner("amount");
		this->tabbar.get()->relate(drop_filter_tab_index, this->drop_filter_mesos_textbox);
		
		this->drop_filter_listview = std::make_shared<list_view>(*this);
		this->drop_filter_listview.get()->create(rectangle(TABPAGE_INDENT_LEFT, y += (control_height + control_indent), rect.width - (TABPAGE_INDENT_LEFT + TABPAGE_INDENT_RIGHT), 400), this->self);
		this->drop_filter_listview.get()->resize_list_view(false);
		
		this->drop_filter_listview.get()->add_column("Item Id", 100, true);
		this->drop_filter_listview.get()->add_column("Item Name", 480);
		
		this->drop_filter_listview.get()->add_menu_item("Remove Item", [this](int index) -> bool 
		{ 
			this->drop_filter_listview.get()->remove_item(index);
			return true;
		});

		this->drop_filter_listview.get()->add_menu_item("Clear Items", [this](int index) -> bool 
		{ 
			this->drop_filter_listview.get()->clear_items();
			return true;
		});

		this->tabbar.get()->relate(drop_filter_tab_index, this->drop_filter_listview);
	}

	void itemfilter_tabpage::create_sell_filter_tab(rectangle& rect, int tab_width, int y)
	{
		int sell_filter_tab_index = this->tabbar.get()->add_tab("SELL FILTER", tab_width);
		
		this->sell_filter_listview = std::make_shared<list_view>(*this);
		this->sell_filter_listview.get()->create(rectangle(TABPAGE_INDENT_LEFT, y, rect.width - (TABPAGE_INDENT_LEFT + TABPAGE_INDENT_RIGHT), 400), this->self);
		this->sell_filter_listview.get()->resize_list_view(false);
		
		this->sell_filter_listview.get()->add_column("Item Id", 100, true);
		this->sell_filter_listview.get()->add_column("Item Name", 480);
		
		this->sell_filter_listview.get()->add_menu_item("Remove Item", [this](int index) -> bool 
		{ 
			this->sell_filter_listview.get()->remove_item(index);
			return true;
		});

		this->sell_filter_listview.get()->add_menu_item("Clear Items", [this](int index) -> bool 
		{ 
			this->sell_filter_listview.get()->clear_items();
			return true;
		});

		this->tabbar.get()->relate(sell_filter_tab_index, this->sell_filter_listview);
	}
}