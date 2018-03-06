#include "tab_menu.hpp"
#include "resource.hpp"

#include "tabpage.hpp"

#include "system_button.hpp"

#include <algorithm>

namespace firefly
{
	#define	SECTION_HEIGHT 24
	#define SECTION_INDENT 10

	/* tab_menu_item */
	tab_menu_item::tab_menu_item(std::string const name, bool activated, HBITMAP icon, bool has_button, bool is_add_button, button_event_function_t button_event_function)
		: tab_base_item(name, activated)
	{
		this->icon = icon;

		this->has_button = has_button;
		this->is_add_button = is_add_button;

		this->tab_button_event_function = button_event_function;

		this->has_toggle_box = false;
	}
	
	tab_menu_item::tab_menu_item(std::string const name, bool activated, HBITMAP icon, bool has_toggle_box, togglebox_event_function_t toggle_box_event_function)
		: tab_base_item(name, activated)
	{
		this->icon = icon;

		this->has_button = false;

		this->has_toggle_box = has_toggle_box;
		this->toggle_box_event_function = toggle_box_event_function;
	}

	tab_menu_item::~tab_menu_item()
	{

	}

	/* tab_menu */
	void tab_menu::init()
	{
		this->font_section = NULL;
		this->font_tab_item = NULL;
	}

	tab_menu::tab_menu(window& parent_window)
		: tab_base(parent_window)
	{	
		this->init();
		this->set_default_message_handlers();
	}
		
	tab_menu::tab_menu(widget& parent_widget)
		: tab_base(parent_widget)
	{	
		this->init();
		this->set_default_message_handlers();
	}

	tab_menu::~tab_menu()
	{

	}
	
	int tab_menu::add_tab(std::string const& caption, bool activated, int icon_resource_id, bool has_button, bool is_add_button, button_event_function_t button_event_function, std::function<void()> post_update_func)
	{
		HBITMAP icon = NULL;
		
		if (icon_resource_id)
			icon = this->make_bitmap_from_png(this->instance(), icon_resource_id);
		
		int index = this->tab_base::add_tab(caption, activated, icon, has_button, is_add_button, button_event_function);

		if (index >= 0)
		{
			RECT rc_item;
			SetRectEmpty(&rc_item);

			this->resize_control(index, &rc_item);

			if (IsWindowVisible(this->handle()))
				this->redraw(&rc_item);

			if (post_update_func != nullptr)
				post_update_func();
		}

		return index;
	}
	
	int tab_menu::add_tab(std::string const& caption, bool activated, int icon_resource_id, bool has_togglebox,	togglebox_event_function_t toggle_box_event_function)
	{
		HBITMAP icon = NULL;
		
		if (icon_resource_id)
			icon = this->make_bitmap_from_png(this->instance(), icon_resource_id);
		
		int index = this->tab_base::add_tab(caption, activated, icon, has_togglebox, toggle_box_event_function);

		if (index >= 0)
		{
			RECT rc_item;
			SetRectEmpty(&rc_item);

			this->resize_control(index, &rc_item);

			if (IsWindowVisible(this->handle()))
				this->redraw(&rc_item);
		}

		return index;
	}

	void tab_menu::remove_tab(int index)
	{
		if (index < static_cast<int>(this->tab_base_items.size()))
		{
			if (this->get_selection() >= index)
				this->reset_selection_state();

			this->tab_base_items.erase(this->tab_base_items.begin() + index);
			
			RECT rc_post_remove(this->rc_client);

			this->enumerate_tabs([&](std::shared_ptr<tab_menu_item> item, int item_index, RECT* rc) -> bool
			{
				if (item_index >= index)
				{
					this->update_button_function(item_index, item);

					if (item_index == index)
						rc_post_remove.top = rc->top;
					else /* if (item_index > index) */
						rc_post_remove.bottom = rc->bottom;

					RECT rc_button;
					CopyRect(&rc_button, rc);

					rc_button.right -= STANDARD_INDENT;
					rc_button.left = rc_button.right - this->button_size;

					int pre_height = (rc_button.bottom - rc_button.top) - this->button_size;

					rc_button.top += (pre_height / 2);
					rc_button.bottom -= (pre_height / 2);

					if (item && item.get()->tab_button)
						item.get()->tab_button.get()->move(rc_button.left, rc_button.top);
				}

				return true;
			});

			this->resize_control(index, &rc_post_remove, true);

			if (IsWindowVisible(this->handle()))
				this->redraw(&rc_post_remove);
		}
	}

	void tab_menu::create(rectangle& rect)
	{
		this->width = rect.width;
		this->tab_base::create(rect);
		
		this->set_background_colors(RGB(24, 24, 24), this->background());

		this->font_section = this->create_font("Arial", 15, 7, FW_LIGHT);
		this->font_tab_item = this->create_font("Arial", 15, 0, FW_LIGHT);
	}
	
	bool tab_menu::on_mouse_wheel(int delta)
	{
		this->enumerate_tabs([&, this](std::shared_ptr<tab_menu_item> item, int index, RECT* rc) -> bool
		{
			if (index >= 0 && index == this->get_selection())
			{
				for (std::shared_ptr<widget> relative : item.get()->relatives)
					relative.get()->on_mouse_wheel(delta);

				return false;
			}

			return true;
		});

		return false;
	}
	
	int tab_menu::resize_control(int item_index, RECT* rc_item, bool greedy)
	{
		int height = 0;

		this->enumerate_tabs([&](std::shared_ptr<tab_menu_item> item, int index, RECT* rc) -> bool
		{
			if (item.get()->activated || index == 0)
				height += (rc->bottom - rc->top);
			else
				height += (STANDARD_INDENT + (rc->bottom - rc->top));

			if (!greedy)
			{
				if (index == item_index)
					CopyRect(rc_item, rc);
			}

			return true;
		});
		
		HDC hdc = GetDC(this->handle());

		if (IsWindowVisible(this->handle()) && (height - this->rc_client.top) < this->rc_client.bottom)
			InvalidateRect(GetParent(this->handle()), NULL, TRUE);

		this->rc_client.bottom = (height - this->rc_client.top);

		this->resize(this->width, height, false);
		this->resize_graphics(hdc, width, height);
		
		ReleaseDC(this->handle(), hdc);
		return height;
	}
	
	bool tab_menu::enumerate_relatives(std::function<bool(std::shared_ptr<widget>)> enum_func)
	{
		for (std::shared_ptr<tab_menu_item> item : this->tab_base_items)
		{
			for (std::shared_ptr<widget> relative : item.get()->relatives)
			{
				if (enum_func(relative))
					return true;
			}
		}

		return false;
	}

	void tab_menu::set_default_message_handlers()
	{
		this->add_message_handler(WM_NOTIFY, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			return SendMessage(reinterpret_cast<NMHDR*>(lParam)->hwndFrom, CUSTOM_NOTIFY, wParam, lParam);
		});
				
		this->add_message_handler(WM_COMMAND, [](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			return SendMessage(reinterpret_cast<HWND>(lParam), CUSTOM_COMMAND, wParam, lParam);
		});

		this->add_message_handler(WM_CTLCOLORSTATIC, [](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			return SendMessage(reinterpret_cast<HWND>(lParam), CUSTOM_CTLCOLOR, wParam, lParam);
		});
	}

	bool tab_menu::enumerate_tabs(tab_base_enum_func enum_func)
	{
		RECT rc(this->rc_client);

		for (std::size_t i = 0; i < this->tab_base_items.size(); i++)
		{
			std::shared_ptr<tab_menu_item> item = this->tab_base_items.at(i);
			
			RECT rc_item(rc);

			if (!item.get()->activated)
			{
				if (i != 0)
					rc_item.top = (rc.top += SECTION_INDENT);
			}

			rc_item.bottom = (rc.top += STANDARD_INDENT);
				
			if (item.get()->icon)
			{
				BITMAP bitmap_info;
				GetObject(item.get()->icon, sizeof(bitmap_info), &bitmap_info);
					
				rc_item.bottom = (rc.top += bitmap_info.bmHeight);
			}
			else if (!item.get()->activated)
				rc_item.bottom = (rc.top += 32);
			
			if (item.get()->has_button)
			{
				if (!item.get()->tab_button)
				{
					RECT rc_button(rc_item);
					rc_button.right -= STANDARD_INDENT;
					rc_button.left = rc_button.right - this->button_size;

					int pre_height = (rc_button.bottom - rc_button.top) - this->button_size;

					rc_button.top += (pre_height / 2);
					rc_button.bottom -= (pre_height / 2);

					int width = rc_button.right - rc_button.left;
					int height = rc_button.bottom - rc_button.top;
					
					item.get()->tab_button = std::make_unique<system_button>(*this);
					item.get()->tab_button.get()->create(item.get()->is_add_button ? system_button_type::add : system_button_type::remove, rectangle(rc_button.left, rc_button.top, width, height));
					this->update_button_function(i, item);
				}
			}
			else if (item.get()->has_toggle_box)
			{
				if (!item.get()->toggle_box)
				{
					RECT rc_togglebox(rc_item);
					rc_togglebox.right -= STANDARD_INDENT;
					rc_togglebox.left = rc_togglebox.right - this->togglebox_size;

					int width = rc_togglebox.right - rc_togglebox.left;
					int height = rc_togglebox.bottom - rc_togglebox.top;

					item.get()->toggle_box = std::make_unique<togglebox>(*this);
					item.get()->toggle_box.get()->create("", rectangle(rc_togglebox.left, rc_togglebox.top, width, height));
					item.get()->toggle_box.get()->set_event(togglebox_event::on_state_change, item.get()->toggle_box_event_function);
				}
					
				rc_item.right -= (STANDARD_INDENT + this->togglebox_size + STANDARD_INDENT);
			}

			if (!enum_func(item, i, &rc_item))
				return false;
		}

		return true;
	}

	bool tab_menu::draw_tab_item(std::shared_ptr<tab_menu_item> item, int index, int flags, RECT* rc)
	{
		/* draw background */
		this->draw_rectangle(rc, this->get_background_front_color());

		/* draw selection box */
		if (flags & TBASE_SELECTED)
		{
			RECT rc_selection = { rc->left, rc->top, rc->left + 4, rc->bottom };
			this->draw_rectangle(&rc_selection, RGB(30, 215, 96));
		}

		/* draw icon */
		rc->left += STANDARD_INDENT;

		if (item.get()->icon)
		{
			BITMAP bitmap_info;
			GetObject(item.get()->icon, sizeof(bitmap_info), &bitmap_info);
			
			RECT rc_icon = { rc->left, rc->top, rc->left + bitmap_info.bmWidth, rc->bottom };
			
			if (flags & TBASE_HOVERED)
				this->draw_alpha_bitmap_recolored(item.get()->icon, 30, point(rc_icon.left, rc_icon.top + ((rc_icon.bottom - rc_icon.top) - bitmap_info.bmHeight) / 2));
			else
				this->draw_alpha_bitmap(item.get()->icon, point(rc_icon.left, rc_icon.top + ((rc_icon.bottom - rc_icon.top) - bitmap_info.bmHeight) / 2));

			rc->left += bitmap_info.bmWidth + STANDARD_INDENT;
		}
		else if (!item.get()->activated)
			rc->left += 32 + STANDARD_INDENT;

		/* draw label */
		HFONT text_font = this->font_section;
		COLORREF text_color = this->foreground();

		if (item.get()->activated)
		{
			text_font = this->font_tab_item;

			if ((flags & TBASE_HOVERED) || (flags & TBASE_SELECTED))
				text_color = RGB(255, 255, 255);
		}
			
		this->draw_text(item.get()->name, rc, text_color, DT_LEFT | DT_MODIFYSTRING | DT_END_ELLIPSIS | DT_SINGLELINE | DT_VCENTER, text_font);
		return true;
	}

	RECT tab_menu::calculate_background_front_rect()
	{
		return this->rc_client;
	}
	
	void tab_menu::update_button_function(int index, std::shared_ptr<tab_menu_item> item)
	{
		if (item.get()->has_button && item.get()->tab_button != nullptr)
		{
			if (item.get()->is_add_button)
				item.get()->tab_button.get()->set_event(button_event::on_click, item.get()->tab_button_event_function);
			else
			{
				item.get()->tab_button.get()->set_event(button_event::on_click, std::bind([this](int index) -> bool
				{
					this->remove_tab(index);
					return true;
				}, index));
			}
		}
	}
}