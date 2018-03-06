#pragma once

#include "native.hpp"
#include "widget.hpp"

#include "tabpage.hpp"

#include "net_buffer.hpp"
#include "net_opcodes.hpp"

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
		
		const int control_width = 200; // 300
		const int control_height = 22;
		const int control_indent = 8;
		
		const int label_width = 50;

		const int key_width = 75;
		const int key_indent = 15;

	public:
		custom_tabpage(main_window* parent);
		~custom_tabpage();
		
		bool toggle(bool checked, bool is_logged_in = true);
	
		bool get_toggle();
		void set_toggle(bool checked, bool is_logged_in);
	
		void create(std::shared_ptr<custom_tabpage> self, rectangle& rect = rectangle());
		
		virtual void save_to_profile(std::string const& profile_path);
		virtual void load_from_profile(std::string const& profile_path);
		
		virtual void toggle_functionality(bool checked);
		virtual void on_toggled(bool checked);

	protected:
		bool send_game_packet(net_buffer& packet);
		
	protected:
		std::shared_ptr<custom_tabpage> self;

		bool requires_logged_in;
		main_window* parent;

		bool toggled_on;

		io::file_reader profile_reader;
		io::file_writer profile_writer;
	};
}