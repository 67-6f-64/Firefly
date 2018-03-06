#pragma once

#include "native.hpp"

#include "basic_types.hpp"
#include "base_window.hpp"

#include <Wincodec.h>

namespace firefly
{
	class image_drawer : public base_window
	{
		friend class combobox;
		friend class list_view;
		friend class menu_clickable;

	public:
		HBITMAP make_bitmap_from_png(HINSTANCE instance, unsigned int resource_id);

		image_drawer(HINSTANCE hinstance, HWND hwnd_parent = NULL);
		virtual ~image_drawer();
		
	protected:
		void reset_graphics(rectangle& rect, int offset_x = 0, int offset_y = 0);
		void resize_graphics(HDC hdc, int width, int height, int offset_x = 0, int offset_y = 0);
		void release_graphics(bool release_hdc = true);
		
	protected:
		void commit_drawing(HDC hdc, RECT* rc);
		
	protected:
		void draw_text(std::string const& caption, RECT* rc, COLORREF foreground_color, unsigned int style, HFONT font = NULL);
		void draw_text_wide(std::wstring const& caption, RECT* rc, COLORREF foreground_color, unsigned int style, HFONT font = NULL);
		
		void draw_hdc(HDC hdc, RECT* rc);
		void draw_focus_rect(RECT* rc);

		void draw_ellipse(RECT* rc, COLORREF fill_color, bool has_border = false, COLORREF border_color = RGB(0, 0, 0), int border_width = 0);
		void draw_rectangle(RECT* rc, COLORREF fill_color, bool has_border = false, COLORREF border_color = RGB(0, 0, 0), int border_width = 0, HBRUSH preset_brush = NULL);
		void draw_round_rectangle(RECT* rc, size& ellipse, COLORREF fill_color, bool has_border = false, COLORREF border_color = RGB(0, 0, 0), int border_width = 0);
		
		void draw_icon(HWND hWnd, rectangle& rect);

		void draw_bitmap(HBITMAP bitmap, point dest = point(), point source = point(), size dimensions = size());
		void draw_alpha_bitmap(HBITMAP bitmap, point dest = point(), point source = point(), size dimensions = size());
		void draw_alpha_bitmap_recolored(HBITMAP bitmap, int delta, point dest = point(), point source = point(), size dimensions = size());

	private:
		HBITMAP create_bitmap_mask(HBITMAP bitmap_object, COLORREF transparent = RGB(0, 0, 0));

		bool create_stream_on_resource(HINSTANCE instance, const char* resource_name, IStream** image_stream);
		bool load_bitmap_from_stream(IStream* image_stream, IWICBitmapSource** bitmap_source);

		HBITMAP create_bitmap_handle(IWICBitmapSource* bitmap_source);

	private:
		HDC hdc_backbuffer;
		HBITMAP bitmap_backbuffer;
		HBITMAP bitmap_backbuffer_old;
	};
}