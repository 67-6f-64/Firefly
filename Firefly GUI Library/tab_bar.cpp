#include "tab_bar.hpp"
#include "resource.hpp"

#include "tabpage.hpp"

namespace firefly
{
	/* tab_bar_item */
	tab_bar_item::tab_bar_item(std::string const name, bool activated, int caption_width)
		: tab_base_item(name, activated)
	{
		this->caption_width = caption_width;
	}

	/* tab_bar */
	void tab_bar::init()
	{
		this->font_height = 0;
	}

	tab_bar::tab_bar(window& parent_window)
		: tab_base(parent_window)
	{
		this->init();
	}

	tab_bar::tab_bar(widget& parent_widget)
		: tab_base(parent_widget)
	{
		this->init();
	}

	tab_bar::~tab_bar()
	{

	}
	
	int tab_bar::add_tab(std::string const& caption, int width)
	{
		if (width == -1)
		{
			HDC hdc = GetDC(this->handle());

			RECT rc_caption = this->calculate_caption_rect(hdc, caption);
			width = rc_caption.right - rc_caption.left;

			ReleaseDC(this->handle(), hdc);
		}

		return this->tab_base::add_tab(caption, true, width);
	}

	int tab_bar::create(rectangle& rect)
	{
		this->tab_base::create(rect);

		this->set_background_colors(this->background(), RGB(40, 40, 40));
		this->set_font(this->create_font("Arial", 16, 7, FW_REGULAR));

		return this->resize_tab_bar(rect);
	}

	bool tab_bar::enumerate_tabs(tab_base_enum_func enum_func)
	{
		RECT rc(this->rc_client);
		rc.bottom -= BORDER_HEIGHT;

		for (std::size_t i = 0; i < this->tab_base_items.size(); i++)
		{
			std::shared_ptr<tab_bar_item> item = this->tab_base_items.at(i);

			RECT rc_tab(rc);
			rc_tab.right = rc_tab.left + item.get()->caption_width;

			if (!enum_func(item, i, &rc_tab))
				return false;
			
			rc.left += item.get()->caption_width + TAB_H_INDENT;
		}

		return true;
	}

	bool tab_bar::draw_tab_item(std::shared_ptr<tab_bar_item> item, int index, int flags, RECT* rc)
	{
		/* draw background */
		this->draw_rectangle(rc, this->get_background_back_color());

		/* draw selection box */
		if (flags & TBASE_SELECTED)
		{
			RECT rc_selection = { rc->left, rc->top, rc->right, rc->bottom };
			rc_selection.top += rc_selection.bottom - SELECTOR_HEIGHT;

			this->draw_rectangle(&rc_selection, RGB(30, 215, 96));
		}

		/* draw label */
		rc->bottom -= SELECTOR_HEIGHT;

		COLORREF text_color = this->foreground();
		
		if ((flags & TBASE_HOVERED) || (flags & TBASE_SELECTED))
			text_color = RGB(255, 255, 255);
		
		this->draw_text(item.get()->name, rc, text_color, DT_CENTER | DT_MODIFYSTRING | DT_END_ELLIPSIS | DT_SINGLELINE | DT_VCENTER, this->font());
		return true;
	}
	
	RECT tab_bar::calculate_caption_rect(HDC hdc, std::string const& caption)
	{
		RECT rc_caption;
		HGDIOBJ old_font = SelectObject(hdc, this->font());

		DrawText(hdc, caption.c_str(), -1, &rc_caption, DT_CENTER | DT_VCENTER | DT_CALCRECT);

		SelectObject(hdc, old_font);
		return rc_caption;
	}

	RECT tab_bar::calculate_background_front_rect()
	{
		RECT rc_seperator(this->rc_client);
		rc_seperator.top = rc_seperator.bottom - BORDER_HEIGHT;

		return rc_seperator;
	}

	int tab_bar::resize_tab_bar(rectangle& rect)
	{
		if (IsRectEmpty(&this->rc_client))
		{
			HDC hdc = GetDC(this->handle());

			RECT rc_caption = this->calculate_caption_rect(hdc, "K");
			this->font_height = rc_caption.bottom - rc_caption.top;

			this->rc_client.bottom = TAB_V_INDENT + this->font_height + TAB_V_INDENT + BORDER_HEIGHT;
			
			this->resize(this->rc_client.right - this->rc_client.left, this->rc_client.bottom - this->rc_client.top);
			this->resize_graphics(hdc, this->rc_client.right - this->rc_client.left, this->rc_client.bottom - this->rc_client.top);

			ReleaseDC(this->handle(), hdc);
		}

		return rect.y + (this->rc_client.bottom - this->rc_client.top) + STANDARD_INDENT;
	}
}