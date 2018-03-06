#pragma once

#include "native.hpp"
#include "custom_tabpage.hpp"

#include "botting_types.hpp"

#include "combobox.hpp"
#include "label.hpp"
#include "list_view.hpp"
#include "textbox.hpp"
#include "togglebox.hpp"
#include "trackbar.hpp"

#include <memory>
#include <vector>

namespace firefly
{
	class autobotting_tabpage : public custom_tabpage
	{
		class auto_macro_control
		{
		public:
			auto_macro_control(std::shared_ptr<togglebox> togglebox, std::shared_ptr<textbox> textbox, std::shared_ptr<combobox> combobox, bool has_combobox, unsigned int multiplier, unsigned int key = 0);
			~auto_macro_control();
			
			std::shared_ptr<togglebox> get_togglebox() const;
			std::shared_ptr<textbox> get_textbox() const;
			std::shared_ptr<combobox> get_combobox() const;

			bool has_combobox() const;
			unsigned int multiplier() const;
			unsigned int get_key() const;

		private:
			std::shared_ptr<togglebox> togglebox_;
			std::shared_ptr<textbox> textbox_;
			std::shared_ptr<combobox> combobox_;

			bool has_combobox_;
			unsigned int multiplier_;
			unsigned int key_;
		};

	public:
		autobotting_tabpage(main_window* parent);
		~autobotting_tabpage();

		void save_to_profile(std::string const& profile_path);
		void load_from_profile(std::string const& profile_path);
		
		void toggle_functionality(bool checked);

	protected:
		void create_controls(rectangle& rect);

	private:
		bool toggle_func(botting_type type, bool enabled);
		void handle_type(botting_type type, net_buffer& packet, bool custom = false, bool enabled = false);

		rectangle rect;

		std::vector<std::pair<std::string, unsigned int>> key_entries;

		std::unique_ptr<togglebox> auto_hp_togglebox;
		std::unique_ptr<textbox> auto_hp_textbox;
		std::unique_ptr<combobox> auto_hp_combobox;

		std::unique_ptr<togglebox> auto_mp_togglebox;
		std::unique_ptr<textbox> auto_mp_textbox;
		std::unique_ptr<combobox> auto_mp_combobox;
		
		std::unique_ptr<togglebox> auto_feed_pet_togglebox;
		std::unique_ptr<label> auto_feed_pet_label;
		std::unique_ptr<trackbar> auto_feed_pet_trackbar;
		std::unique_ptr<combobox> auto_feed_pet_combobox;

		std::vector<auto_macro_control> auto_macro_controls;
		
		std::unique_ptr<togglebox> auto_ability_points_togglebox;
		std::unique_ptr<combobox> auto_ability_points_combobox;
		
		std::unique_ptr<togglebox> auto_familiar_togglebox;
		std::unique_ptr<combobox> auto_familiar_combobox;
		
		std::unique_ptr<togglebox> auto_job_mount_togglebox;
		std::unique_ptr<combobox> auto_job_mount_combobox;
		
		std::unique_ptr<togglebox> auto_escape_togglebox;
		std::unique_ptr<textbox> auto_escape_textbox;
		std::unique_ptr<combobox> auto_escape_combobox;

		std::unique_ptr<togglebox> auto_timed_escape_togglebox;
		std::unique_ptr<textbox> auto_timed_escape_textbox;
		std::unique_ptr<combobox> auto_timed_escape_combobox;

		std::unique_ptr<togglebox> custom_channel_pool_togglebox;
		std::vector<std::shared_ptr<togglebox>> custom_channel_pool_container;

		std::unique_ptr<togglebox> allow_names;
		std::unique_ptr<textbox> allow_names_textbox;
		std::unique_ptr<list_view> allow_names_listview;
	};
}