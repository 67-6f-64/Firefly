#include "scrollbar.hpp"

namespace firefly
{
	scrollbar::scrollbar(widget& parent_widget)
		: widget(parent_widget)
	{
		this->scroll_up_button = std::make_unique<system_button>(parent_widget);
		this->scroll_down_button = std::make_unique<system_button>(parent_widget);

		this->is_dragging_thumb = false;
		this->thumb_offset = 0.0;

		this->min = 0.0;
		this->max = 1.0;
		this->page = 1.0;
		this->position = 0.0;
		this->scroll_unit = 1.0;

		this->scroll_remainder = 0.0;
		this->scroll_offset = 0.0;

		this->bound_control = nullptr;
		
		SetRectEmpty(&this->rc_scrollbar);
		SetRectEmpty(&this->rc_bound_client);
		
		this->client_height = 0.0;
		this->client_relation = 0.0;
		this->client_delta = 0.0;
		this->client_page = 0.0;
		this->max_position = 0.0;
		this->relative_delta = 0.0;
		this->window_height = 0.0;

		this->set_background(RGB(18, 18, 18));
		this->set_default_message_handlers();
	}
			
	scrollbar::~scrollbar()
	{

	}
	
	void scrollbar::set_scroll_unit(float unit)
	{
		this->scroll_unit = unit;
	}
	
	void scrollbar::scroll_to(int new_position)
	{
		this->update_position(new_position / this->client_delta);
	}

	void scrollbar::scroll_to_coord(int new_coordinate, bool from_bottom)
	{
		if (from_bottom)
			new_coordinate -= static_cast<int>(this->client_height);

		this->update_position(static_cast<float>(new_coordinate) / this->relative_delta);
	}
	
	bool scrollbar::get_visible_rect(RECT* rc)
	{
		if (!CopyRect(rc, &this->rc_bound_client))
			return false;

		rc->top = static_cast<int>((this->position / this->max) * this->window_height);
		rc->bottom = rc->top + static_cast<int>(this->client_height);
		return true;
	}
	
	HWND scrollbar::get_scroll_button_down()
	{
		return this->scroll_down_button.get()->handle();
	}

	HWND scrollbar::get_scroll_button_up()
	{
		return this->scroll_up_button.get()->handle();
	}

	void scrollbar::map_point_relatively(POINT* pt, bool positive)
	{
		if (positive)
			pt->y += static_cast<int>((this->position / this->max) * this->window_height);
		else
			pt->y -= static_cast<int>((this->position / this->max) * this->window_height);
	}
	
	void scrollbar::map_points_relatively(RECT* rc, bool positive)
	{
		this->map_point_relatively(reinterpret_cast<POINT*>(&rc->left), positive);
		this->map_point_relatively(reinterpret_cast<POINT*>(&rc->right), positive);
	}

	void scrollbar::auto_reset(widget& control, int full_height)
	{
		GetClientRect(this->handle(), &this->rc_scrollbar);

		if (GetClientRect(control.handle(), &this->rc_bound_client))
			this->rc_bound_client.right -= SCROLLBAR_WIDTH;

		this->min = 0.0;
		this->max = 100.0;

		this->client_height = static_cast<float>(this->rc_scrollbar.bottom - this->rc_scrollbar.top + SCROLLBUTTON_HEIGHT + SCROLLBUTTON_HEIGHT);
		this->client_relation = static_cast<float>(full_height) / client_height;
		this->client_delta = (this->client_height / (this->max - this->min));

		this->relative_delta = (this->client_delta * this->client_relation);
		this->window_height = (this->client_height * this->client_relation);

		this->page = this->max / this->client_relation;
		this->client_page = (this->page * this->client_delta);

		this->max_position = static_cast<float>(this->rc_scrollbar.bottom + SCROLLBUTTON_HEIGHT + SCROLLBUTTON_HEIGHT) - this->client_page;
		
		this->redraw();
	}
	
	void scrollbar::bind_control(widget* control)
	{
		this->bound_control = control;
	}

	void scrollbar::create(rectangle& rect)
	{
		if (this->scroll_up_button)
		{
			this->scroll_up_button.get()->create(system_button_type::scrollbar_up, rectangle(rect.x, rect.y, SCROLLBAR_WIDTH, SCROLLBUTTON_HEIGHT));
			this->scroll_up_button.get()->set_event(button_event::on_click, [this]() -> bool
			{
				float new_position = (this->position - 1) * this->client_delta;

				if (new_position < 0)
					new_position = 0;

				this->update_position(new_position / this->client_delta);
				return true;
			});
			
			rect.y += SCROLLBUTTON_HEIGHT;
			rect.height -= SCROLLBUTTON_HEIGHT;
		}

		if (this->scroll_down_button)
		{
			this->scroll_down_button.get()->create(system_button_type::scrollbar_down, rectangle(rect.x, rect.y + rect.height - SCROLLBUTTON_HEIGHT, SCROLLBAR_WIDTH, SCROLLBUTTON_HEIGHT));
			this->scroll_down_button.get()->set_event(button_event::on_click, [this]() -> bool
			{
				float new_position = (this->position + 1) * this->client_delta;

				if (new_position >= this->max_position)
					new_position = this->max_position;

				this->update_position(new_position / this->client_delta);
				return true;
			});
			
			rect.height -= SCROLLBUTTON_HEIGHT;
		}
	
		if (!this->build(WC_CUSTOM_SCROLLBAR, "", rect, WS_VISIBLE | WS_CHILD))
			throw std::exception("Failed to create 'scrollbar' widget");
	}
	
	bool scrollbar::on_mouse_wheel(int wheel_delta)
	{
		unsigned int wheel_scroll_lines = 0;
		
		if (!SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &wheel_scroll_lines, 0) || wheel_scroll_lines <= 1)
			wheel_scroll_lines = 3;

		float scroll_delta = static_cast<float>(-wheel_delta / 120) * static_cast<float>(wheel_scroll_lines);
		float new_position = (this->position + scroll_delta) * this->client_delta;

		if (new_position < 0)
			new_position = 0;
		if (new_position >= this->max_position)
			new_position = this->max_position;

		this->update_position(new_position / this->client_delta);
		return true;
	}
	
	void scrollbar::on_mouse_enter(unsigned int keys, POINT pt)
	{
		this->redraw();
	}

	void scrollbar::on_mouse_move(unsigned int keys, POINT pt)
	{
		float thumb_start = this->position * this->client_delta;
		float thumb_end = thumb_start + this->client_page;

		if (static_cast<float>(pt.y) >= thumb_start && static_cast<float>(pt.y) <= thumb_end)
		{
			if (!this->is_hovered)
			{
				this->is_hovered = true;
				this->redraw();
			}
		}
		else
		{
			if (this->is_hovered)
			{
				this->is_hovered = false;
				this->redraw();
			}
		}

		if (this->is_dragging_thumb)
		{
			float new_position = static_cast<float>(pt.y) - this->thumb_offset;

			if (new_position < 0)
				new_position = 0;
			if (new_position >= this->max_position)
				new_position = this->max_position;

			this->update_position(new_position / this->client_delta);
		}
	}

	void scrollbar::on_mouse_leave()
	{
		this->redraw();
	}
	
	void scrollbar::on_mouse_left_down(HWND hWnd, unsigned int keys, POINT pt)
	{
		float thumb_start = this->position * this->client_delta;
		float thumb_end = thumb_start + this->client_page;

		if (static_cast<float>(pt.y) >= thumb_start && static_cast<float>(pt.y) <= thumb_end)
		{
			this->thumb_offset = static_cast<float>(pt.y) - thumb_start;
			this->is_dragging_thumb = true;
		}
		else
		{
			float new_position = ((static_cast<float>(pt.y) < thumb_start) ? (thumb_start - this->client_page) : thumb_end);

			if (new_position < 0)
				new_position = 0;
			if (new_position >= this->max_position)
				new_position = this->max_position;
			
			this->update_position(new_position / this->client_delta);
		}

		SetCapture(hWnd);
	}
	
	void scrollbar::on_mouse_left_up(HWND hWnd, unsigned int keys, POINT pt)
	{
		ReleaseCapture();
		this->is_dragging_thumb = false;
	}
	
	void scrollbar::redraw_bound_control(int dy)
	{
		RECT rc_redraw(this->rc_bound_client);

		if (dy < 0)
			rc_redraw.top = rc_redraw.bottom + dy;
		else
			rc_redraw.bottom = rc_redraw.top + dy;

		WNDENUMPROC enum_proc = [](HWND hWnd, LPARAM lParam) -> BOOL
		{
			RECT* rc = reinterpret_cast<RECT*>(lParam);
			
			RECT rc_client;
			GetWindowRect(hWnd, &rc_client);

			if (IntersectRect(&rc_client, &rc_client, rc))
			{
				char class_name[256];
				GetClassName(hWnd, class_name, sizeof(class_name));

				if (!strcmp(class_name, "Edit"))
					InvalidateRect(hWnd, NULL, TRUE);
				else
				{
					ScreenToClient(hWnd, reinterpret_cast<POINT*>(&rc_client.left));
					ScreenToClient(hWnd, reinterpret_cast<POINT*>(&rc_client.right));

					InvalidateRect(hWnd, &rc_client, TRUE);
				}
			}

			return TRUE;
		};

		RECT rc_screen(rc_redraw);
		ClientToScreen(this->bound_control->handle(), reinterpret_cast<POINT*>(&rc_screen.left));
		ClientToScreen(this->bound_control->handle(), reinterpret_cast<POINT*>(&rc_screen.right));

		EnumChildWindows(this->bound_control->handle(), enum_proc, reinterpret_cast<LPARAM>(&rc_screen));
		this->bound_control->redraw(&rc_redraw);
	}
	
	void scrollbar::update_position(float new_position)
	{
		this->perform_scroll(new_position);
		this->position = new_position;
		this->redraw();
	}

	void scrollbar::perform_scroll(float new_position)
	{
		if (this->bound_control && new_position != this->position)
		{
			float position_delta = this->position - new_position;
			float dy = (position_delta * this->relative_delta) + this->scroll_remainder;
			
			this->scroll_remainder = dy;
			this->scroll_remainder -= std::roundf(dy);
			
			float client_offset = this->client_height + this->scroll_offset;
			float window_offset = this->window_height - client_offset;

			RECT rc_update;
			SetRectEmpty(&rc_update);
			
			RECT rc_temp_bound_client(this->rc_bound_client);
			rc_temp_bound_client.top -= static_cast<int>(this->scroll_offset);	
			rc_temp_bound_client.bottom += static_cast<int>(window_offset);
			
			this->scroll_offset -= std::roundf(dy);

			ScrollWindowEx(this->bound_control->handle(), 0, static_cast<int>(std::roundf(dy)), &rc_temp_bound_client,
				NULL, NULL, &rc_update, SW_ERASE | SW_INVALIDATE | SW_SCROLLCHILDREN);

			this->redraw_bound_control(static_cast<int>(std::roundf(dy)));
		}
	}

	void scrollbar::set_default_message_handlers()
	{
		this->add_message_handler(WM_COMMAND, [](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			return SendMessage(reinterpret_cast<HWND>(lParam), CUSTOM_COMMAND, wParam, lParam);
		});

		this->add_message_handler(WM_ERASEBKGND, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			return TRUE;
		});

		this->add_message_handler(WM_PAINT, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			
			this->paint_client_area(hdc, &ps.rcPaint, ps.fErase != FALSE);

			EndPaint(hWnd, &ps);
			return 0;
		});

		this->add_message_handler(WM_PRINTCLIENT, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			RECT rc;
			GetClientRect(this->handle(), &rc);
			
			this->paint_client_area(reinterpret_cast<HDC>(wParam), &rc, true);
			return 0;
		});
	}

	LRESULT scrollbar::paint_client_area(HDC hdc, RECT* rc_paint, bool erase)
	{
		/* draw scrollbar background */
		this->draw_rectangle(rc_paint, this->background());
		
		/* draw scrollbar thumb */
		float delta = static_cast<float>(rc_paint->bottom - rc_paint->top) / (this->max - this->min);
		
		RECT rc_thumb = { rc_paint->left, rc_paint->top, rc_paint->right, rc_paint->bottom };
		rc_thumb.top = static_cast<int>(std::roundf(this->position * delta));
		rc_thumb.bottom = static_cast<int>(std::roundf((this->position + this->page) * delta));

		COLORREF fill_color = 0;

		if (this->is_hovered)
			fill_color = RGB(64, 64, 64);
		else
			fill_color = RGB(51, 51, 51);

		this->draw_rectangle(&rc_thumb, fill_color);
		this->commit_drawing(hdc, rc_paint);
		return 0;
	}
}