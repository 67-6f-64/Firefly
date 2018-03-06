#include "spawncontrol_tabpage.hpp"
#include "resource.hpp"

#include "button.hpp"
#include "label.hpp"
#include "tab_bar.hpp"

#include "utility.hpp"

#include <algorithm>

namespace firefly
{
	spawncontrol_tabpage::spawncontrol_tabpage(main_window* parent)
		: custom_tabpage(parent)
	{

	}

	spawncontrol_tabpage::~spawncontrol_tabpage()
	{

	}
	
	void spawncontrol_tabpage::set_map_pool(map_pool* maps)
	{
		this->maps = maps;
	}

	void spawncontrol_tabpage::save_to_profile(std::string const& profile_path)
	{
		this->profile_writer.set_path(profile_path);
		
		std::vector<std::pair<std::string, std::string>> section_data;
		section_data.push_back(std::make_pair("map_count", std::to_string(this->map_lock_list.get()->get_item_count())));
		
		for (int i = 0, count = this->map_lock_list.get()->get_item_count(); i < count; i++)
		{
			section_data.push_back(std::make_pair("map_number", std::to_string(i + 1)));
			section_data.push_back(std::make_pair("min_level", this->map_lock_list.get()->get_item_data(i, 1)));
			section_data.push_back(std::make_pair("max_level", this->map_lock_list.get()->get_item_data(i, 2)));
			section_data.push_back(std::make_pair("x", this->map_lock_list.get()->get_item_data(i, 3)));
			section_data.push_back(std::make_pair("y", this->map_lock_list.get()->get_item_data(i, 4)));
			section_data.push_back(std::make_pair("id", this->map_lock_list.get()->get_item_data(i, 5)));
		}
		
		this->profile_writer.write_section("MapLockEvents", section_data);
	}

	void spawncontrol_tabpage::load_from_profile(std::string const& profile_path)
	{
		this->profile_reader.set_path(profile_path);

		std::vector<std::pair<std::string, std::string>> section_data;
		this->profile_reader.get_section_strings("MapLockEvents", section_data);

		int map_count = 0;

		std::remove_if(section_data.begin(), section_data.end(), [&](std::pair<std::string, std::string> pair) -> bool
		{
			if (!pair.first.compare("map_count"))
			{
				try
				{
					map_count = std::stoi(pair.second, 0, 10);
					return true;
				}
				catch (std::exception& e)
				{
					UNREFERENCED_PARAMETER(e);
				}
			}

			return false;
		});

		for (int i = 0; i < map_count; i++)
		{
			const std::string map_prefix("map_number");

			std::string map_name = std::string("N/A");
			std::map<std::string, std::string> map_data;

			std::vector<std::pair<std::string, std::string>>::const_iterator iterator = section_data.begin();
			std::vector<std::pair<std::string, std::string>>::const_iterator end = section_data.end();

			do
			{
				map_data[(*iterator).first] = (*iterator).second;
				iterator++;
			} 
			while (iterator != end && (*iterator).first.compare(map_prefix));

			section_data.erase(section_data.begin(), iterator);

			try
			{
				map_name = this->maps->find_map_name(std::stoi(map_data["id"]));
			}
			catch (std::exception & e)
			{
				UNREFERENCED_PARAMETER(e);
			}

			this->map_lock_list.get()->add_item_manual(i + 1, map_data["min_level"], map_data["max_level"], map_data["x"], map_data["y"], map_data["id"], map_name);
		}
		
		this->map_lock_list.get()->resize_list_view();
	}
	
	void spawncontrol_tabpage::toggle_functionality(bool checked)
	{
		net_buffer packet(trainer_tabpage_toggle);
		packet.write1(tabpage_ids::spawncontrol);
		
		if (packet.write1(checked))
		{
			packet.write4(this->map_lock_list.get()->get_item_count());

			for (int i = 0, count = this->map_lock_list.get()->get_item_count(); i < count; i++)
			{
				packet.write2(this->map_lock_list.get()->get_item_integer(i, 1));
				packet.write2(this->map_lock_list.get()->get_item_integer(i, 2));
				packet.write4(this->map_lock_list.get()->get_item_integer(i, 3));
				packet.write4(this->map_lock_list.get()->get_item_integer(i, 4));
				packet.write4(this->map_lock_list.get()->get_item_integer(i, 5));
			}
		}

		this->send_game_packet(packet);
	}

	void spawncontrol_tabpage::on_fetch_spawncontrol_data(net_buffer& packet)
	{
		this->map_lock_map_id_textbox.get()->set_text(std::to_string(packet.read4()));
		this->map_lock_x_coordinate_textbox.get()->set_text(std::to_string(static_cast<int>(packet.read4())));
		this->map_lock_y_coordinate_textbox.get()->set_text(std::to_string(static_cast<int>(packet.read4())));
	}

	void spawncontrol_tabpage::create_controls(rectangle& rect)
	{
		/* section image */
		std::shared_ptr<button> section_image = std::make_shared<button>(*this);
		section_image.get()->create("", rectangle((rect.width / 2) - (image_size / 2), image_offset, image_size, image_size));
		section_image.get()->set_bitmap(png_button_spawncontrol);
		this->store_control(section_image);
		
		/* tab_bar */
		int tab_width = (rect.width - (TABPAGE_INDENT_LEFT + TABPAGE_INDENT_RIGHT) - TAB_H_INDENT) / 2;
		const int button_width = (tab_width - STANDARD_INDENT - control_indent) / 2;

		this->map_lock_map_id_textbox = std::make_shared<textbox>(*this);
		this->map_lock_map_id_textbox.get()->create(rectangle(TABPAGE_INDENT_LEFT, image_offset + image_size + image_offset, tab_width - STANDARD_INDENT, control_height), true, true);
		this->map_lock_map_id_textbox.get()->set_maximum_length(9);
		this->map_lock_map_id_textbox.get()->set_cue_banner("Map Id");
		
		std::shared_ptr<button> map_lock_add_button = std::make_shared<button>(*this);
		map_lock_add_button.get()->create("Add Map", rectangle(TABPAGE_INDENT_LEFT, image_offset + image_size + image_offset + control_height + control_indent, button_width, control_height));
		map_lock_add_button.get()->set_event(button_event::on_click, [this]() -> bool
		{
			std::string map_id = this->map_lock_map_id_textbox.get()->get_text();

			if (map_id.length() > 0)
			{
				if (!this->map_lock_x_coordinate_textbox.get()->get_text().empty() && !this->map_lock_y_coordinate_textbox.get()->get_text().empty())
				{
					std::string min_level = "N/A";
					std::string max_level = "N/A";
					std::string x_coord = this->map_lock_x_coordinate_textbox.get()->get_text();
					std::string y_coord = this->map_lock_y_coordinate_textbox.get()->get_text();

					if (this->map_lock_level_togglebox.get()->get_check())
					{
						if (!this->map_lock_level_minimum_textbox.get()->get_text().empty())
							min_level = this->map_lock_level_minimum_textbox.get()->get_text();

						if (!this->map_lock_level_maximum_textbox.get()->get_text().empty())
							max_level = this->map_lock_level_maximum_textbox.get()->get_text();
					}

					for (std::size_t i = 0, count = this->map_lock_list.get()->get_item_count(); i < count; i++)
					{
						std::string temp_min_level = this->map_lock_list.get()->get_item_data(i, 1);
						std::string temp_max_level = this->map_lock_list.get()->get_item_data(i, 2);
						std::string temp_x_coord = this->map_lock_list.get()->get_item_data(i, 3);
						std::string temp_y_coord = this->map_lock_list.get()->get_item_data(i, 4);
						std::string temp_map_id = this->map_lock_list.get()->get_item_data(i, 5);

						if (!map_id.compare(temp_map_id) && !min_level.compare(temp_min_level) && !max_level.compare(temp_max_level) &&
							!x_coord.compare(temp_x_coord) && !y_coord.compare(temp_y_coord))
							return false;
					}

					std::string map_name = (map_id.length() > 0 ? this->maps->find_map_name(std::stoi(map_id, 0, 10)) : std::string("N/A"));

					this->map_lock_list.get()->add_item(this->map_lock_list.get()->get_item_count() + 1, min_level, max_level, x_coord, y_coord, map_id, map_name);
				}
				else
				{
					MessageBox(this->handle(), "You must enter an x-coordinate and a y-coordinate.", "Coordinate Error", MB_OK | MB_ICONERROR | MB_TOPMOST | MB_SETFOREGROUND);
				}
			}
			else
			{
				MessageBox(this->handle(), "You must enter a Map ID.", "Map Error", MB_OK | MB_ICONERROR | MB_TOPMOST | MB_SETFOREGROUND);
			}

			return true;
		});
		this->store_control(map_lock_add_button);
		
		std::shared_ptr<button> map_lock_fetch_map_id_button = std::make_shared<button>(*this);
		map_lock_fetch_map_id_button.get()->create("Fetch Information", rectangle(TABPAGE_INDENT_LEFT + button_width + control_indent, image_offset + image_size + image_offset + control_height + control_indent, button_width, control_height));
		map_lock_fetch_map_id_button.get()->set_event(button_event::on_click, [this]() -> bool
		{
			net_buffer packet(trainer_fetch_data);
			packet.write1(fetch_data_ids::spawncontrol_data);

			if (!this->send_game_packet(packet))
				MessageBox(this->handle(), "You must be connected to the game client.", "Connection Error", MB_OK | MB_ICONERROR | MB_TOPMOST | MB_SETFOREGROUND);

			return true;
		});
		this->store_control(map_lock_fetch_map_id_button);
		
		this->map_lock_coordinate_togglebox = std::make_unique<togglebox>(*this);
		this->map_lock_coordinate_togglebox.get()->create("Assign Coordinates", rectangle(rect.width - (togglebox_width + STANDARD_INDENT + level_width + control_indent + level_between_width + control_indent + level_width + TABPAGE_INDENT_RIGHT), image_offset + image_size + image_offset, togglebox_width, control_height));
		this->map_lock_coordinate_togglebox.get()->set_check(true);
		this->map_lock_coordinate_togglebox.get()->set_event(togglebox_event::on_state_change, [](bool checked) -> bool
		{
			return false;
		});
		
		std::shared_ptr<label> map_lock_coordinate_label = std::make_shared<label>(*this);
		map_lock_coordinate_label.get()->create("-", rectangle(rect.width - (level_between_width + control_indent + level_width + TABPAGE_INDENT_RIGHT), image_offset + image_size + image_offset, level_between_width, control_height), true, true);
		this->store_control(map_lock_coordinate_label);
		
		this->map_lock_x_coordinate_textbox = std::make_shared<textbox>(*this);
		this->map_lock_x_coordinate_textbox.get()->create(rectangle(rect.width - (level_width + control_indent + level_between_width + control_indent + level_width + TABPAGE_INDENT_RIGHT), image_offset + image_size + image_offset, level_width, control_height), true);
		this->map_lock_x_coordinate_textbox.get()->set_maximum_length(5);
		this->map_lock_x_coordinate_textbox.get()->set_cue_banner("x");

		this->map_lock_y_coordinate_textbox = std::make_shared<textbox>(*this);
		this->map_lock_y_coordinate_textbox.get()->create(rectangle(rect.width - (level_width + TABPAGE_INDENT_RIGHT), image_offset + image_size + image_offset, level_width, control_height), true);
		this->map_lock_y_coordinate_textbox.get()->set_maximum_length(5);
		this->map_lock_y_coordinate_textbox.get()->set_cue_banner("y");

		this->map_lock_level_togglebox = std::make_unique<togglebox>(*this);
		this->map_lock_level_togglebox.get()->create("Assign Level Range", rectangle(rect.width - (togglebox_width + STANDARD_INDENT + level_width + control_indent + level_between_width + control_indent + level_width + TABPAGE_INDENT_RIGHT), image_offset + image_size + image_offset + control_height + control_indent, togglebox_width, control_height));
		
		std::shared_ptr<label> map_lock_level_label = std::make_shared<label>(*this);
		map_lock_level_label.get()->create("-", rectangle(rect.width - (level_between_width + control_indent + level_width + TABPAGE_INDENT_RIGHT), image_offset + image_size + image_offset + control_height + control_indent, level_between_width, control_height), true, true);
		this->store_control(map_lock_level_label);
		
		this->map_lock_level_minimum_textbox = std::make_shared<textbox>(*this);
		this->map_lock_level_minimum_textbox.get()->create(rectangle(rect.width - (level_width + control_indent + level_between_width + control_indent + level_width + TABPAGE_INDENT_RIGHT), image_offset + image_size + image_offset + control_height + control_indent, level_width, control_height), true, true);
		this->map_lock_level_minimum_textbox.get()->set_maximum_length(3);
		this->map_lock_level_minimum_textbox.get()->set_cue_banner("min");
		this->map_lock_level_minimum_textbox.get()->set_event(textbox_event::on_text_change, [this](std::string const& text) -> void
		{
			if (text.length() > 0)
			{
				try
				{
					if (std::stoi(text, 0, 10) > 250)
						this->map_lock_level_minimum_textbox.get()->set_text("250");
				}
				catch (std::exception& e)
				{
					UNREFERENCED_PARAMETER(e);
				}
			}
		});

		this->map_lock_level_maximum_textbox = std::make_shared<textbox>(*this);
		this->map_lock_level_maximum_textbox.get()->create(rectangle(rect.width - (level_width + TABPAGE_INDENT_RIGHT), image_offset + image_size + image_offset + control_height + control_indent, level_width, control_height), true, true);
		this->map_lock_level_maximum_textbox.get()->set_maximum_length(3);
		this->map_lock_level_maximum_textbox.get()->set_cue_banner("max");
		this->map_lock_level_maximum_textbox.get()->set_event(textbox_event::on_text_change, [this](std::string const& text) -> void
		{
			if (text.length() > 0)
			{
				try
				{
					if (std::stoi(text, 0, 10) > 250)
						this->map_lock_level_maximum_textbox.get()->set_text("250");
				}
				catch (std::exception& e)
				{
					UNREFERENCED_PARAMETER(e);
				}
			}
		});

		this->map_lock_list = std::make_shared<list_view>(*this);
		this->map_lock_list.get()->create(rectangle(TABPAGE_INDENT_LEFT, image_offset + image_size + image_offset + control_height + control_indent + control_height + STANDARD_INDENT, rect.width - (TABPAGE_INDENT_LEFT + TABPAGE_INDENT_RIGHT), 0), this->self);
		this->map_lock_list.get()->resize_list_view(false);
		
		this->map_lock_list.get()->add_column("#", LVIEW_INDENT + 20);
		this->map_lock_list.get()->add_column("Min. Lvl", LVIEW_INDENT + 65, true);
		this->map_lock_list.get()->add_column("Max. Lvl", LVIEW_INDENT + 65, true);
		this->map_lock_list.get()->add_column("X", LVIEW_INDENT + 55, true);
		this->map_lock_list.get()->add_column("Y", LVIEW_INDENT + 55, true);
		this->map_lock_list.get()->add_column("Map Id", LVIEW_INDENT + 100, true);
		this->map_lock_list.get()->add_column("Map Name", LVIEW_INDENT + 150);
		
		this->map_lock_list.get()->add_menu_item("Move Up", [this](int index) -> bool
		{
			if (index > 0)
			{
				this->map_lock_list.get()->swap_items(index, index - 1, [this](int index1, int index2) -> void
				{
					this->map_lock_list.get()->set_item_data(index1, 0, std::to_string(index1 + 1));
					this->map_lock_list.get()->set_item_data(index2, 0, std::to_string(index2 + 1));

					int current_selection = this->map_lock_list.get()->get_selection();
					this->map_lock_list.get()->set_selection(current_selection - 1);
				});
			}

			return true;
		});
		
		this->map_lock_list.get()->add_menu_item("Move Down", [this](int index) -> bool
		{
			if (index < this->map_lock_list.get()->get_item_count() - 1)
			{
				this->map_lock_list.get()->swap_items(index, index + 1, [this](int index1, int index2) -> void
				{
					this->map_lock_list.get()->set_item_data(index1, 0, std::to_string(index1 + 1));
					this->map_lock_list.get()->set_item_data(index2, 0, std::to_string(index2 + 1));

					int current_selection = this->map_lock_list.get()->get_selection();
					this->map_lock_list.get()->set_selection(current_selection + 1);
				});
			}
			
			return true;
		});
		
		this->map_lock_list.get()->add_menu_item("Remove Item", [this](int index) -> bool
		{
			this->map_lock_list.get()->remove_item(index);
			this->reset_list_numbering();
			return true;
		});

		this->map_lock_list.get()->add_menu_item("Clear Items", [this](int index) -> bool
		{
			this->map_lock_list.get()->clear_items();
			this->reset_list_numbering();
			return true;
		});
	}
	
	void spawncontrol_tabpage::reset_list_numbering()
	{
		for (int i = 0; i < this->map_lock_list.get()->get_item_count(); i++)
		{
			this->map_lock_list.get()->set_item_data(i, 0, std::to_string(i + 1));
		}
	}
}