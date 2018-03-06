#include "exclusive_tabpage.hpp"
#include "resource.hpp"

#include "button.hpp"
#include "label.hpp"

namespace firefly
{
	exclusive_tabpage::exclusive_tabpage(main_window* parent)
		: custom_tabpage(parent)
	{

	}

	exclusive_tabpage::~exclusive_tabpage()
	{

	}

	void exclusive_tabpage::save_to_profile(std::string const& profile_path)
	{
		this->profile_writer.set_path(profile_path);
		
		this->profile_writer.write_int("Exclusive", "PetLootCheck", this->pet_loot_togglebox.get()->get_check());
		this->profile_writer.write_int("Exclusive", "PetLootDelay", this->pet_loot_trackbar.get()->get_position());
		
		this->profile_writer.write_int("Exclusive", "FullMapAttackCheck", this->fullmap_attack_togglebox.get()->get_check());
		this->profile_writer.write_string("Exclusive", "FullMapAttackSkillId", this->fullmap_attack_id_textbox.get()->get_text());
		
		this->profile_writer.write_int("Exclusive", "SkillInjectCheck", this->skill_inject_togglebox.get()->get_check());
		this->profile_writer.write_string("Exclusive", "SkillInjectSkillId", this->skill_inject_id_textbox.get()->get_text());
		this->profile_writer.write_int("Exclusive", "SkillInjectDelay", this->skill_inject_delay_trackbar.get()->get_position());
		
		this->profile_writer.write_int("Exclusive", "DupeXCheck", this->dupex_togglebox.get()->get_check());
		this->profile_writer.write_string("Exclusive", "DupeXx", this->dupex_x_textbox.get()->get_text());
		this->profile_writer.write_string("Exclusive", "DupeXy", this->dupex_y_textbox.get()->get_text());
	}

	void exclusive_tabpage::load_from_profile(std::string const& profile_path)
	{
		this->profile_reader.set_path(profile_path);
		
		this->pet_loot_togglebox.get()->set_check(this->profile_reader.get_int("Exclusive", "PetLootCheck", FALSE) != FALSE);
		this->pet_loot_trackbar.get()->set_position(this->profile_reader.get_int("Exclusive", "PetLootDelay", 0), true);
		
		this->fullmap_attack_togglebox.get()->set_check(this->profile_reader.get_int("Exclusive", "FullMapAttackCheck", FALSE) != FALSE);
		this->fullmap_attack_id_textbox.get()->set_text(this->profile_reader.get_string("Exclusive", "FullMapAttackSkillId", ""));

		this->skill_inject_togglebox.get()->set_check(this->profile_reader.get_int("Exclusive", "SkillInjectCheck", FALSE) != FALSE);
		this->skill_inject_id_textbox.get()->set_text(this->profile_reader.get_string("Exclusive", "SkillInjectSkillId", ""));
		this->skill_inject_delay_trackbar.get()->set_position(this->profile_reader.get_int("Exclusive", "SkillInjectDelay", 0), true);
		
		this->dupex_togglebox.get()->set_check(this->profile_reader.get_int("Exclusive", "DupeXCheck", FALSE) != FALSE);
		this->dupex_x_textbox.get()->set_text(this->profile_reader.get_string("Exclusive", "DupeXx", ""));
		this->dupex_y_textbox.get()->set_text(this->profile_reader.get_string("Exclusive", "DupeXy", ""));
	}
	
	void exclusive_tabpage::toggle_functionality(bool checked)
	{
		net_buffer packet(trainer_tabpage_toggle);
		packet.write1(tabpage_ids::exclusive);
		packet.write1(-1);

		if (packet.write1(checked))
		{
			this->handle_type(exclusive_type::pet_loot, packet);
			this->handle_type(exclusive_type::fullmap_attack, packet);
			this->handle_type(exclusive_type::skill_inject, packet);
			this->handle_type(exclusive_type::dupex, packet);
		}

		this->send_game_packet(packet);
	}
	
	void exclusive_tabpage::on_fetch_dupex_data(net_buffer& packet)
	{
		this->dupex_x_textbox.get()->set_text(std::to_string(static_cast<int>(packet.read4())));
		this->dupex_y_textbox.get()->set_text(std::to_string(static_cast<int>(packet.read4())));
	}
	
	void exclusive_tabpage::create_controls(rectangle& rect)
	{
		/* section image */
		std::shared_ptr<button> section_image = std::make_shared<button>(*this);
		section_image.get()->create("", rectangle((rect.width / 2) - (image_size / 2), image_offset, image_size, image_size));
		section_image.get()->set_bitmap(png_button_exclusive);
		this->store_control(section_image);

		int small_textbox_width = ((control_width + key_indent + key_width) / 2) - (control_indent / 2);

		/* Pet Loot */
		int pet_loot_seperator_continue = this->create_seperator("Pet Loot", image_offset + image_size + image_offset);

		this->pet_loot_togglebox = std::make_unique<togglebox>(*this);
		this->pet_loot_togglebox.get()->create("Pet Loot", rectangle(TABPAGE_INDENT_LEFT, pet_loot_seperator_continue, control_width, control_height));
		this->pet_loot_togglebox.get()->set_tooltip("Teleports the users pet(s) to items around the map, so they can pick them up.");
		this->pet_loot_togglebox.get()->set_event(togglebox_event::on_state_change, std::bind(&exclusive_tabpage::toggle_func, this, exclusive_type::pet_loot, std::placeholders::_1));
		
		std::shared_ptr<label> pet_loot_every_label = std::make_shared<label>(*this);
		pet_loot_every_label.get()->create("every", rectangle(rect.width - (label_width + key_indent + control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), pet_loot_seperator_continue, label_width, control_height), true);
		this->store_control(pet_loot_every_label);

		this->pet_loot_trackbar = std::make_unique<trackbar>(*this);
		this->pet_loot_trackbar.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), pet_loot_seperator_continue + 3, control_width + key_indent + key_width, control_height));
		this->pet_loot_trackbar.get()->set_range(0, 1000);
		this->pet_loot_trackbar.get()->set_position(0);
		this->pet_loot_trackbar.get()->set_event(trackbar_event::on_drag, [this](short position) -> void
		{
			if (position == 0)
				this->pet_loot_textbox.get()->set_text("");
			else
				this->pet_loot_textbox.get()->set_text(std::to_string(position));
		});

		this->pet_loot_textbox = std::make_unique<textbox>(*this);
		this->pet_loot_textbox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), pet_loot_seperator_continue += (control_height + control_indent), control_width + key_indent + key_width, control_height), true);
		this->pet_loot_textbox.get()->set_cue_banner("milliseconds");
		this->pet_loot_textbox.get()->set_event(textbox_event::on_text_change, [this](std::string const& text) -> void
		{
			if (text.length() > 0)
			{
				int position = std::stoi(text, 0, 10);
				this->pet_loot_trackbar.get()->set_position(position);

				if (position < 0)
					this->pet_loot_textbox.get()->set_text("0");
				else if (position > 1000)
					this->pet_loot_textbox.get()->set_text("1000");
			}
			else
			{
				this->pet_loot_trackbar.get()->set_position(0);
			}
		});

		/* Fullmap Attack */
		static struct
		{
			int skill_id;
			std::string skill_description;
		} fullmap_attack_skills[] =
		{
			{ -1, "All Skills" },
			{ 21100018, "Aran\tFinal Charge (...)" },
			{ 112001006, "Beast Tamer\tMajestic Trumpet" },
			{ -2, "Blaze Wizard\tOrbital Flame" },
			{ 12100029, "Blaze Wizard\tIgnition" },
			{ 12120011, "Blaze Wizard\tBlazing Extinction" },
			{ -3, "Kinesis\tMetal Press, Trainwreck, B.P.M" },
			{ 142111002, "Kinesis\tPsychic Grab" },
			{ 27121100, "Luminous\tReflection" },
			{ 35121052, "Mechanic\tDistortion Bomb" },
			{ 33111013, "Wild Hunter\tHunting Assistant Unit" },
			{ 33121016, "Wild Hunter\tDrill Salvo" },
			{ 36110005, "Xenon\tTriangulation" },
			{ 37001000, "Blaster\tMagnum Punch" },
			{ 37001002, "Blaster\tBunker Buster Explosion" },
			{ 37101000, "Blaster\tDouble Blast" },
			{ 37111000, "Blaster\tHammer Smash" },
			{ 37110001, "Blaster\tHammer Smash (Charged)" },
			{ 37110002, "Blaster\tHammer Smash (Charged, DoT)" },
			{ 37111005, "Blaster\tRocket Rush (Grab)" },
			{ 37110011, "Blaster\tRocket Rush (Push back)" },
			{ 37110006, "Blaster\tMagnum Launch" },
			{ 37121000, "Blaster\tShotgun Punch" },
			{ 37120001, "Blaster\tShotgun Punch (Shockwave)" },
			{ 37120002, "Blaster\tMuzzle Flash" },
			{ 37121003, "Blaster\tBallistic Hurricane" },
			{ 37120022, "Blaster\tBallistic Hurricane (Uppercut)" },
			{ 37121004, "Blaster\tRevolving Blast" },
		};

		int fullmap_attack_seperator_continue = this->create_seperator("Fullmap Attack", pet_loot_seperator_continue + control_height + control_indent + STANDARD_INDENT);
		
		this->fullmap_attack_togglebox = std::make_unique<togglebox>(*this);
		this->fullmap_attack_togglebox.get()->create("Fullmap Attack", rectangle(TABPAGE_INDENT_LEFT, fullmap_attack_seperator_continue, control_width, control_height));
		this->fullmap_attack_togglebox.get()->set_tooltip("Allows your character to hit the entire map.");
		this->fullmap_attack_togglebox.get()->set_event(togglebox_event::on_state_change, std::bind(&exclusive_tabpage::toggle_func, this, exclusive_type::fullmap_attack, std::placeholders::_1));
		
		std::shared_ptr<label> fullmap_attack_using_label = std::make_shared<label>(*this);
		fullmap_attack_using_label.get()->create("using", rectangle(rect.width - (label_width + key_indent + control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), fullmap_attack_seperator_continue, label_width, control_height), true);
		this->store_control(fullmap_attack_using_label);
		
		this->fullmap_attack_id_textbox = std::make_unique<textbox>(*this);
		this->fullmap_attack_id_textbox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), fullmap_attack_seperator_continue, control_width + key_indent + key_width, control_height), true, true);
		this->fullmap_attack_id_textbox.get()->set_maximum_length(9);
		this->fullmap_attack_id_textbox.get()->set_cue_banner("skill id");
		this->fullmap_attack_id_textbox.get()->set_event(textbox_event::on_text_change, [this](std::string const& caption) -> void
		{
			int skill_count = this->fullmap_attack_id_combobox.get()->get_count();
			
			try
			{
				this->fullmap_attack_id_combobox.get()->set_selection(0, false);

				for (int i = 1, skill_id = std::stoi(caption, 0, 10); i < skill_count; i++)
				{
					if (skill_id == fullmap_attack_skills[i - 1].skill_id)
					{
						this->fullmap_attack_id_combobox.get()->set_selection(i, false);
						break;
					}
				}
			}
			catch (std::exception& e)
			{
#ifdef _DEBUG
				printf("An exception occured in exclusive_tabpage::lambda[skill_inject_id_textbox -> on_text_change] - %s\n", e.what());
#else
				UNREFERENCED_PARAMETER(e);
#endif
			}
		});
			
		this->fullmap_attack_id_combobox = std::make_unique<combobox>(*this);
		this->fullmap_attack_id_combobox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), fullmap_attack_seperator_continue += (control_height + control_indent), control_width + key_indent + key_width, control_height), true);
		this->fullmap_attack_id_combobox.get()->add_token_width(30);
		this->fullmap_attack_id_combobox.get()->add_token_width(70);
		this->fullmap_attack_id_combobox.get()->set_event(combobox_event::on_selection_change, [this](int index) -> void
		{
			if (index > 0)
				this->fullmap_attack_id_textbox.get()->set_text(std::to_string(fullmap_attack_skills[index - 1].skill_id));
			else
				this->fullmap_attack_id_textbox.get()->set_text("");
		});
		
		this->fullmap_attack_id_combobox.get()->add_item("Custom");

		for (int i = 0; i < _countof(fullmap_attack_skills); i++)
			this->fullmap_attack_id_combobox.get()->add_item(fullmap_attack_skills[i].skill_description);

		this->fullmap_attack_id_combobox.get()->set_selection(0);

		/* Skill Inject */
		static struct
		{
			unsigned int skill_id;
			std::string skill_description;
		} skill_inject_skills[] =
		{
			{ 21100018, "Aran\tFinal Charge (...)" },
			{ 21120024, "Aran\tBeyond Blade (...)" },
			{ 112001006, "Beast Tamer\tMajestic Trumpet" },
			{ 12100029, "Blaze Wizard\tIgnition" },
			{ 12120011, "Blaze Wizard\tBlazing Extinction" },
			{ 12121002, "Blaze Wizard\tTowering Inferno" },
			{ 142120030, "Kinesis\tMental Tempest" },
			{ 35121052, "Mechanic\tDistortion Bomb" },
			{ 25100002, "Shade\tGround Pound" },
			{ 25110003, "Shade\tShockwave Punch" },
			{ 36110005, "Xenon\tTriangulation" },
			{ 37110011, "Blaster\tRocket Rush (Push back)" },
			{ 37110006, "Blaster\tMagnum Launch" },
			{ 37121003, "Blaster\tBallistic Hurricane" },
			{ 37120022, "Blaster\tBallistic Hurricane (Uppercut)" },
		};

		int skill_inject_seperator_continue = this->create_seperator("Skill Injection", fullmap_attack_seperator_continue + control_height + control_indent + STANDARD_INDENT);
		
		this->skill_inject_togglebox = std::make_unique<togglebox>(*this);
		this->skill_inject_togglebox.get()->create("Skill Inject", rectangle(TABPAGE_INDENT_LEFT, skill_inject_seperator_continue, control_width, control_height));
		this->skill_inject_togglebox.get()->set_tooltip("Makes your character cast a given skill at a given delay.");
		this->skill_inject_togglebox.get()->set_event(togglebox_event::on_state_change, std::bind(&exclusive_tabpage::toggle_func, this, exclusive_type::skill_inject, std::placeholders::_1));
		
		std::shared_ptr<label> skill_inject_using_label = std::make_shared<label>(*this);
		skill_inject_using_label.get()->create("using", rectangle(rect.width - (label_width + key_indent + control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), skill_inject_seperator_continue, label_width, control_height), true);
		this->store_control(skill_inject_using_label);
		
		this->skill_inject_id_textbox = std::make_unique<textbox>(*this);
		this->skill_inject_id_textbox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), skill_inject_seperator_continue, control_width + key_indent + key_width, control_height), true, true);
		this->skill_inject_id_textbox.get()->set_maximum_length(9);
		this->skill_inject_id_textbox.get()->set_cue_banner("skill id");
		this->skill_inject_id_textbox.get()->set_event(textbox_event::on_text_change, [this](std::string const& caption) -> void
		{
			int skill_count = this->skill_inject_id_combobox.get()->get_count();
			
			try
			{
				this->skill_inject_id_combobox.get()->set_selection(0, false);

				for (int i = 1, skill_id = std::stoi(caption, 0, 10); i < skill_count; i++)
				{
					if (skill_id == skill_inject_skills[i - 1].skill_id)
					{
						this->skill_inject_id_combobox.get()->set_selection(i, false);
						break;
					}
				}
			}
			catch (std::exception& e)
			{
#ifdef _DEBUG
				printf("An exception occured in exclusive_tabpage::lambda[skill_inject_id_textbox -> on_text_change] - %s\n", e.what());
#else
				UNREFERENCED_PARAMETER(e);
#endif
			}
		});
			
		this->skill_inject_id_combobox = std::make_unique<combobox>(*this);
		this->skill_inject_id_combobox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), skill_inject_seperator_continue += (control_height + control_indent), control_width + key_indent + key_width, control_height), true);
		this->skill_inject_id_combobox.get()->add_token_width(30);
		this->skill_inject_id_combobox.get()->add_token_width(70);
		this->skill_inject_id_combobox.get()->add_item("Custom");
		
		for (int i = 0; i < _countof(skill_inject_skills); i++)
			this->skill_inject_id_combobox.get()->add_item(skill_inject_skills[i].skill_description);

		this->skill_inject_id_combobox.get()->set_event(combobox_event::on_selection_change, [this](int index) -> void
		{
			if (index > 0)
				this->skill_inject_id_textbox.get()->set_text(std::to_string(skill_inject_skills[index - 1].skill_id));
			else
				this->skill_inject_id_textbox.get()->set_text("");
		});
		
		this->skill_inject_id_combobox.get()->set_selection(0);
		
		std::shared_ptr<label> skill_inject_every_label = std::make_shared<label>(*this);
		skill_inject_every_label.get()->create("every", rectangle(rect.width - (label_width + key_indent + control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), (skill_inject_seperator_continue += (control_height + control_indent) + 2), label_width, control_height), true);
		this->store_control(skill_inject_every_label);
		
		this->skill_inject_delay_trackbar = std::make_unique<trackbar>(*this);
		this->skill_inject_delay_trackbar.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), skill_inject_seperator_continue + 3, control_width + key_indent + key_width, control_height));
		this->skill_inject_delay_trackbar.get()->set_range(0, 2000);
		this->skill_inject_delay_trackbar.get()->set_position(0);
		this->skill_inject_delay_trackbar.get()->set_event(trackbar_event::on_drag, [this](short position) -> void
		{
			if (position == 0)
				this->skill_inject_delay_textbox.get()->set_text("");
			else
				this->skill_inject_delay_textbox.get()->set_text(std::to_string(position));
		});
		
		this->skill_inject_delay_textbox = std::make_unique<textbox>(*this);
		this->skill_inject_delay_textbox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), skill_inject_seperator_continue += (control_height + control_indent), control_width + key_indent + key_width, control_height), true);
		this->skill_inject_delay_textbox.get()->set_cue_banner("milliseconds");
		this->skill_inject_delay_textbox.get()->set_event(textbox_event::on_text_change, [this](std::string const& text) -> void
		{
			if (text.length() > 0)
			{
				int position = std::stoi(text, 0, 10);
				this->skill_inject_delay_trackbar.get()->set_position(position);

				if (position < 0)
					this->skill_inject_delay_textbox.get()->set_text("0");
				else if (position > 2000)
					this->skill_inject_delay_textbox.get()->set_text("2000");
			}
			else
			{
				this->skill_inject_delay_trackbar.get()->set_position(0);
			}
		});
		
		/* DupeX */
		int dupex_seperator_continue = this->create_seperator("DupeX", skill_inject_seperator_continue + control_height + control_indent + STANDARD_INDENT);
		
		this->dupex_togglebox = std::make_unique<togglebox>(*this);
		this->dupex_togglebox.get()->create("DupeX", rectangle(TABPAGE_INDENT_LEFT, dupex_seperator_continue, control_width, control_height));
		this->dupex_togglebox.get()->set_tooltip("Vacs all mobs to a given position.");
		this->dupex_togglebox.get()->set_event(togglebox_event::on_state_change, std::bind(&exclusive_tabpage::toggle_func, this, exclusive_type::dupex, std::placeholders::_1));
		
		std::shared_ptr<label> dupex_at_label = std::make_shared<label>(*this);
		dupex_at_label.get()->create("at", rectangle(rect.width - (label_width + key_indent + control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), dupex_seperator_continue, label_width, control_height), true);
		this->store_control(dupex_at_label);
		
		this->dupex_x_textbox = std::make_unique<textbox>(*this);
		this->dupex_x_textbox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), dupex_seperator_continue, small_textbox_width, control_height), true);
		this->dupex_x_textbox.get()->set_cue_banner("x-coordinate");

		this->dupex_y_textbox = std::make_unique<textbox>(*this);
		this->dupex_y_textbox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT) + (control_indent + small_textbox_width), dupex_seperator_continue, small_textbox_width, control_height), true);
		this->dupex_y_textbox.get()->set_cue_banner("y-coordinate");
		
		std::shared_ptr<button> map_lock_fetch_map_id_button = std::make_shared<button>(*this);
		map_lock_fetch_map_id_button.get()->create("Fetch Character Coordinates", rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), dupex_seperator_continue += (control_height + control_indent), control_width + key_indent + key_width, control_height));
		map_lock_fetch_map_id_button.get()->set_event(button_event::on_click, [this]() -> bool
		{
			net_buffer packet(trainer_fetch_data);
			packet.write1(fetch_data_ids::dupex_data);

			if (!this->send_game_packet(packet))
				MessageBox(this->handle(), "You must be connected to the game client.", "Connection Error", MB_OK | MB_ICONERROR | MB_TOPMOST | MB_SETFOREGROUND);

			return true;
		});
		this->store_control(map_lock_fetch_map_id_button);
	}
	
	bool exclusive_tabpage::toggle_func(exclusive_type type, bool enabled)
	{
		if (this->toggled_on)
		{
			net_buffer packet(trainer_tabpage_toggle);
			packet.write1(tabpage_ids::exclusive);
			packet.write1(static_cast<unsigned char>(type));
			
			this->handle_type(type, packet, true, enabled);
			this->send_game_packet(packet);
		}

		return true;
	}

	void exclusive_tabpage::handle_type(exclusive_type type, net_buffer& packet, bool custom, bool enabled)
	{
		switch (type)
		{
		case exclusive_type::pet_loot:
			if (packet.write1(custom ? enabled : this->pet_loot_togglebox.get()->get_check()))
				packet.write2(this->pet_loot_trackbar.get()->get_position());

			break;

		case exclusive_type::fullmap_attack:
			if (packet.write1(custom ? enabled : this->fullmap_attack_togglebox.get()->get_check()))
				packet.write4(this->fullmap_attack_id_textbox.get()->get_integer());

			break;

		case exclusive_type::skill_inject:
			if (packet.write1(custom ? enabled : this->skill_inject_togglebox.get()->get_check()))
			{
				packet.write4(this->skill_inject_id_textbox.get()->get_integer());
				packet.write4(this->skill_inject_delay_trackbar.get()->get_position());
			}

			break;

		case exclusive_type::dupex:
			if (packet.write1(custom ? enabled : this->dupex_togglebox.get()->get_check()))
			{
				packet.write4(this->dupex_x_textbox.get()->get_integer());
				packet.write4(this->dupex_y_textbox.get()->get_integer());
			}

			break;

		default:
			break;
		}
	}
}