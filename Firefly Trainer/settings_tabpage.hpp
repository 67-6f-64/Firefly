#pragma once

#include "native.hpp"
#include "custom_tabpage.hpp"

#include "button.hpp"
#include "combobox.hpp"
#include "label.hpp"
#include "textbox.hpp"

#include <memory>
#include <vector>

namespace firefly
{
	class profile_data
	{
	public:
		profile_data(std::string const& file_name, std::string const& file_path);
		
		std::string name;
		std::string path;
	};

	class settings_tabpage : public custom_tabpage
	{
	public:
		settings_tabpage(main_window* parent);
		~settings_tabpage();

		void save_to_profile(std::string const& profile_path);
		void load_from_profile(std::string const& profile_path);
		
		void initialize_profile();
		
		void on_reset_authentication();
		void on_processing_authentication();
		void on_successful_authentication();

		void on_accepted_manager_connection();
		void on_closed_manager_connection();

	protected:
		void create_controls(rectangle& rect);

	private:
		std::vector<profile_data> profiles;
		std::unique_ptr<combobox> profile_combobox;

		std::unique_ptr<textbox> license_email_textbox;
		std::unique_ptr<textbox> license_password_textbox;
		std::unique_ptr<label> license_status_label;
		std::unique_ptr<button> license_status_image;

		std::unique_ptr<label> manager_status_label;
	};
}