#pragma once

#include "native.hpp"
#include "widget.hpp"

#include "tabpage.hpp"

#include "file_reader.hpp"
#include "file_writer.hpp"

#include <vector>

namespace firefly
{
	class main_window;
	
	class custom_tabpage : public tabpage
	{
	protected:
		const int image_size = 48;
		const int image_offset = 16;
		
		const int control_width = 200;
		const int control_height = 22;
		const int control_indent = 8;

	public:
		custom_tabpage(main_window* parent);
		~custom_tabpage();
		
		void create(rectangle& rect = rectangle());
		
		virtual void save_to_profile(std::string const& profile_path);
		virtual void load_from_profile(std::string const& profile_path);
		
	protected:
		main_window* parent;

		io::file_reader profile_reader;
		io::file_writer profile_writer;
	};
}