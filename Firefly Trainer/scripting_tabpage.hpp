#pragma once

#include "native.hpp"
#include "custom_tabpage.hpp"

#include <memory>
#include <vector>

namespace firefly
{
	class scripting_tabpage : public custom_tabpage
	{
	public:
		scripting_tabpage(main_window* parent);
		~scripting_tabpage();

		void save_to_profile(std::string const& profile_path);
		void load_from_profile(std::string const& profile_path);
		
	protected:
		void create_controls(rectangle& rect);
		void create_handlers();
	};
}