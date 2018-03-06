#pragma once

#include "native.hpp"

#include "custom_tabpage.hpp"
#include "client_socket.hpp"

#include "account.hpp"

#include "togglebox.hpp"

#include <memory>
#include <vector>

namespace firefly
{
	class instance_tabpage : public custom_tabpage, public client_socket
	{
	public:
		instance_tabpage(main_window* parent);
		~instance_tabpage();

		void save_to_profile(std::string const& profile_path);
		void load_from_profile(std::string const& profile_path);
		
	protected:
		bool on_connect(SOCKET sock);
		bool on_close(SOCKET sock);

		void create_handlers();
		void create_controls(rectangle& rect);

	private:
		std::unique_ptr<togglebox> auto_hp_togglebox;
	};
}