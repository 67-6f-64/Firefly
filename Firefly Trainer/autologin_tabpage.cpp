#include "autologin_tabpage.hpp"
#include "main_window.hpp"
#include "resource.hpp"

#include "button.hpp"
#include "seperator.hpp"

#include <ShlObj.h>
#pragma comment(lib, "shell32")

namespace firefly
{
	autologin_tabpage::autologin_tabpage(main_window* parent)
		: custom_tabpage(parent)
	{

	}

	autologin_tabpage::~autologin_tabpage()
	{

	}

	void autologin_tabpage::save_to_profile(std::string const& profile_path)
	{
		this->profile_writer.set_path(profile_path);

		this->profile_writer.write_string("AutoLogin", "Username", this->username_textbox.get()->get_text());
		this->profile_writer.write_string("AutoLogin", "Password", this->password_textbox.get()->get_text());
		this->profile_writer.write_string("AutoLogin", "Pic", this->pic_textbox.get()->get_text());
		
		this->profile_writer.write_int("AutoLogin", "Region", this->region_combobox.get()->get_selection());
		this->profile_writer.write_int("AutoLogin", "World", this->world_combobox.get()->get_selection());
		this->profile_writer.write_int("AutoLogin", "Channel", this->channel_combobox.get()->get_selection());
		this->profile_writer.write_int("AutoLogin", "Character", this->character_combobox.get()->get_selection());

		this->profile_writer.write_int("AutoLogin", "OfflineMode", this->offline_mode_togglebox.get()->get_check());
		this->profile_writer.write_int("AutoLogin", "RandomizeHwid", this->randomize_hwid_togglebox.get()->get_check());
		
		this->profile_writer.write_int("AutoLogin", "Relaunch", this->maplestory_relaunch_togglebox.get()->get_check());
		this->profile_writer.write_string("AutoLogin", "GamePath", this->game_path_label.get()->get_text());	
	}

	void autologin_tabpage::load_from_profile(std::string const& profile_path)
	{
		this->profile_reader.set_path(profile_path);

		this->username_textbox.get()->set_text(this->profile_reader.get_string("AutoLogin", "Username", ""));
		this->password_textbox.get()->set_text(this->profile_reader.get_string("AutoLogin", "Password", ""));
		this->pic_textbox.get()->set_text(this->profile_reader.get_string("AutoLogin", "Pic", ""));
		
		this->region_combobox.get()->set_selection(this->profile_reader.get_int("AutoLogin", "Region", 0));
		this->world_combobox.get()->set_selection(this->profile_reader.get_int("AutoLogin", "World", 0));
		this->channel_combobox.get()->set_selection(this->profile_reader.get_int("AutoLogin", "Channel", 0));
		this->character_combobox.get()->set_selection(this->profile_reader.get_int("AutoLogin", "Character", 0));
		
		this->offline_mode_togglebox.get()->set_check(this->profile_reader.get_int("AutoLogin", "OfflineMode", FALSE) != FALSE);
		this->randomize_hwid_togglebox.get()->set_check(this->profile_reader.get_int("AutoLogin", "RandomizeHwid", FALSE) != FALSE);
		
		this->maplestory_relaunch_togglebox.get()->set_check(this->profile_reader.get_int("AutoLogin", "Relaunch", FALSE) != FALSE);
		this->set_game_path(this->profile_reader.get_string("AutoLogin", "GamePath", "C:\\"));
	}
	
	void autologin_tabpage::toggle_functionality(bool checked)
	{
		net_buffer packet(trainer_tabpage_toggle);
		packet.write1(tabpage_ids::autologin);
		
		if (packet.write1(checked))
		{
			packet.write_string(this->username_textbox.get()->get_text());
			packet.write_string(this->password_textbox.get()->get_text());
			packet.write_string(this->pic_textbox.get()->get_text());
			
			packet.write1(this->region_combobox.get()->get_selection());
			packet.write1(this->world_combobox.get()->get_selection());
			packet.write1(this->channel_combobox.get()->get_selection());
			packet.write1(this->character_combobox.get()->get_selection());
			
			packet.write1(this->offline_mode_togglebox.get()->get_check());
			packet.write1(this->randomize_hwid_togglebox.get()->get_check());
		}

		this->send_game_packet(packet);
	}
	
	void autologin_tabpage::on_accepted_game_connection(unsigned int process_id)
	{
		this->maplestory_status_label.get()->set_foreground(RGB(0, 255, 0));
		this->maplestory_status_label->set_text("Connected - " + std::to_string(process_id));
	}

	void autologin_tabpage::on_closed_game_connection(bool relaunch)
	{
		this->maplestory_status_label.get()->set_foreground(RGB(255, 0, 0));
		this->maplestory_status_label->set_text("Disconnected");
		
		if (this->maplestory_relaunch_togglebox.get()->get_check() && relaunch)
			this->launch_game();
	}

	void autologin_tabpage::create_controls(rectangle& rect)
	{
		const int small_image_size = 24;

		/* section image */
		std::shared_ptr<button> section_image = std::make_shared<button>(*this);
		section_image.get()->create("", rectangle((rect.width / 2) - (image_size / 2), image_offset, image_size, image_size));
		section_image.get()->set_bitmap(png_button_autologin);
		this->store_control(section_image);
		
		/* Login Information */
		int login_details_seperator_continue = this->create_seperator("Login Details", image_offset + image_size + image_offset);

		std::shared_ptr<label> username_label = std::make_shared<label>(*this);
		username_label.get()->create("Username:", rectangle(TABPAGE_INDENT_LEFT, login_details_seperator_continue, control_width, control_height));
		this->store_control(username_label);

		this->username_textbox = std::make_unique<textbox>(*this);
		this->username_textbox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), login_details_seperator_continue, control_width + key_indent + key_width, control_height), true);
		this->username_textbox.get()->set_maximum_length(40);
		this->username_textbox.get()->set_cue_banner("username");

		std::shared_ptr<label> password_label = std::make_shared<label>(*this);
		password_label.get()->create("Password:", rectangle(TABPAGE_INDENT_LEFT, login_details_seperator_continue += (control_height + control_indent), control_width, control_height));
		this->store_control(password_label);

		this->password_textbox = std::make_unique<textbox>(*this);
		this->password_textbox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), login_details_seperator_continue, control_width + key_indent + key_width, control_height), true, false, true);
		this->password_textbox.get()->set_maximum_length(40);
		this->password_textbox.get()->set_cue_banner("password");

		std::shared_ptr<label> pic_label = std::make_shared<label>(*this);
		pic_label.get()->create("PIC:", rectangle(TABPAGE_INDENT_LEFT, login_details_seperator_continue += (control_height + control_indent), control_width, control_height));
		this->store_control(pic_label);

		this->pic_textbox = std::make_unique<textbox>(*this);
		this->pic_textbox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), login_details_seperator_continue, control_width + key_indent + key_width, control_height), true);
		this->pic_textbox.get()->set_maximum_length(20);
		this->pic_textbox.get()->set_cue_banner("pic");

		/* Login Preferences */
		int login_information_seperator_continue = this->create_seperator("Login Information", login_details_seperator_continue + control_height + control_indent + STANDARD_INDENT);
		
		std::shared_ptr<label> region_label = std::make_shared<label>(*this);
		region_label.get()->create("Region:", rectangle(TABPAGE_INDENT_LEFT, login_information_seperator_continue, control_width, control_height));
		this->store_control(region_label);

		this->region_combobox = std::make_unique<combobox>(*this);
		this->region_combobox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), login_information_seperator_continue, control_width + key_indent + key_width, control_height), true);
		this->region_combobox.get()->add_item("North America");
		this->region_combobox.get()->add_item("Europe");
		this->region_combobox.get()->set_event(combobox_event::on_selection_change, [this](int index) -> void
		{
			this->world_combobox.get()->clear_items();

			switch (index)
			{
			case 0:		
				this->world_combobox.get()->add_item("Scania");
				this->world_combobox.get()->add_item("Bera");
				this->world_combobox.get()->add_item("Broa");
				this->world_combobox.get()->add_item("Windia");
				this->world_combobox.get()->add_item("Khaini");
				this->world_combobox.get()->add_item("Bellocan");
				this->world_combobox.get()->add_item("Mardia");
				this->world_combobox.get()->add_item("Kradia");
				this->world_combobox.get()->add_item("Yellonda");
				this->world_combobox.get()->add_item("Demethos");
				this->world_combobox.get()->add_item("Galicia");
				this->world_combobox.get()->add_item("El Nido");
				this->world_combobox.get()->add_item("Zenith");
				this->world_combobox.get()->add_item("Arcania");
				this->world_combobox.get()->add_item("Chaos");
				this->world_combobox.get()->add_item("Nova");
				this->world_combobox.get()->add_item("Renegade");
				this->world_combobox.get()->add_item("Reboot");
				break;

			case 1:
				this->world_combobox.get()->add_item("Luna");
				break;

			default:
				break;
			}

			this->world_combobox.get()->set_selection(0);
		});

		std::shared_ptr<label> world_label = std::make_shared<label>(*this);
		world_label.get()->create("World:", rectangle(TABPAGE_INDENT_LEFT, login_information_seperator_continue += ((control_height + control_indent) + 2), control_width, control_height));
		this->store_control(world_label);

		this->world_combobox = std::make_unique<combobox>(*this);
		this->world_combobox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), login_information_seperator_continue, control_width + key_indent + key_width, control_height), true);

		this->region_combobox.get()->set_selection(0);

		std::shared_ptr<label> channel_label = std::make_shared<label>(*this);
		channel_label.get()->create("Channel:", rectangle(TABPAGE_INDENT_LEFT, login_information_seperator_continue += ((control_height + control_indent) + 2), control_width, control_height));
		this->store_control(channel_label);

		this->channel_combobox = std::make_unique<combobox>(*this);
		this->channel_combobox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), login_information_seperator_continue, control_width + key_indent + key_width, control_height), true);

		this->channel_combobox.get()->add_item("Random");

		for (int i = 0; i < 20; i++)
			this->channel_combobox.get()->add_item("Channel " + std::to_string(i + 1));

		this->channel_combobox.get()->set_selection(0);

		std::shared_ptr<label> character_label = std::make_shared<label>(*this);
		character_label.get()->create("Character:", rectangle(TABPAGE_INDENT_LEFT, login_information_seperator_continue += ((control_height + control_indent) + 2), control_width, control_height));
		this->store_control(character_label);

		this->character_combobox = std::make_unique<combobox>(*this);
		this->character_combobox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), login_information_seperator_continue, control_width + key_indent + key_width, control_height), true);

		for (int i = 0; i < 40; i++)
			this->character_combobox.get()->add_item("Character " + std::to_string(i + 1));

		this->character_combobox.get()->set_selection(0);
		
		/* Login Method */
		int login_preferences_seperator_continue = this->create_seperator("Login Preferences", login_information_seperator_continue + control_height + control_indent + STANDARD_INDENT);

		this->offline_mode_togglebox = std::make_unique<togglebox>(*this);
		this->offline_mode_togglebox.get()->create("Offline Mode", rectangle(TABPAGE_INDENT_LEFT, login_preferences_seperator_continue, control_width, control_height));
		this->offline_mode_togglebox.get()->set_tooltip("Logs you in with offline-mode on (hides you from buddylists and /find-commands).\nNotice: People can still whisper you.");

		this->randomize_hwid_togglebox = std::make_unique<togglebox>(*this);
		this->randomize_hwid_togglebox.get()->create("Randomize Hardware ID", rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), login_preferences_seperator_continue, control_width, control_height));
		this->randomize_hwid_togglebox.get()->set_tooltip("Randomizes your Hardware ID for MapleStory, so multiple clients can appear unlinked to Nexon.\nNotice: Should only be used when multi-clienting.");

		/* Status information */
		int status_information_seperator_continue = this->create_seperator("Launch Preferences", login_preferences_seperator_continue + control_height + control_indent + STANDARD_INDENT);
		
		this->maplestory_relaunch_togglebox = std::make_unique<togglebox>(*this);
		this->maplestory_relaunch_togglebox.get()->create("Auto Relaunch", rectangle(TABPAGE_INDENT_LEFT, status_information_seperator_continue, control_width, control_height));
		this->maplestory_relaunch_togglebox.get()->set_tooltip("Automatically relaunches MapleStory upon closure (crash, termination, etc.)");

		this->maplestory_status_label = std::make_unique<label>(*this);
		this->maplestory_status_label.get()->create("Disconnected", rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), status_information_seperator_continue, control_width + key_indent + key_width, control_height), true, true);
		this->maplestory_status_label.get()->set_foreground(RGB(255, 0, 0));
		
		std::shared_ptr<button> game_launch_button = std::make_shared<button>(*this);
		game_launch_button.get()->create("", rectangle(rect.width - (label_width + key_indent + control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), status_information_seperator_continue - 1, label_width, 24));
		game_launch_button.get()->set_tooltip("Launch MapleStory");
		game_launch_button.get()->set_bitmap(png_game_icon);
		game_launch_button.get()->set_event(button_event::on_click, [this]() -> bool
		{
			return this->launch_game();
		});
		this->store_control(game_launch_button);

		std::shared_ptr<label> game_location_label = std::make_shared<label>(*this);
		game_location_label.get()->create("Location:", rectangle(TABPAGE_INDENT_LEFT, status_information_seperator_continue += (control_height + control_indent), 50, control_height));
		this->store_control(game_location_label);

		this->game_path_label = std::make_unique<label>(*this);
		this->game_path_label.get()->create("C:\\", rectangle(TABPAGE_INDENT_LEFT + 55, status_information_seperator_continue, rect.width - (TABPAGE_INDENT_LEFT + 55 + key_indent + control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), control_height), false, true, true);
		this->game_path_label.get()->set_tooltip("C:\\");
		
		std::shared_ptr<button> change_location_button = std::make_shared<button>(*this);
		change_location_button.get()->create("Change Location", rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), status_information_seperator_continue, control_width + key_indent + key_width, control_height));
		change_location_button.get()->set_event(button_event::on_click, [this]() -> bool
		{
			char file_path[MAX_PATH];
			memset(file_path, 0, sizeof(file_path));

			BROWSEINFO browse_info;
			memset(&browse_info, 0, sizeof(BROWSEINFO));

			browse_info.hwndOwner = this->handle();
			browse_info.pszDisplayName = file_path;
			browse_info.lpszTitle = "Select the MapleStory folder.";
			browse_info.iImage = -1;

			ITEMIDLIST* item_id_list = SHBrowseForFolder(&browse_info);
			
			if (!item_id_list)
				return false;

			SHGetPathFromIDList(item_id_list, file_path);
			this->set_game_path(file_path);
			return true;
		});

		this->store_control(change_location_button);
	}
	
	bool autologin_tabpage::launch_game()
	{
		this->maplestory_status_label.get()->set_foreground(RGB(65, 150, 255));
		this->maplestory_status_label.get()->set_text("Connecting...");

		if (!this->parent->try_launch_game(this->game_path_label.get()->get_text(), this->username_textbox.get()->get_text(), this->password_textbox.get()->get_text()))
		{
			this->on_closed_game_connection(false);
			return false;
		}

		return true;
	}
	
	void autologin_tabpage::set_game_path(std::string const& path)
	{
		this->game_path_label.get()->set_text(path);
		this->game_path_label.get()->set_tooltip(path);
	}
}