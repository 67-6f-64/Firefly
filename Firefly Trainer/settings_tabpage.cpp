#include "settings_tabpage.hpp"
#include "main_window.hpp"
#include "resource.hpp"

#include <ShlObj.h>
#pragma comment(lib, "shell32")

namespace firefly
{
	/* profile_data */
	profile_data::profile_data(std::string const& file_name, std::string const& file_path)
	{
		this->name = file_name;
		this->path = file_path;
	}

	/* settings_tabpage */
	settings_tabpage::settings_tabpage(main_window* parent)
		: custom_tabpage(parent)
	{

	}

	settings_tabpage::~settings_tabpage()
	{

	}
	
	void settings_tabpage::save_to_profile(std::string const& profile_path)
	{
		this->profile_writer.set_path(profile_path);

		this->profile_writer.write_string("Settings", "LicenseEmail", this->license_email_textbox.get()->get_text());
		this->profile_writer.write_string("Settings", "LicensePassword", this->license_password_textbox.get()->get_text());
	}

	void settings_tabpage::load_from_profile(std::string const& profile_path)
	{
		this->profile_reader.set_path(profile_path);
		
		this->license_email_textbox.get()->set_text(this->profile_reader.get_string("Settings", "LicenseEmail", ""));
		this->license_password_textbox.get()->set_text(this->profile_reader.get_string("Settings", "LicensePassword", ""));
	}
	
	void settings_tabpage::initialize_profile()
	{
		this->profile_combobox.get()->set_selection(0);
	}
	
	void settings_tabpage::on_successful_authentication()
	{
		this->license_status_label.get()->set_foreground(RGB(0, 255, 0));
		this->license_status_label.get()->set_text("Authenticated");

		this->license_status_image.get()->set_bitmap(png_authenticated);
	}
	
	void settings_tabpage::on_processing_authentication()
	{
		this->license_status_label.get()->set_foreground(RGB(65, 150, 255));
		this->license_status_label.get()->set_text("Authenticating...");
	}

	void settings_tabpage::on_reset_authentication()
	{
		this->license_status_label.get()->set_foreground(RGB(255, 0, 0));
		this->license_status_label.get()->set_text("Unauthenticated");

		this->license_status_image.get()->set_bitmap(png_unauthenticated);
	}

	void settings_tabpage::on_accepted_manager_connection()
	{
		this->manager_status_label.get()->set_foreground(RGB(0, 255, 0));
		this->manager_status_label->set_text("Connected");
	}

	void settings_tabpage::on_closed_manager_connection()
	{
		this->manager_status_label.get()->set_foreground(RGB(255, 0, 0));
		this->manager_status_label->set_text("Disconnected");
	}

	void settings_tabpage::create_controls(rectangle& rect)
	{
		const int small_image_size = 24;

		/* section image */
		std::shared_ptr<button> section_image = std::make_shared<button>(*this);
		section_image.get()->create("", rectangle((rect.width / 2) - (image_size / 2), image_offset, image_size, image_size));
		section_image.get()->set_bitmap(png_button_settings);
		this->store_control(section_image);
		
		/* Profile Settings */
		int profile_settings_seperator_continue = this->create_seperator("Profile Settings", image_offset + image_size + image_offset);
		
		std::shared_ptr<label> current_profile_label = std::make_shared<label>(*this);
		current_profile_label.get()->create("Current Profile:", rectangle(TABPAGE_INDENT_LEFT, profile_settings_seperator_continue, 150, control_height));
		this->store_control(current_profile_label);
		
		this->profile_combobox = std::make_unique<combobox>(*this);
		this->profile_combobox.get()->create(rectangle(rect.width - control_width - TABPAGE_INDENT_RIGHT, profile_settings_seperator_continue, control_width - ((small_image_size + 5) * 3), control_height), true);
		this->profile_combobox.get()->set_event(combobox_event::on_selection_change, [this](int index) -> void
		{
			this->parent->load_profile(this->profiles.at(index).path);
		});

		char file_path[MAX_PATH];
		memset(file_path, 0, sizeof(file_path));

		if (GetModuleFileName(NULL, file_path, sizeof(file_path)))
		{
			*strrchr(file_path, '\\') = 0;
			strcat(file_path, "\\Profiles\\");

			WIN32_FIND_DATA find_data;
			HANDLE file = FindFirstFile(std::string(file_path + std::string("*.ffp")).c_str(), &find_data);

			if (file != INVALID_HANDLE_VALUE)
			{
				do
				{
					if (!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
					{
						std::string profile_name = std::string(find_data.cFileName);
						std::string profile_path = std::string(file_path + profile_name);

						this->profiles.push_back(profile_data(profile_name, profile_path));
						this->profile_combobox.get()->add_item(profile_name);
					}
				} 
				while (FindNextFile(file, &find_data));

				FindClose(file);
			}
		}

		std::shared_ptr<button> load_profile_image = std::make_shared<button>(*this);
		load_profile_image.get()->create("", rectangle(rect.width - (small_image_size * 3) - (5 * 2) - TABPAGE_INDENT_RIGHT, (profile_settings_seperator_continue += 1) - ((small_image_size - control_height) / 2), small_image_size, small_image_size));
		load_profile_image.get()->set_bitmap(png_load_profile);
		load_profile_image.get()->set_tooltip("Load profile");
		load_profile_image.get()->set_event(button_event::on_click, [this]() -> bool
		{
			OPENFILENAME open_filename;
			memset(&open_filename, 0, sizeof(open_filename));

			open_filename.lStructSize = sizeof(open_filename);
			open_filename.hInstance = this->instance();
			open_filename.lpstrFilter = "Firefly Profile (*.ffp)\0*.ffp\0\0";
			open_filename.nFilterIndex = 1;
			open_filename.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
			open_filename.lpstrTitle = "Choose a profile";
			open_filename.lpstrDefExt = "ffp";

			char profile_directory[MAX_PATH];
			memset(profile_directory, 0, sizeof(profile_directory));

			GetCurrentDirectory(MAX_PATH, profile_directory);
			strcat(profile_directory, "\\Profiles\\");

			open_filename.lpstrInitialDir = profile_directory;
			
			char profile_name[MAX_PATH];
			memset(profile_name, 0, sizeof(profile_name));

			open_filename.lpstrFileTitle = profile_name;
			open_filename.nMaxFileTitle = sizeof(profile_name);

			char profile_path[MAX_PATH];
			memset(profile_path, 0, sizeof(profile_path));

			open_filename.lpstrFile = profile_path;
			open_filename.nMaxFile = sizeof(profile_path);

			if (!GetOpenFileName(&open_filename))
				return false;

			for (profile_data profile : this->profiles)
				if (!profile.path.compare(profile_path))
					return true;

			this->profiles.push_back(profile_data(profile_name, profile_path));
			this->profile_combobox.get()->add_item(profile_name);
			return true;
		});
		this->store_control(load_profile_image);
		
		std::shared_ptr<button> save_profile_image = std::make_shared<button>(*this);
		save_profile_image.get()->create("", rectangle(rect.width - (small_image_size * 2) - 5 - TABPAGE_INDENT_RIGHT, profile_settings_seperator_continue - ((small_image_size - control_height) / 2), small_image_size, small_image_size));
		save_profile_image.get()->set_bitmap(png_save_profile);
		save_profile_image.get()->set_tooltip("Save profile");
		save_profile_image.get()->set_event(button_event::on_click, [this]() -> bool
		{
			OPENFILENAME open_filename;
			memset(&open_filename, 0, sizeof(open_filename));

			open_filename.lStructSize = sizeof(open_filename);
			open_filename.hInstance = this->instance();
			open_filename.lpstrFilter = "Firefly Profile (*.ffp)\0*.ffp\0\0";
			open_filename.nFilterIndex = 1;
			open_filename.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
			open_filename.lpstrTitle = "Select a destination file";
			open_filename.lpstrDefExt = "ffp";

			char profile_directory[MAX_PATH];
			memset(profile_directory, 0, sizeof(profile_directory));

			GetCurrentDirectory(MAX_PATH, profile_directory);
			strcat(profile_directory, "\\Profiles\\");

			open_filename.lpstrInitialDir = profile_directory;
			
			char profile_path[MAX_PATH];
			memset(profile_path, 0, sizeof(profile_path));

			open_filename.lpstrFile = profile_path;
			open_filename.nMaxFile = sizeof(profile_path);

			if (!GetSaveFileName(&open_filename))
				return false;
			
			this->parent->save_profile(profile_path);
			return true;
		});
		this->store_control(save_profile_image);
		
		std::shared_ptr<button> quicksave_profile_image = std::make_shared<button>(*this);
		quicksave_profile_image.get()->create("", rectangle(rect.width - small_image_size - TABPAGE_INDENT_RIGHT, profile_settings_seperator_continue - ((small_image_size - control_height) / 2), small_image_size, small_image_size));
		quicksave_profile_image.get()->set_bitmap(png_quicksave_profile);
		quicksave_profile_image.get()->set_tooltip("Quicksave current profile");
		quicksave_profile_image.get()->set_event(button_event::on_click, [this]() -> bool
		{
			this->parent->save_profile(this->profiles.at(this->profile_combobox.get()->get_selection()).path);
			return true;
		});
		this->store_control(quicksave_profile_image);

		/* License information */
		int license_information_seperator_continue = this->create_seperator("License Information", profile_settings_seperator_continue + control_height + control_indent + STANDARD_INDENT);
		
		std::shared_ptr<label> license_email_label = std::make_shared<label>(*this);
		license_email_label.get()->create("License Email:", rectangle(TABPAGE_INDENT_LEFT, license_information_seperator_continue, 150, control_height), false, true);
		this->store_control(license_email_label);

		this->license_email_textbox = std::make_unique<textbox>(*this);
		this->license_email_textbox.get()->create(rectangle(rect.width - control_width - TABPAGE_INDENT_RIGHT, license_information_seperator_continue, control_width, control_height), true);
		this->license_email_textbox.get()->set_maximum_length(40);
		this->license_email_textbox.get()->set_cue_banner("email");

		std::shared_ptr<label> license_password_label = std::make_shared<label>(*this);
		license_password_label.get()->create("License Password:", rectangle(TABPAGE_INDENT_LEFT, license_information_seperator_continue += (control_height + control_indent), 150, control_height), false, true);
		this->store_control(license_password_label);

		this->license_password_textbox = std::make_unique<textbox>(*this);
		this->license_password_textbox.get()->create(rectangle(rect.width - control_width - TABPAGE_INDENT_RIGHT, license_information_seperator_continue, control_width, control_height), true, false, true);
		this->license_password_textbox.get()->set_maximum_length(40);
		this->license_password_textbox.get()->set_cue_banner("password");
		
		std::shared_ptr<label> license_status_label = std::make_shared<label>(*this);
		license_status_label.get()->create("License Status:", rectangle(TABPAGE_INDENT_LEFT, license_information_seperator_continue += (control_height + control_indent), 150, control_height), false, true);
		this->store_control(license_status_label);
		
		this->license_status_label = std::make_unique<label>(*this);
		this->license_status_label.get()->create("Unauthenticated", rectangle(rect.width - (control_width + TABPAGE_INDENT_RIGHT - (small_image_size + 5)), license_information_seperator_continue, control_width - ((small_image_size + 5) * 2), control_height), true, true);
		this->license_status_label.get()->set_foreground(RGB(255, 0, 0));
		
		this->license_status_image = std::make_unique<button>(*this);
		this->license_status_image.get()->create("", rectangle(rect.width - small_image_size - TABPAGE_INDENT_RIGHT, license_information_seperator_continue - ((small_image_size - control_height) / 2), small_image_size, small_image_size));
		this->license_status_image.get()->set_bitmap(png_unauthenticated);
		this->license_status_image.get()->set_event(button_event::on_click, [this]() -> bool
		{
			return this->parent->try_authenticate(this->license_email_textbox.get()->get_text(), this->license_password_textbox.get()->get_text());
		});
		
		/* Status information */
		int status_information_seperator_continue = this->create_seperator("Status Information", license_information_seperator_continue + control_height + control_indent + STANDARD_INDENT);
		
		std::shared_ptr<label> manager_status_description_label = std::make_shared<label>(*this);
		manager_status_description_label.get()->create("Manager Status:", rectangle(TABPAGE_INDENT_LEFT, status_information_seperator_continue, 150, control_height), false, true);
		this->store_control(manager_status_description_label);

		this->manager_status_label = std::make_unique<label>(*this);
		this->manager_status_label.get()->create("Disconnected", rectangle(rect.width - control_width - TABPAGE_INDENT_RIGHT, status_information_seperator_continue, control_width, control_height), true, true);
		this->manager_status_label.get()->set_foreground(RGB(255, 0, 0));

	}
}