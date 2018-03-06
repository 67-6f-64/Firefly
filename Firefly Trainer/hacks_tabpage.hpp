#pragma once

#include "native.hpp"
#include "custom_tabpage.hpp"

#include "hack_types.hpp"

#include "togglebox.hpp"

#include <memory>
#include <vector>

namespace firefly
{
	class hacks_tabpage : public custom_tabpage
	{
	public:
		hacks_tabpage(main_window* parent);
		~hacks_tabpage();

		void save_to_profile(std::string const& profile_path);
		void load_from_profile(std::string const& profile_path);
		
		void toggle_functionality(bool checked);

	protected:
		void create_controls(rectangle& rect);

	private:
		bool toggle_func(int index, bool enabled);

	private:
		std::vector<std::shared_ptr<togglebox>> togglebox_container;
		std::map<hack_type, hack_type> exception_list;
	};
}