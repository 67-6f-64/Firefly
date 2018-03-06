#pragma once

#include "native.hpp"
#include "widget.hpp"

#include "system_button.hpp"

#include <functional>
#include <vector>

namespace firefly
{
	#define WC_CUSTOM_SCROLLBAR "CustomScrollbar"

	#define SCROLLBAR_WIDTH 16
	#define SCROLLBUTTON_HEIGHT 16

	struct FRECT
	{
		float left;
		float top;
		float right;
		float bottom;
	};

	class scrollbar : public widget
	{
	public:
		scrollbar(widget& parent_widget);
		~scrollbar();
		
		void set_scroll_unit(float unit);

		void scroll_to(int new_position);
		void scroll_to_coord(int new_coordinate, bool from_bottom = false);

		bool get_visible_rect(RECT* rc);

		HWND get_scroll_button_down();
		HWND get_scroll_button_up();

		void map_point_relatively(POINT* pt, bool positive = true);
		void map_points_relatively(RECT* rc, bool positive = true);

		void auto_reset(widget& control, int full_height);
		void bind_control(widget* control);

		void create(rectangle& rect = rectangle());

	public:
		bool on_mouse_wheel(int wheel_delta);

	protected:
		void on_mouse_enter(unsigned int keys, POINT pt);
		void on_mouse_move(unsigned int keys, POINT pt);
		void on_mouse_leave();

		void on_mouse_left_down(HWND hWnd, unsigned int keys, POINT pt);
		void on_mouse_left_up(HWND hWnd, unsigned int keys, POINT pt);
		
	private:
		void redraw_bound_control(int dy);

		void update_position(float new_position);
		void perform_scroll(float new_position);

	private:
		void set_default_message_handlers();
		
		LRESULT paint_client_area(HDC hdc, RECT* rc_paint, bool erase);

	private:
		std::unique_ptr<system_button> scroll_up_button;
		std::unique_ptr<system_button> scroll_down_button;

		bool is_dragging_thumb;
		float thumb_offset;

		float min;
		float max;
		float page;
		float position;
		float scroll_unit;

		float scroll_remainder;
		float scroll_offset;

		widget* bound_control;

		RECT rc_scrollbar;
		RECT rc_bound_client;

		float client_height;
		float client_relation;
		float client_delta;
		float client_page;
		float max_position;
		float relative_delta;
		float window_height;
	};
}