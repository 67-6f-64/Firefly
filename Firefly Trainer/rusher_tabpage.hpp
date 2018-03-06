#pragma once

#include "native.hpp"
#include "custom_tabpage.hpp"

#include "list_view.hpp"
#include "textbox.hpp"

#include "map_pool.hpp"

#include <memory>
#include <vector>

namespace firefly
{
	enum class fetch_actions
	{
		rush_go,
		generate_full_path,
	};

	class rusher_tabpage : public custom_tabpage
	{
	public:
		rusher_tabpage(main_window* parent);
		~rusher_tabpage();
		
		void set_map_pool(map_pool* maps);
		
		void fetch_rusher_data(int map_id, fetch_actions action);
		void on_fetch_rusher_data(net_buffer& packet);

	protected:
		void create_controls(rectangle& rect);

	private:
		map_pool* maps;

		std::shared_ptr<textbox> map_search_textbox;
		std::shared_ptr<list_view> map_search_listview;
		
		std::vector<std::shared_ptr<map_data> const> map_route;
		std::vector<std::shared_ptr<portal_data> const> portal_route;
	};
}