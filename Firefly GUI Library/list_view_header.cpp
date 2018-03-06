#include "list_view_header.hpp"

namespace firefly
{
	/* list_view_header */
	list_view_header::list_view_header(window& parent_window)
		: list_view_base(parent_window)
	{

	}

	list_view_header::list_view_header(widget& parent_widget)
		: list_view_base(parent_widget)
	{

	}

	list_view_header::~list_view_header()
	{

	}

	int list_view_header::add_item(std::string const& caption, int width, bool center)
	{
		this->items.push_back(list_view_header_item(caption, width, center));

		if (IsWindowVisible(this->handle()))
			this->redraw(&this->rc_client);
		
		return (this->items.size() - 1);
	}
	
	void list_view_header::create(rectangle& rect)
	{
		this->list_view_base::create(rect);
	}

	RECT list_view_header::calculate_item_rect(int index)
	{
		RECT rc_column_rect;
		SetRectEmpty(&rc_column_rect);
		
		rc_column_rect.left = this->rc_client.left;
		rc_column_rect.top = this->rc_client.top;
		rc_column_rect.bottom = rc_column_rect.top + LISTVIEW_COLUMN_HEIGHT;
		
		if (index >= 0)
		{
			rc_column_rect.right = rc_column_rect.left + this->items.at(0).width;

			for (std::size_t i = 1; i < this->items.size() && static_cast<int>(i) <= index; i++)
			{
				rc_column_rect.left = rc_column_rect.right;
				rc_column_rect.right += this->items.at(i).width;
			}
		}
		else
		{
			rc_column_rect.right = this->rc_client.right;
		}

		return rc_column_rect;
	}

	void list_view_header::draw_item(list_view_header_item& column, int index, int flags, RECT* rc)
	{
		this->draw_rectangle(rc, this->background());
		
		RECT rc_seperator = { rc->left, rc->top, rc->right, rc->bottom };
		rc_seperator.top = rc_seperator.bottom - 1;

		this->draw_rectangle(&rc_seperator, RGB(40, 40, 40));		

		COLORREF text_color = this->foreground();

		if (flags & LVBASE_HOVERED)
			text_color = RGB(255, 255, 255);
		
		InflateRect(rc, -(LVIEW_INDENT / 2), 0);
		this->draw_text(column.name, rc, text_color, (column.center ? DT_CENTER : DT_LEFT) | DT_MODIFYSTRING | DT_END_ELLIPSIS | DT_SINGLELINE | DT_VCENTER, this->font());
	}
}