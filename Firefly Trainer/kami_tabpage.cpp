#include "kami_tabpage.hpp"
#include "resource.hpp"

#include "button.hpp"
#include "label.hpp"

namespace firefly
{
	kami_tabpage::kami_tabpage(main_window* parent)
		: custom_tabpage(parent)
	{

	}

	kami_tabpage::~kami_tabpage()
	{

	}

	void kami_tabpage::save_to_profile(std::string const& profile_path)
	{
		this->profile_writer.set_path(profile_path);
		
		this->profile_writer.write_int("Kami", "RangeVertical", this->vertical_range_trackbar.get()->get_position());
		this->profile_writer.write_int("Kami", "RangeHorizontal", this->horizontal_range_trackbar.get()->get_position());

		this->profile_writer.write_int("Kami", "KamiMovementType", this->kami_type_combobox.get()->get_selection());
		this->profile_writer.write_int("Kami", "KamiTargetMethod", this->kami_target_combobox.get()->get_selection());

		this->profile_writer.write_int("Kami", "ItemLootCheck", this->kami_item_loot_checkbox.get()->get_check());
		this->profile_writer.write_string("Kami", "ItemLootValue", this->kami_item_loot_textbox.get()->get_text());
	}

	void kami_tabpage::load_from_profile(std::string const& profile_path)
	{
		this->profile_reader.set_path(profile_path);
		
		this->vertical_range_trackbar.get()->set_position(this->profile_reader.get_int("Kami", "RangeVertical", 50), true);
		this->horizontal_range_trackbar.get()->set_position(this->profile_reader.get_int("Kami", "RangeHorizontal", 50), true);
		
		this->kami_type_combobox.get()->set_selection(this->profile_reader.get_int("Kami", "KamiMovementType", 0));
		this->kami_target_combobox.get()->set_selection(this->profile_reader.get_int("Kami", "KamiTargetMethod", 0));

		this->kami_item_loot_checkbox.get()->set_check(this->profile_reader.get_int("Kami", "ItemLootCheck", FALSE) != FALSE);
		this->kami_item_loot_textbox.get()->set_text(this->profile_reader.get_string("Kami", "ItemLootValue", ""));
	}
	
	void kami_tabpage::toggle_functionality(bool checked)
	{
		net_buffer packet(trainer_tabpage_toggle);
		packet.write1(tabpage_ids::kami);
		
		if (packet.write1(checked))
		{
			packet.write2(this->horizontal_range_trackbar.get()->get_position());
			packet.write2(this->vertical_range_trackbar.get()->get_position());
			packet.write1(this->kami_type_combobox.get()->get_selection());
			packet.write1(this->kami_target_combobox.get()->get_selection());
			
			if (packet.write1(this->kami_item_loot_checkbox.get()->get_check()))
			{
				packet.write2(this->kami_item_loot_textbox.get()->get_integer());
				packet.write1(this->kami_loot_target_combobox.get()->get_selection());
			}
		}

		this->send_game_packet(packet);
	}

	void kami_tabpage::create_controls(rectangle& rect)
	{
		/* section image */
		std::shared_ptr<button> section_image = std::make_shared<button>(*this);
		section_image.get()->create("", rectangle((rect.width / 2) - (image_size / 2), image_offset, image_size, image_size));
		section_image.get()->set_bitmap(png_button_kami);
		this->store_control(section_image);
		
		/* Kami Range */
		int kami_range_seperator_continue = this->create_seperator("Kami Range", image_offset + image_size + image_offset);
		
		std::shared_ptr<label> horizontal_range_label = std::make_shared<label>(*this);
		horizontal_range_label.get()->create("Horizontal range", rectangle(TABPAGE_INDENT_LEFT, kami_range_seperator_continue, 150, control_height));
		this->store_control(horizontal_range_label);

		this->horizontal_range_trackbar = std::make_unique<trackbar>(*this);
		this->horizontal_range_trackbar.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), kami_range_seperator_continue + 3, control_width + key_indent + key_width, control_height));
		this->horizontal_range_trackbar.get()->set_range(0, 200);
		this->horizontal_range_trackbar.get()->set_position(100);
		this->horizontal_range_trackbar.get()->set_event(trackbar_event::on_drag, [this](short position) -> void
		{
			this->horizontal_range_textbox.get()->set_text(std::to_string(position));
		});

		this->horizontal_range_textbox = std::make_unique<textbox>(*this);
		this->horizontal_range_textbox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), kami_range_seperator_continue += (control_height + control_indent), control_width + key_indent + key_width, control_height), true);
		this->horizontal_range_textbox.get()->set_text("100");
		this->horizontal_range_textbox.get()->set_event(textbox_event::on_text_change, [this](std::string const& text) -> void
		{
			if (text.length() > 0)
			{
				int position = std::stoi(text, 0, 10);
				this->horizontal_range_trackbar.get()->set_position(position);

				if (position > 200)
					this->horizontal_range_textbox.get()->set_text("200");
			}
			else
			{
				this->horizontal_range_trackbar.get()->set_position(0);
			}
		});

		std::shared_ptr<label> vertical_range_label = std::make_shared<label>(*this);
		vertical_range_label.get()->create("Vertical range", rectangle(TABPAGE_INDENT_LEFT, kami_range_seperator_continue += (control_height + STANDARD_INDENT), 150, control_height));
		this->store_control(vertical_range_label);

		this->vertical_range_trackbar = std::make_unique<trackbar>(*this);
		this->vertical_range_trackbar.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), kami_range_seperator_continue + 3, control_width + key_indent + key_width, control_height));
		this->vertical_range_trackbar.get()->set_range(-100, 100);
		this->vertical_range_trackbar.get()->set_position(-100);
		this->vertical_range_trackbar.get()->set_event(trackbar_event::on_drag, [this](short position) -> void
		{
			this->vertical_range_textbox.get()->set_text(std::to_string(position));
		});
		
		this->vertical_range_textbox = std::make_unique<textbox>(*this);
		this->vertical_range_textbox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), kami_range_seperator_continue += (control_height + control_indent), control_width + key_indent + key_width, control_height), true);
		this->vertical_range_textbox.get()->set_text("-100");
		this->vertical_range_textbox.get()->set_event(textbox_event::on_text_change, [this](std::string const& text) -> void
		{
			if (text.length() > 0)
			{
				int position = std::stoi(text, 0, 10);
				this->vertical_range_trackbar.get()->set_position(position);
			
				if (position < -100)
					this->vertical_range_textbox.get()->set_text("-100");
				else if (position > 100)
					this->vertical_range_textbox.get()->set_text("100");
			}
			else
			{
				this->vertical_range_trackbar.get()->set_position(-100);
			}
		});
		
		/* Kami Preferences */
		int kami_preferences_seperator_continue = this->create_seperator("Kami Preferences", kami_range_seperator_continue + control_height + control_indent + STANDARD_INDENT);
		
		std::shared_ptr<label> kami_type_label = std::make_shared<label>(*this);
		kami_type_label.get()->create("Movement Type:", rectangle(TABPAGE_INDENT_LEFT, kami_preferences_seperator_continue, 150, control_height));
		this->store_control(kami_type_label);
		
		this->kami_type_combobox = std::make_unique<combobox>(*this);
		this->kami_type_combobox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), kami_preferences_seperator_continue, control_width + key_indent + key_width, control_height), true);
		this->kami_type_combobox.get()->add_item("Teleport Kami");
		//this->kami_type_combobox.get()->add_item("Portal Kami");
		this->kami_type_combobox.get()->set_selection(0);

		std::shared_ptr<label> kami_target_label = std::make_shared<label>(*this);
		kami_target_label.get()->create("Target Method:", rectangle(TABPAGE_INDENT_LEFT, kami_preferences_seperator_continue += ((control_height + control_indent) + 2), 150, control_height));
		this->store_control(kami_target_label);
		
		this->kami_target_combobox = std::make_unique<combobox>(*this);
		this->kami_target_combobox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), kami_preferences_seperator_continue, control_width + key_indent + key_width, control_height), true);
		this->kami_target_combobox.get()->add_item("Random Mob");
		this->kami_target_combobox.get()->add_item("Closest Mob");
		this->kami_target_combobox.get()->set_selection(0);
		
		/* Kami Loot */
		int kami_loot_seperator_continue = this->create_seperator("Kami Loot Preferences", kami_preferences_seperator_continue + control_height + control_indent + STANDARD_INDENT);
		
		this->kami_item_loot_checkbox = std::make_unique<togglebox>(*this);
		this->kami_item_loot_checkbox.get()->create("Activate kami-loot when items >", rectangle(TABPAGE_INDENT_LEFT, kami_loot_seperator_continue, 230, control_height));
		this->kami_item_loot_checkbox.get()->set_tooltip("Teleports your character to items around the map.");
		
		this->kami_item_loot_textbox = std::make_unique<textbox>(*this);
		this->kami_item_loot_textbox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), kami_loot_seperator_continue, control_width + key_indent + key_width, control_height), true, true);
		this->kami_item_loot_textbox.get()->set_maximum_length(3);
		this->kami_item_loot_textbox.get()->set_cue_banner("amount");
		
		std::shared_ptr<label> kami_loot_target_label = std::make_shared<label>(*this);
		kami_loot_target_label.get()->create("Target Method:", rectangle(TABPAGE_INDENT_LEFT, kami_loot_seperator_continue += (control_height + control_indent), 150, control_height));
		this->store_control(kami_loot_target_label);
		
		this->kami_loot_target_combobox = std::make_unique<combobox>(*this);
		this->kami_loot_target_combobox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), kami_loot_seperator_continue, control_width + key_indent + key_width, control_height), true);
		this->kami_loot_target_combobox.get()->add_item("Random Item");
		this->kami_loot_target_combobox.get()->add_item("Closest Item");
		this->kami_loot_target_combobox.get()->set_selection(0);
	}
}