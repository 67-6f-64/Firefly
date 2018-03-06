#pragma once	

#include "tab_base.hpp"
#include "tabpage.hpp"

namespace firefly
{
	#define TAB_H_INDENT 30
	#define TAB_V_INDENT 11

	#define SELECTOR_HEIGHT 2

	#define BORDER_HEIGHT 1

	class tab_bar_item : public tab_base_item
	{
	public:
		tab_bar_item(std::string const name, bool activated, int caption_width);

	public:
		int caption_width;
	};

	class tab_bar : public tab_base<tab_bar_item>
	{
		void init();

	public:
		tab_bar(window& parent_window);
		tab_bar(widget& parent_window);
		~tab_bar();
		
		int add_tab(std::string const& caption, int width = -1);
		
		int create(rectangle& rect = rectangle());

	private:
		bool enumerate_tabs(tab_base_enum_func tab_func);
		bool draw_tab_item(std::shared_ptr<tab_bar_item> item, int index, int flags, RECT* rc);
		
		RECT calculate_caption_rect(HDC hdc, std::string const& caption);
		RECT calculate_background_front_rect();
		
		int resize_tab_bar(rectangle& rect);

	private:
		int font_height;
	};
}