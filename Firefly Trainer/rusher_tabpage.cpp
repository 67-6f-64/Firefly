#include "rusher_tabpage.hpp"
#include "resource.hpp"

#include "button.hpp"

namespace firefly
{
	rusher_tabpage::rusher_tabpage(main_window* parent)
		: custom_tabpage(parent)
	{

	}

	rusher_tabpage::~rusher_tabpage()
	{

	}
	
	void rusher_tabpage::set_map_pool(map_pool* maps)
	{
		this->maps = maps;
	}
	
	void rusher_tabpage::fetch_rusher_data(int map_id, fetch_actions action)
	{
		net_buffer packet(trainer_fetch_data);
		packet.write1(fetch_data_ids::rusher_data);
		packet.write4(map_id);
		packet.write1(static_cast<unsigned char>(action));

		if (!this->send_game_packet(packet))
			MessageBox(this->handle(), "You must be connected to the game client.", "Connection Error", MB_OK | MB_ICONERROR | MB_TOPMOST | MB_SETFOREGROUND);
	}

	void rusher_tabpage::on_fetch_rusher_data(net_buffer& packet)
	{
		unsigned int start = packet.read4();
		unsigned int end = packet.read4();
		
		this->map_route = this->maps->navigate(start, end);
		this->portal_route = this->maps->generate_path(start, end);

		if (this->map_route.size() <= 0 || this->portal_route.size() <= 0)
		{
			MessageBox(this->parent_handle(), "Could not generate a path to the specified Map ID.", "Rusher Error", MB_OK | MB_ICONERROR | MB_TOPMOST | MB_SETFOREGROUND);
		}
		else
		{
			switch (static_cast<fetch_actions>(packet.read1()))
			{
			case fetch_actions::rush_go:
				{
					net_buffer packet(trainer_tabpage_toggle);
					packet.write1(tabpage_ids::rusher);
					packet.write4(this->portal_route.size());
				
					for (std::shared_ptr<portal_data> const portal : this->portal_route)
					{
						packet.write4(portal.get()->map_id());
						packet.write2(portal.get()->position().x);
						packet.write2(portal.get()->position().y);
					}
				
					this->send_game_packet(packet);
				}
				
				break;

			case fetch_actions::generate_full_path:
				{
					this->map_search_listview.get()->clear_items();

					for (std::size_t i = 0; i < this->map_route.size(); i++)
						this->map_search_listview.get()->add_item_manual(static_cast<int>(i), this->map_route.at(i).get()->id(), this->map_route.at(i).get()->name());

					this->map_search_listview.get()->resize_list_view();
				}

				break;

			default:
				break;
			}
		}
	}

	void rusher_tabpage::create_controls(rectangle& rect)
	{
		/* section image */
		std::shared_ptr<button> section_image = std::make_shared<button>(*this);
		section_image.get()->create("", rectangle((rect.width / 2) - (image_size / 2), image_offset, image_size, image_size));
		section_image.get()->set_bitmap(png_button_rusher);
		this->store_control(section_image);
		
		/* tab_bar */
		this->map_search_textbox = std::make_shared<textbox>(*this);
		this->map_search_textbox.get()->create(rectangle(TABPAGE_INDENT_LEFT, image_offset + image_size + image_offset, rect.width - (TABPAGE_INDENT_LEFT + TABPAGE_INDENT_RIGHT), control_height), true);
		this->map_search_textbox.get()->set_cue_banner("Enter search query (map name or id)...");
		this->map_search_textbox.get()->set_event(textbox_event::on_return, [this](std::string const& text) -> void
		{
			if (text.length() > 0)
			{
				this->map_search_listview.get()->clear_items();

				std::vector<std::shared_ptr<map_data> const> map_list = this->maps->find_map(text);
				
				if (map_list.size() > 0 && map_list.size() <= 999)
				{
					for (std::size_t i = 0; i < map_list.size(); i++)
						this->map_search_listview.get()->add_item_manual(static_cast<int>(i), map_list.at(i).get()->id(), map_list.at(i).get()->name());

					this->map_search_listview.get()->resize_list_view();
				}
			}
		});

		this->map_search_listview = std::make_shared<list_view>(*this);
		this->map_search_listview.get()->create(rectangle(TABPAGE_INDENT_LEFT, image_offset + image_size + image_offset + control_height + STANDARD_INDENT, rect.width - (TABPAGE_INDENT_LEFT + TABPAGE_INDENT_RIGHT), 0), this->self);
		this->map_search_listview.get()->resize_list_view(false);
		
		this->map_search_listview.get()->add_column("#", (STANDARD_INDENT * 2) + 25);
		this->map_search_listview.get()->add_column("Map Id", 120, true);
		this->map_search_listview.get()->add_column("Map Name", 403);
		
		this->map_search_listview.get()->set_event(list_view_event_type::on_left_double_click, [this](int index) -> bool
		{
			this->fetch_rusher_data(this->map_search_listview.get()->get_item_integer(index, 1), fetch_actions::rush_go);
			return true;
		});
		
		this->map_search_listview.get()->add_menu_item("Rush (Go)", [this](int index) -> bool
		{
			this->fetch_rusher_data(this->map_search_listview.get()->get_item_integer(index, 1), fetch_actions::rush_go);
			return true;
		});
		
		this->map_search_listview.get()->add_menu_item("Generate Full Path", [this](int index) -> bool
		{
			this->fetch_rusher_data(this->map_search_listview.get()->get_item_integer(index, 1), fetch_actions::generate_full_path);
			return true;
		});
	}
}