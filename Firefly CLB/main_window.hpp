#pragma once

#include "native.hpp"

#include "window.hpp"
#include "widget.hpp"

#include "custom_tabpage.hpp"
#include "tab_menu.hpp"
#include "panel.hpp"

#include <memory>
#include <vector>

namespace firefly
{
	class main_window : public window
	{
		static const unsigned int window_width = 839;
		static const unsigned int window_height = 700;
		static const unsigned int tabpage_position = (216 + SCROLLBAR_WIDTH + 1);
		static const unsigned int dy = 37;

	public:
		main_window(HINSTANCE hinstance, std::string const& class_name, std::string const& window_name);
		~main_window();

		bool is_dialog_message(MSG* message);
		
		void save_profile(std::string const& profile_path);
		void load_profile(std::string const& profile_path);

		void create_client_instance();

	private:
		void create_handlers();
		void create_controls();

		void create_tab_control();
		
		template <typename T, typename... Args>
		void create_tab_page(std::string const& caption, unsigned int icon_resource_id, Args&&... args);

	private:
		std::unique_ptr<tabpage> tabmenu_panel;
		std::shared_ptr<scrollbar> scroll_bar;
		std::unique_ptr<tab_menu> tabmenu;
	};
	
	template <typename T, typename... Args>
	void main_window::create_tab_page(std::string const& caption, unsigned int icon_resource_id, Args&&... args)
	{
		int tab_index = this->tabmenu.get()->add_tab(caption, true, icon_resource_id, std::forward<Args>(args)...);
		
		std::shared_ptr<T> tab_page = std::make_shared<T>(this);
		tab_page.get()->create(rectangle(tabpage_position, dy, window_width - tabpage_position - 1, window_height - dy - 1));

		this->tabmenu.get()->relate(tab_index, tab_page);
	}
}