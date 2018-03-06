#pragma once

#include "native.hpp"
#include "window.hpp"

#include "image_drawer.hpp"
#include "message_controller.hpp"

namespace firefly
{
	#define STANDARD_INDENT 16

	namespace custom_control
	{
		void initialize();
		void uninitialize();
	}

	class tooltip;

	class widget : public message_controller
	{
		void init(base_window& base);

	public:
		bool set_tooltip(std::string const& caption);
		void redraw(RECT* rc = NULL, bool erase = true);

	protected:
		widget(window& parent_window);
		widget(widget& parent_widget);
		virtual ~widget();
		
		bool build(std::string const& class_name, std::string const& widget_name, rectangle& rect, unsigned int style, unsigned int ex_style = 0);
		
		void reset_font();

	public:
		virtual bool on_mouse_wheel(int delta);
		
	private:
		virtual void on_mouse_enter(unsigned int keys, POINT pt);
		virtual void on_mouse_move(unsigned int keys, POINT pt);
		virtual void on_mouse_leave();
		
		virtual void on_mouse_left_down(HWND hWnd, unsigned int keys, POINT pt);
		virtual void on_mouse_left_up(HWND hWnd, unsigned int keys, POINT pt);
		virtual void on_mouse_left_double_click(HWND hWnd, unsigned int keys, POINT pt);

		virtual void on_mouse_right_down(HWND hWnd, unsigned int keys, POINT pt);
		virtual void on_mouse_right_up(HWND hWnd, unsigned int keys, POINT pt);
		virtual void on_mouse_right_double_click(HWND hWnd, unsigned int keys, POINT pt);
		
		virtual void on_set_focus(HWND old_focus);
		virtual void on_kill_focus(HWND new_focus);
		
		virtual void on_measure_item(HWND hWnd, MEASUREITEMSTRUCT* measure_item);
		virtual void on_draw_item(HWND hWnd, DRAWITEMSTRUCT* draw_item);

	private:
		void set_default_widget_handlers();

	protected:
		bool is_focused;
		bool is_hovered;

	private:
		std::unique_ptr<tooltip> infotip;
	};
}