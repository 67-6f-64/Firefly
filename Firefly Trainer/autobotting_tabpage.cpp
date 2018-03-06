#include "autobotting_tabpage.hpp"
#include "resource.hpp"
#include "utility.hpp"

#include "button.hpp"

namespace firefly
{
	autobotting_tabpage::auto_macro_control::auto_macro_control(std::shared_ptr<togglebox> togglebox, std::shared_ptr<textbox> textbox, std::shared_ptr<combobox> combobox, bool has_combobox, unsigned int multiplier, unsigned int key)
		: togglebox_(togglebox), textbox_(textbox), combobox_(combobox), has_combobox_(has_combobox), key_(key), multiplier_(multiplier)
	{

	}

	autobotting_tabpage::auto_macro_control::~auto_macro_control()
	{

	}
	
	std::shared_ptr<togglebox> autobotting_tabpage::auto_macro_control::get_togglebox() const
	{
		return this->togglebox_;
	}

	std::shared_ptr<textbox> autobotting_tabpage::auto_macro_control::get_textbox() const
	{
		return this->textbox_;
	}

	std::shared_ptr<combobox> autobotting_tabpage::auto_macro_control::get_combobox() const
	{
		return this->combobox_;
	}
	
	bool autobotting_tabpage::auto_macro_control::has_combobox() const
	{
		return this->has_combobox_;
	}
	
	unsigned int autobotting_tabpage::auto_macro_control::multiplier() const
	{
		return this->multiplier_;
	}
	
	unsigned int autobotting_tabpage::auto_macro_control::get_key() const
	{
		return this->key_;
	}
	
	autobotting_tabpage::autobotting_tabpage(main_window* parent)
		: custom_tabpage(parent)
	{

	}

	autobotting_tabpage::~autobotting_tabpage()
	{

	}

	void autobotting_tabpage::save_to_profile(std::string const& profile_path)
	{
		this->profile_writer.set_path(profile_path);

		this->profile_writer.write_int("AutoBotting", "HpCheck", this->auto_hp_togglebox.get()->get_check());
		this->profile_writer.write_string("AutoBotting", "HpPercentage", this->auto_hp_textbox.get()->get_text());
		this->profile_writer.write_int("AutoBotting", "HpKey", this->auto_hp_combobox.get()->get_selection());

		this->profile_writer.write_int("AutoBotting", "MpCheck", this->auto_mp_togglebox.get()->get_check());
		this->profile_writer.write_string("AutoBotting", "MpPercentage", this->auto_mp_textbox.get()->get_text());
		this->profile_writer.write_int("AutoBotting", "MpKey", this->auto_mp_combobox.get()->get_selection());

		this->profile_writer.write_int("AutoBotting", "PetFeedCheck", this->auto_feed_pet_togglebox.get()->get_check());
		this->profile_writer.write_int("AutoBotting", "PetFeedValue", this->auto_feed_pet_trackbar.get()->get_position());
		this->profile_writer.write_int("AutoBotting", "PetFeedKey", this->auto_feed_pet_combobox.get()->get_selection());

		for (std::size_t i = 0; i < this->auto_macro_controls.size(); i++)
		{
			this->profile_writer.write_int("AutoBotting", "AutoMacro" + std::to_string(i) + "Check", this->auto_macro_controls.at(i).get_togglebox().get()->get_check());
			this->profile_writer.write_string("AutoBotting", "AutoMacro" + std::to_string(i) + "Value", this->auto_macro_controls.at(i).get_textbox().get()->get_text());
			this->profile_writer.write_int("AutoBotting", "AutoMacro" + std::to_string(i) + "Key", this->auto_macro_controls.at(i).get_combobox().get()->get_selection());
		}
		
		this->profile_writer.write_int("AutoBotting", "AutoAbilityPointCheck", this->auto_ability_points_togglebox.get()->get_check());
		this->profile_writer.write_int("AutoBotting", "AutoAbilityPointsValue", this->auto_ability_points_combobox.get()->get_selection());
		
		this->profile_writer.write_int("AutoBotting", "AutoFamiliarCheck", this->auto_familiar_togglebox.get()->get_check());
		this->profile_writer.write_int("AutoBotting", "AutoFamiliarCombo", this->auto_familiar_combobox.get()->get_selection());

		this->profile_writer.write_int("AutoBotting", "AutoJobMountCheck", this->auto_job_mount_togglebox.get()->get_check());
		this->profile_writer.write_int("AutoBotting", "AutoJobMountCombo", this->auto_job_mount_combobox.get()->get_selection());

		this->profile_writer.write_int("AutoBotting", "AutoEscapeCheck", this->auto_escape_togglebox.get()->get_check());
		this->profile_writer.write_string("AutoBotting", "AutoEscapeValue", this->auto_escape_textbox.get()->get_text());
		this->profile_writer.write_int("AutoBotting", "AutoEscapeCombo", this->auto_escape_combobox.get()->get_selection());

		this->profile_writer.write_int("AutoBotting", "AutoTimedEscapeCheck", this->auto_timed_escape_togglebox.get()->get_check());
		this->profile_writer.write_string("AutoBotting", "AutoTimedEscapeValue", this->auto_timed_escape_textbox.get()->get_text());
		this->profile_writer.write_int("AutoBotting", "AutoTimedEscapeCombo", this->auto_timed_escape_combobox.get()->get_selection());

		this->profile_writer.write_int("AutoBotting", "CustomChannelPoolCheck", this->custom_channel_pool_togglebox.get()->get_check());

		for (std::size_t i = 0; i < this->custom_channel_pool_container.size(); i++)
			this->profile_writer.write_int("AutoBotting", "CustomChannel" + std::to_string(i) + "Check", this->custom_channel_pool_container.at(i).get()->get_check());
		
		this->profile_writer.write_int("AutoBotting", "AllowNamesCount", this->allow_names_listview.get()->get_item_count());

		for (int i = 0, count = this->allow_names_listview.get()->get_item_count(); i < count; i++)
			this->profile_writer.write_string("AutoBotting", "AllowName" + std::to_string(i) + "Value", this->allow_names_listview.get()->get_item_data(i, 0));
	}
	
	void autobotting_tabpage::load_from_profile(std::string const& profile_path)
	{
		this->profile_reader.set_path(profile_path);
		
		this->auto_hp_togglebox.get()->set_check(this->profile_reader.get_int("AutoBotting", "HpCheck", FALSE) != FALSE);
		this->auto_hp_textbox.get()->set_text(this->profile_reader.get_string("AutoBotting", "HpPercentage", ""));
		this->auto_hp_combobox.get()->set_selection(this->profile_reader.get_int("AutoBotting", "HpKey", 0));

		this->auto_mp_togglebox.get()->set_check(this->profile_reader.get_int("AutoBotting", "MpCheck", FALSE) != FALSE);
		this->auto_mp_textbox.get()->set_text(this->profile_reader.get_string("AutoBotting", "MpPercentage", ""));
		this->auto_mp_combobox.get()->set_selection(this->profile_reader.get_int("AutoBotting", "MpKey", 0));

		this->auto_feed_pet_togglebox.get()->set_check(this->profile_reader.get_int("AutoBotting", "PetFeedCheck", FALSE) != FALSE);
		this->auto_feed_pet_trackbar.get()->set_position(this->profile_reader.get_int("AutoBotting", "PetFeedValue", 50), true);
		this->auto_feed_pet_combobox.get()->set_selection(this->profile_reader.get_int("AutoBotting", "PetFeedKey", 0));
		
		for (std::size_t i = 0; i < this->auto_macro_controls.size(); i++)
		{
			this->auto_macro_controls.at(i).get_togglebox().get()->set_check(this->profile_reader.get_int("AutoBotting", "AutoMacro" + std::to_string(i) + "Check", FALSE) != FALSE);
			this->auto_macro_controls.at(i).get_textbox().get()->set_text(this->profile_reader.get_string("AutoBotting", "AutoMacro" + std::to_string(i) + "Value", ""));
			this->auto_macro_controls.at(i).get_combobox().get()->set_selection(this->profile_reader.get_int("AutoBotting", "AutoMacro" + std::to_string(i) + "Key", 0));
		}
		
		this->auto_ability_points_togglebox.get()->set_check(this->profile_reader.get_int("AutoBotting", "AutoAbilityPointCheck", FALSE) != FALSE);
		this->auto_ability_points_combobox.get()->set_selection(this->profile_reader.get_int("AutoBotting", "AutoAbilityPointsValue", 0));
		
		this->auto_familiar_togglebox.get()->set_check(this->profile_reader.get_int("AutoBotting", "AutoFamiliarCheck", FALSE) != FALSE);
		this->auto_familiar_combobox.get()->set_selection(this->profile_reader.get_int("AutoBotting", "AutoFamiliarCombo", 0));
		
		this->auto_job_mount_togglebox.get()->set_check(this->profile_reader.get_int("AutoBotting", "AutoJobMountCheck", FALSE) != FALSE);
		this->auto_job_mount_combobox.get()->set_selection(this->profile_reader.get_int("AutoBotting", "AutoJobMountCombo", 0));
		
		this->auto_escape_togglebox.get()->set_check(this->profile_reader.get_int("AutoBotting", "AutoEscapeCheck", FALSE) != FALSE);
		this->auto_escape_textbox.get()->set_text(this->profile_reader.get_string("AutoBotting", "AutoEscapeValue", ""));
		this->auto_escape_combobox.get()->set_selection(this->profile_reader.get_int("AutoBotting", "AutoEscapeCombo", 0));

		this->auto_timed_escape_togglebox.get()->set_check(this->profile_reader.get_int("AutoBotting", "AutoTimedEscapeCheck", FALSE) != FALSE);
		this->auto_timed_escape_textbox.get()->set_text(this->profile_reader.get_string("AutoBotting", "AutoTimedEscapeValue", ""));
		this->auto_timed_escape_combobox.get()->set_selection(this->profile_reader.get_int("AutoBotting", "AutoTimedEscapeCombo", 0));

		this->custom_channel_pool_togglebox.get()->set_check(this->profile_reader.get_int("AutoBotting", "CustomChannelPoolCheck", FALSE) != FALSE);

		for (std::size_t i = 0; i < this->custom_channel_pool_container.size(); i++)
			this->custom_channel_pool_container.at(i).get()->set_check(this->profile_reader.get_int("AutoBotting", "CustomChannel" + std::to_string(i) + "Check", TRUE) != FALSE);
		
		for (int i = 0, count = this->profile_reader.get_int("AutoBotting", "AllowNamesCount", 0); i < count; i++)
			this->allow_names_listview.get()->add_item_manual(this->profile_reader.get_string("AutoBotting", "AllowName" + std::to_string(i) + "Value", "?"));
		
		this->allow_names_listview.get()->resize_list_view(true);
	}
	
	void autobotting_tabpage::toggle_functionality(bool checked)
	{
		net_buffer packet(trainer_tabpage_toggle);
		packet.write1(tabpage_ids::autobotting);
		packet.write1(-1);

		if (packet.write1(checked))
		{
			this->handle_type(botting_type::auto_hp, packet);
			this->handle_type(botting_type::auto_mp, packet);
			this->handle_type(botting_type::auto_feed_pet, packet);

			for (std::size_t i = 0; i < this->auto_macro_controls.size(); i++)
				this->handle_type(static_cast<botting_type>(static_cast<int>(botting_type::auto_loot) + i), packet);

			this->handle_type(botting_type::auto_ability_points, packet);
			this->handle_type(botting_type::auto_familiar, packet);
			this->handle_type(botting_type::auto_job_mount, packet);
			this->handle_type(botting_type::auto_evade, packet);
			this->handle_type(botting_type::auto_refresh, packet);
			this->handle_type(botting_type::custom_channel_pool, packet);
			this->handle_type(botting_type::allow_names, packet);
		}

		this->send_game_packet(packet);
	}
	
	void autobotting_tabpage::create_controls(rectangle& rect)
	{
		/* section image */
		std::shared_ptr<button> section_image = std::make_shared<button>(*this);
		section_image.get()->create("", rectangle((rect.width / 2) - (image_size / 2), image_offset, image_size, image_size));
		section_image.get()->set_bitmap(png_button_autobotting);
		this->store_control(section_image);
		
		int small_textbox_width = ((control_width + key_indent + key_width) / 2) - (control_indent / 2);
		
		this->key_entries.push_back(std::make_pair("Page Up", VK_PRIOR));
		this->key_entries.push_back(std::make_pair("Page Down", VK_NEXT));
		this->key_entries.push_back(std::make_pair("Delete", VK_DELETE));
		this->key_entries.push_back(std::make_pair("Insert", VK_INSERT));
		this->key_entries.push_back(std::make_pair("Home", VK_HOME));
		this->key_entries.push_back(std::make_pair("End", VK_END));
		this->key_entries.push_back(std::make_pair("Control", VK_LCONTROL));
		this->key_entries.push_back(std::make_pair("Shift", VK_LSHIFT));
		this->key_entries.push_back(std::make_pair("Alt", VK_MENU));

		for (int i = 0, A = 0x41; i < ('Z' - 'A') + 1; i++)
		{
			char key_string[] = { 'A' + i, '\0' };
			this->key_entries.push_back(std::make_pair(key_string, A + i));
		}

		/* Auto Pot */
		int auto_pot_seperator_continue = this->create_seperator("Auto Potion", image_offset + image_size + image_offset);
		
		this->auto_hp_togglebox = std::make_unique<togglebox>(*this);
		this->auto_hp_togglebox.get()->create("Use Health Potion", rectangle(TABPAGE_INDENT_LEFT, auto_pot_seperator_continue, control_width, control_height + 2));
		this->auto_hp_togglebox.get()->set_tooltip("Uses a health potion everytime your health goes below a certain %.");
		this->auto_hp_togglebox.get()->set_event(togglebox_event::on_state_change, std::bind(&autobotting_tabpage::toggle_func, this, botting_type::auto_hp, std::placeholders::_1));
		
		std::shared_ptr<label> auto_hp_when_less_label = std::make_shared<label>(*this);
		auto_hp_when_less_label.get()->create("when <", rectangle(rect.width - (label_width + key_indent + control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_pot_seperator_continue, label_width, control_height + 2), true);
		this->store_control(auto_hp_when_less_label);

		this->auto_hp_textbox = std::make_unique<textbox>(*this);
		this->auto_hp_textbox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_pot_seperator_continue, small_textbox_width, control_height + 2), true, true);
		this->auto_hp_textbox.get()->set_maximum_length(40);
		this->auto_hp_textbox.get()->set_cue_banner("%");
		this->auto_hp_textbox.get()->set_event(textbox_event::on_text_change, [this](std::string const& text) -> void
		{
			if (text.length() > 0)
			{
				if (std::stoi(text, 0, 10) > 100)
					this->auto_hp_textbox.get()->set_text("100");
			}
		});
		
		this->auto_hp_combobox = std::make_unique<combobox>(*this);
		this->auto_hp_combobox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT) + (control_indent + small_textbox_width), auto_pot_seperator_continue, small_textbox_width, control_height), true);
		
		for (auto entry : this->key_entries)
			auto_hp_combobox.get()->add_item(entry.first);

		this->auto_hp_combobox.get()->set_selection(0);

		this->auto_mp_togglebox = std::make_unique<togglebox>(*this);
		this->auto_mp_togglebox.get()->create("Use Mana Potion", rectangle(TABPAGE_INDENT_LEFT, auto_pot_seperator_continue += ((control_height + control_indent) + 2), control_width, control_height + 2));
		this->auto_mp_togglebox.get()->set_tooltip("Uses a mana potion everytime your mana goes below a certain %.");
		this->auto_mp_togglebox.get()->set_event(togglebox_event::on_state_change, std::bind(&autobotting_tabpage::toggle_func, this, botting_type::auto_mp, std::placeholders::_1));
		
		std::shared_ptr<label> auto_mp_when_less_label = std::make_shared<label>(*this);
		auto_mp_when_less_label.get()->create("when <", rectangle(rect.width - (label_width + key_indent + control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_pot_seperator_continue, label_width, control_height + 2), true);
		this->store_control(auto_mp_when_less_label);

		this->auto_mp_textbox = std::make_unique<textbox>(*this);
		this->auto_mp_textbox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_pot_seperator_continue, small_textbox_width, control_height + 2), true, true);
		this->auto_mp_textbox.get()->set_maximum_length(40);
		this->auto_mp_textbox.get()->set_cue_banner("%");
		this->auto_mp_textbox.get()->set_event(textbox_event::on_text_change, [this](std::string const& text) -> void
		{
			if (text.length() > 0)
			{
				if (std::stoi(text, 0, 10) > 100)
					this->auto_mp_textbox.get()->set_text("100");
			}
		});
		
		this->auto_mp_combobox = std::make_unique<combobox>(*this);
		this->auto_mp_combobox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT) + (control_indent + small_textbox_width), auto_pot_seperator_continue, small_textbox_width, control_height), true);
		
		for (auto entry : this->key_entries)
			auto_mp_combobox.get()->add_item(entry.first);

		this->auto_mp_combobox.get()->set_selection(0);

		/* Auto Pet Feed */
		int auto_pet_feed_seperator_continue = this->create_seperator("Auto Feed Pet", auto_pot_seperator_continue + control_height + control_indent + STANDARD_INDENT);

		this->auto_feed_pet_togglebox = std::make_unique<togglebox>(*this);
		this->auto_feed_pet_togglebox.get()->create("Use Pet Food when fullness <", rectangle(TABPAGE_INDENT_LEFT, auto_pet_feed_seperator_continue, control_width, control_height + 2));
		this->auto_feed_pet_togglebox.get()->set_tooltip("Uses a pet food everytime one of your pets' fullness goes below a certain value.");
		this->auto_feed_pet_togglebox.get()->set_event(togglebox_event::on_state_change, std::bind(&autobotting_tabpage::toggle_func, this, botting_type::auto_feed_pet, std::placeholders::_1));

		this->auto_feed_pet_label = std::make_unique<label>(*this);
		this->auto_feed_pet_label.get()->create("50", rectangle(rect.width - (label_width + key_indent + control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_pet_feed_seperator_continue, label_width, control_height + 2), true);
		this->auto_feed_pet_label.get()->set_foreground(RGB(255, 255, 255));

		this->auto_feed_pet_trackbar = std::make_unique<trackbar>(*this);
		this->auto_feed_pet_trackbar.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_pet_feed_seperator_continue + 4, small_textbox_width, control_height + 2));
		this->auto_feed_pet_trackbar.get()->set_range(0, 100);
		this->auto_feed_pet_trackbar.get()->set_position(50);
		this->auto_feed_pet_trackbar.get()->set_event(trackbar_event::on_drag, [this](short position) -> void
		{
			this->auto_feed_pet_label.get()->set_text(std::to_string(position));
		});
		
		this->auto_feed_pet_combobox = std::make_unique<combobox>(*this);
		this->auto_feed_pet_combobox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT) + (control_indent + small_textbox_width), auto_pet_feed_seperator_continue, small_textbox_width, control_height), true);
		
		for (auto entry : this->key_entries)
			auto_feed_pet_combobox.get()->add_item(entry.first);

		this->auto_feed_pet_combobox.get()->set_selection(0);

		/* Auto Macro */
		int auto_macro_seperator_continue = this->create_seperator("Auto Macro", auto_pet_feed_seperator_continue + control_height + control_indent + STANDARD_INDENT);
		
		struct 
		{
			const std::string macro_name;
			const std::string delay_type;
			const unsigned int multiplier;
			const bool has_combobox;
		} auto_macros[] =
		{
			{ "Auto Loot", "milliseconds", 1, false },
			{ "Auto Attack", "milliseconds", 1, true },
			{ "Auto Timed Skill (1)", "seconds", 1000, true },
			{ "Auto Timed Skill (2)", "seconds", 1000, true },
			{ "Auto Timed Skill (3)", "seconds", 1000, true },
			{ "Auto Timed Skill (4)", "seconds", 1000, true },
		};

		for (int i = 0; i < _countof(auto_macros); i++, auto_macro_seperator_continue += ((control_height + control_indent) + 2))
		{
			std::shared_ptr<togglebox> macro_togglebox = std::make_shared<togglebox>(*this);
			macro_togglebox.get()->create(auto_macros[i].macro_name, rectangle(TABPAGE_INDENT_LEFT, auto_macro_seperator_continue, control_width, control_height));
			macro_togglebox.get()->set_event(togglebox_event::on_state_change, std::bind(&autobotting_tabpage::toggle_func, this, static_cast<botting_type>(static_cast<int>(botting_type::auto_loot) + i), std::placeholders::_1));
			
			std::shared_ptr<label> auto_macro_every_label = std::make_shared<label>(*this);
			auto_macro_every_label.get()->create("every", rectangle(rect.width - (label_width + key_indent + control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_macro_seperator_continue, label_width, control_height + 2), true);
			this->store_control(auto_macro_every_label);

			std::shared_ptr<textbox> macro_textbox = std::make_shared<textbox>(*this);
			macro_textbox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_macro_seperator_continue, small_textbox_width, control_height + 2), true, true);
			macro_textbox.get()->set_cue_banner(auto_macros[i].delay_type);
			macro_textbox.get()->set_maximum_length(6);
			
			std::shared_ptr<combobox> auto_macro_key_combobox = std::make_shared<combobox>(*this);
			auto_macro_key_combobox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT) + (control_indent + small_textbox_width), auto_macro_seperator_continue, small_textbox_width, control_height), true);
				
			for (auto entry : this->key_entries)
				auto_macro_key_combobox.get()->add_item(entry.first);

			auto_macro_key_combobox.get()->set_selection(0);
			
			if (!auto_macros[i].has_combobox)
				auto_macro_key_combobox.get()->show(false);

			this->auto_macro_controls.push_back(auto_macro_control(macro_togglebox, macro_textbox, auto_macro_key_combobox, auto_macros[i].has_combobox, auto_macros[i].multiplier));
		}

		/* Auto Distribution */
		int auto_distribution_seperator_continue = this->create_seperator("Auto Distribution", auto_macro_seperator_continue + STANDARD_INDENT);
		
		this->auto_ability_points_togglebox = std::make_unique<togglebox>(*this);
		this->auto_ability_points_togglebox.get()->create("Auto Distribute Ability Points", rectangle(TABPAGE_INDENT_LEFT, auto_distribution_seperator_continue + 1, control_width, control_height));
		this->auto_ability_points_togglebox.get()->set_tooltip("Automatically distributes your ability-points upon leveling up.");
		this->auto_ability_points_togglebox.get()->set_event(togglebox_event::on_state_change, std::bind(&autobotting_tabpage::toggle_func, this, botting_type::auto_ability_points, std::placeholders::_1));

		std::shared_ptr<label> auto_ability_points_using_label = std::make_shared<label>(*this);
		auto_ability_points_using_label.get()->create("using", rectangle(rect.width - (label_width + key_indent + control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_distribution_seperator_continue, label_width, control_height), true);
		this->store_control(auto_ability_points_using_label);

		this->auto_ability_points_combobox = std::make_unique<combobox>(*this);
		this->auto_ability_points_combobox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_distribution_seperator_continue, control_width + key_indent + key_width, control_height), true);
		this->auto_ability_points_combobox.get()->add_item("Regular");
		this->auto_ability_points_combobox.get()->add_item("Pirate (1st Job -> Brawler)");
		this->auto_ability_points_combobox.get()->add_item("Pirate (1st Job -> Gunslinger)");
		this->auto_ability_points_combobox.get()->add_item("Xenon (STR)");
		this->auto_ability_points_combobox.get()->add_item("Xenon (DEX)");
		this->auto_ability_points_combobox.get()->add_item("Xenon (LUK)");
		this->auto_ability_points_combobox.get()->set_selection(0);
		
		/* Auto Familiar */
		int auto_familiar_seperator_continue = this->create_seperator("Auto Familiar", auto_distribution_seperator_continue + control_height + control_indent + STANDARD_INDENT);
		
		this->auto_familiar_togglebox = std::make_unique<togglebox>(*this);
		this->auto_familiar_togglebox.get()->create("Auto Familiar", rectangle(TABPAGE_INDENT_LEFT, auto_familiar_seperator_continue, control_width, control_height));
		this->auto_familiar_togglebox.get()->set_tooltip("Automatically summons your familiar, whenever it's not present.");
		this->auto_familiar_togglebox.get()->set_event(togglebox_event::on_state_change, std::bind(&autobotting_tabpage::toggle_func, this, botting_type::auto_familiar, std::placeholders::_1));
		
		std::shared_ptr<label> auto_familiar_using_label = std::make_shared<label>(*this);
		auto_familiar_using_label.get()->create("using", rectangle(rect.width - (label_width + key_indent + control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_familiar_seperator_continue, label_width, control_height), true);
		this->store_control(auto_familiar_using_label);

		this->auto_familiar_combobox = std::make_unique<combobox>(*this);
		this->auto_familiar_combobox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_familiar_seperator_continue, control_width + key_indent + key_width, control_height), true);
		this->auto_familiar_combobox.get()->add_item("Jr Boogie (+15% MP Regen)");
		this->auto_familiar_combobox.get()->add_item("Big Spider (+60% Item/Meso)");
		this->auto_familiar_combobox.get()->add_item("Leprechaun (+100% Meso Drop)");
		this->auto_familiar_combobox.get()->add_item("Eye of Time (+100% Meso Drop)");
		this->auto_familiar_combobox.get()->add_item("Mutant Snail (+15% MP + HP Regen)");
		this->auto_familiar_combobox.get()->add_item("Wolf Underling (+100% Item Drop)");
		this->auto_familiar_combobox.get()->add_item("Mutant Ribbon Pig (+60% Item/Meso Drop)");
		this->auto_familiar_combobox.get()->set_selection(0);
		
		/* Auto Job */
		int auto_job_specifics_seperator_continue = this->create_seperator("Auto Job Specifics", auto_familiar_seperator_continue + control_height + control_indent + STANDARD_INDENT);
		
		this->auto_job_mount_togglebox = std::make_unique<togglebox>(*this);
		this->auto_job_mount_togglebox.get()->create("Auto Job Mount", rectangle(TABPAGE_INDENT_LEFT, auto_job_specifics_seperator_continue + 1, control_width, control_height));
		this->auto_job_mount_togglebox.get()->set_tooltip("Automatically mounts your character, whenever he's not mounted.");
		this->auto_job_mount_togglebox.get()->set_event(togglebox_event::on_state_change, std::bind(&autobotting_tabpage::toggle_func, this, botting_type::auto_job_mount, std::placeholders::_1));
		
		std::shared_ptr<label> auto_job_mount_using_label = std::make_shared<label>(*this);
		auto_job_mount_using_label.get()->create("using", rectangle(rect.width - (label_width + key_indent + control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_job_specifics_seperator_continue, label_width, control_height), true);
		this->store_control(auto_job_mount_using_label);

		this->auto_job_mount_combobox = std::make_unique<combobox>(*this);
		this->auto_job_mount_combobox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_job_specifics_seperator_continue, control_width + key_indent + key_width, control_height), true);
		this->auto_job_mount_combobox.get()->add_item("Wild Hunter - Jaguar Riding");
		this->auto_job_mount_combobox.get()->add_item("Mechanic - Humanoid Mech");
		this->auto_job_mount_combobox.get()->add_item("Mechanic - Tank Mech");
		this->auto_job_mount_combobox.get()->set_selection(0);

		/* Auto Change Plane */
		int auto_change_plane_seperator_continue = this->create_seperator("Auto Change Plane", auto_job_specifics_seperator_continue + control_height + control_indent + STANDARD_INDENT);
		
		this->auto_escape_togglebox = std::make_unique<togglebox>(*this);
		this->auto_escape_togglebox.get()->create("Auto Evade", rectangle(TABPAGE_INDENT_LEFT, auto_change_plane_seperator_continue, control_width, control_height));
		this->auto_escape_togglebox.get()->set_event(togglebox_event::on_state_change, std::bind(&autobotting_tabpage::toggle_func, this, botting_type::auto_evade, std::placeholders::_1));
		
		std::shared_ptr<label> auto_escape_when_greater_label = std::make_shared<label>(*this);
		auto_escape_when_greater_label.get()->create("when >", rectangle(rect.width - (label_width + key_indent + control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_change_plane_seperator_continue, label_width, control_height), true);
		this->store_control(auto_escape_when_greater_label);
		
		this->auto_escape_textbox = std::make_unique<textbox>(*this);
		this->auto_escape_textbox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_change_plane_seperator_continue, control_width + key_indent + key_width, control_height), true, true);
		this->auto_escape_textbox.get()->set_maximum_length(2);
		this->auto_escape_textbox.get()->set_cue_banner("people (detects GMs at '0')");
		
		std::shared_ptr<label> auto_escape_using_label = std::make_shared<label>(*this);
		auto_escape_using_label.get()->create("using", rectangle(rect.width - (label_width + key_indent + control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_change_plane_seperator_continue += (control_height + control_indent), label_width, control_height), true);
		this->store_control(auto_escape_using_label);
		
		this->auto_escape_combobox = std::make_unique<combobox>(*this);
		this->auto_escape_combobox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_change_plane_seperator_continue, control_width + key_indent + key_width, control_height), true);
		this->auto_escape_combobox.get()->add_item("Stop botting");
		this->auto_escape_combobox.get()->add_item("Change channel (wait for breath)");
		this->auto_escape_combobox.get()->add_item("CashShop (wait for breath)");
		//this->auto_escape_combobox.get()->add_item("Disconnect");
		this->auto_escape_combobox.get()->set_selection(0);
		
		this->auto_timed_escape_togglebox = std::make_unique<togglebox>(*this);
		this->auto_timed_escape_togglebox.get()->create("Auto Refresh", rectangle(TABPAGE_INDENT_LEFT, auto_change_plane_seperator_continue += ((control_height + control_indent) + 2), control_width, control_height));
		this->auto_timed_escape_togglebox.get()->set_event(togglebox_event::on_state_change, std::bind(&autobotting_tabpage::toggle_func, this, botting_type::auto_refresh, std::placeholders::_1));
		
		std::shared_ptr<label> auto_timed_escape_every_label = std::make_shared<label>(*this);
		auto_timed_escape_every_label.get()->create("every", rectangle(rect.width - (label_width + key_indent + control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_change_plane_seperator_continue, label_width, control_height), true);
		this->store_control(auto_timed_escape_every_label);
		
		this->auto_timed_escape_textbox = std::make_unique<textbox>(*this);
		this->auto_timed_escape_textbox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_change_plane_seperator_continue, control_width + key_indent + key_width, control_height), true, true);
		this->auto_timed_escape_textbox.get()->set_maximum_length(4);
		this->auto_timed_escape_textbox.get()->set_cue_banner("seconds");
		
		std::shared_ptr<label> auto_timed_escape_using_label = std::make_shared<label>(*this);
		auto_escape_using_label.get()->create("using", rectangle(rect.width - (label_width + key_indent + control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_change_plane_seperator_continue += (control_height + control_indent), label_width, control_height), true);
		this->store_control(auto_escape_using_label);
		
		this->auto_timed_escape_combobox = std::make_unique<combobox>(*this);
		this->auto_timed_escape_combobox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_change_plane_seperator_continue, control_width + key_indent + key_width, control_height), true);
		this->auto_timed_escape_combobox.get()->add_item("Change channel (wait for breath)");
		this->auto_timed_escape_combobox.get()->add_item("CashShop (wait for breath)");

		this->custom_channel_pool_togglebox = std::make_unique<togglebox>(*this);
		this->custom_channel_pool_togglebox.get()->create("Custom Channel Pool", rectangle(TABPAGE_INDENT_LEFT, auto_change_plane_seperator_continue += ((control_height + control_indent) + 2), 150, control_height));
		this->custom_channel_pool_togglebox.get()->set_event(togglebox_event::on_state_change, std::bind(&autobotting_tabpage::toggle_func, this, botting_type::custom_channel_pool, std::placeholders::_1));
		this->custom_channel_pool_togglebox.get()->set_check(true);

		for (int i = 0; i < 20; i++)
		{
			const int togglebox_width = 16;

			int x = rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT);
			int cx = (rect.width - TABPAGE_INDENT_RIGHT) - x;
			int dx = ((i % 10) * ((cx + togglebox_width) / 10));

			int y = auto_change_plane_seperator_continue;
			int dy = ((i / 10) * (control_height * 2));

			std::shared_ptr<togglebox> custom_channel_togglebox = std::make_shared<togglebox>(*this);
			custom_channel_togglebox.get()->create("", rectangle(x + dx, y + dy, togglebox_width, control_height));
			custom_channel_togglebox.get()->set_check(true);
			this->custom_channel_pool_container.push_back(custom_channel_togglebox);
			
			std::shared_ptr<label> custom_channel_label = std::make_shared<label>(*this);
			custom_channel_label.get()->create(std::to_string(i + 1), rectangle(x + dx, y + dy + control_height, togglebox_width, control_height), true);
			this->store_control(custom_channel_label);
		}
		
		auto_change_plane_seperator_continue += ((control_height * 2) * 2);
		
		this->allow_names = std::make_unique<togglebox>(*this);
		this->allow_names.get()->create("Allow Names", rectangle(TABPAGE_INDENT_LEFT, auto_change_plane_seperator_continue += control_indent, control_width, control_height));
		this->allow_names.get()->set_event(togglebox_event::on_state_change, std::bind(&autobotting_tabpage::toggle_func, this, botting_type::allow_names, std::placeholders::_1));
		
		this->allow_names_textbox = std::make_unique<textbox>(*this);
		this->allow_names_textbox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_change_plane_seperator_continue, control_width + key_indent + key_width, control_height), true);
		this->allow_names_textbox.get()->set_cue_banner("character name");
		
		std::shared_ptr<button> add_name_button = std::make_shared<button>(*this);
		add_name_button.get()->create("", rectangle(rect.width - (label_width + key_indent + control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_change_plane_seperator_continue - 1, label_width, 24));
		add_name_button.get()->set_tooltip("Add name to the whitelist");
		add_name_button.get()->set_bitmap(png_button_add);
		add_name_button.get()->set_event(button_event::on_click, [this]() -> bool
		{
			if (this->allow_names_textbox.get()->get_text_length() <= 0)
			{
				MessageBox(this->handle(), "You must enter a character name.", "Syntax Error", MB_OK | MB_ICONERROR | MB_TOPMOST | MB_SETFOREGROUND);
				return false;
			}

			this->allow_names_listview.get()->add_item(utility::lower(this->allow_names_textbox.get()->get_text()));
			return true;
		});
		this->store_control(add_name_button);

		this->allow_names_listview = std::make_unique<list_view>(*this);
		this->allow_names_listview.get()->create(rectangle(TABPAGE_INDENT_LEFT, auto_change_plane_seperator_continue += (control_height + control_indent), rect.width - (TABPAGE_INDENT_LEFT + TABPAGE_INDENT_RIGHT), 0), this->self);
		this->allow_names_listview.get()->resize_list_view(false);
		
		this->allow_names_listview.get()->add_column("Allowed Character Names", rect.width - (TABPAGE_INDENT_LEFT + TABPAGE_INDENT_RIGHT), true);
		
		this->allow_names_listview.get()->add_menu_item("Remove Item", [this](int index) -> bool 
		{ 
			this->allow_names_listview.get()->remove_item(index);
			return true;
		});

		this->allow_names_listview.get()->add_menu_item("Clear Items", [this](int index) -> bool 
		{ 
			this->allow_names_listview.get()->clear_items();
			return true;
		});
	}
	
	bool autobotting_tabpage::toggle_func(botting_type type, bool enabled)
	{
		if (this->toggled_on)
		{
			net_buffer packet(trainer_tabpage_toggle);
			packet.write1(tabpage_ids::autobotting);
			packet.write1(static_cast<unsigned char>(type));
			
			this->handle_type(type, packet, true, enabled);
			this->send_game_packet(packet);
		}

		return true;
	}

	void autobotting_tabpage::handle_type(botting_type type, net_buffer& packet, bool custom, bool enabled)
	{
		switch (type)
		{
		case botting_type::auto_hp:
			if (packet.write1(custom ? enabled : this->auto_hp_togglebox.get()->get_check()))
			{
				packet.write4(this->auto_hp_textbox.get()->get_integer());
				packet.write4(this->key_entries.at(this->auto_hp_combobox.get()->get_selection()).second);
			}

			break;

		case botting_type::auto_mp:
			if (packet.write1(custom ? enabled : this->auto_mp_togglebox.get()->get_check()))
			{
				packet.write4(this->auto_mp_textbox.get()->get_integer());
				packet.write4(this->key_entries.at(this->auto_mp_combobox.get()->get_selection()).second);
			}
			
			break;

		case botting_type::auto_feed_pet:
			if (packet.write1(custom ? enabled : this->auto_feed_pet_togglebox.get()->get_check()))
			{
				packet.write4(this->auto_feed_pet_trackbar.get()->get_position());
				packet.write4(this->key_entries.at(this->auto_feed_pet_combobox.get()->get_selection()).second);
			}
			
			break;

		case botting_type::auto_loot:
		case botting_type::auto_attack:
		case botting_type::auto_timed_skill_1:
		case botting_type::auto_timed_skill_2:
		case botting_type::auto_timed_skill_3:
		case botting_type::auto_timed_skill_4:
			{
				int index = static_cast<int>(type) - static_cast<int>(botting_type::auto_loot);

				if (packet.write1(custom ? enabled : this->auto_macro_controls.at(index).get_togglebox().get()->get_check()))
				{
					packet.write4(this->auto_macro_controls.at(index).get_textbox().get()->get_integer() * this->auto_macro_controls.at(index).multiplier());
					packet.write4(this->key_entries.at(this->auto_macro_controls.at(index).get_combobox().get()->get_selection()).second);
				}
			}
			break;

		case botting_type::auto_ability_points:
			if (packet.write1(custom ? enabled : this->auto_ability_points_togglebox.get()->get_check()))
				packet.write1(this->auto_ability_points_combobox.get()->get_selection());
			
			break;

		case botting_type::auto_familiar:
			if (packet.write1(custom ? enabled : this->auto_familiar_togglebox.get()->get_check()))
				packet.write1(this->auto_familiar_combobox.get()->get_selection());
			
			break;

		case botting_type::auto_job_mount:	
			if (packet.write1(custom ? enabled : this->auto_job_mount_togglebox.get()->get_check()))
				packet.write1(this->auto_job_mount_combobox.get()->get_selection());
			
			break;

		case botting_type::auto_evade:
			if (packet.write1(custom ? enabled : this->auto_escape_togglebox.get()->get_check()))
			{
				packet.write4(this->auto_escape_textbox.get()->get_integer());
				packet.write1(this->auto_escape_combobox.get()->get_selection());
			}
			
			break;

		case botting_type::auto_refresh:
			if (packet.write1(custom ? enabled : this->auto_timed_escape_togglebox.get()->get_check()))
			{
				packet.write4(this->auto_timed_escape_textbox.get()->get_integer());
				packet.write1(this->auto_timed_escape_combobox.get()->get_selection());
			}
			
			break;

		case botting_type::custom_channel_pool:
			if (packet.write1(custom ? enabled : this->custom_channel_pool_togglebox.get()->get_check()))
			{
				if (packet.write1(this->custom_channel_pool_container.size()) > 0)
				{
					for (std::shared_ptr<togglebox> custom_channel : this->custom_channel_pool_container)
						packet.write1(custom_channel.get()->get_check());
				}
			}
			
			break;

		case botting_type::allow_names:
			if (packet.write1(custom ? enabled : this->allow_names.get()->get_check()))
			{
				unsigned int name_count = packet.write4(this->allow_names_listview.get()->get_item_count());

				if (name_count > 0)
				{
					for (unsigned int i = 0; i < name_count; i++)
						packet.write_string(this->allow_names_listview.get()->get_item_data(i, 0));
				}
			}

		default:
			break;
		}
	}
}