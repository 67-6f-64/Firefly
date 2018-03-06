#include "image_drawer.hpp"

#pragma comment(lib, "Msimg32")
#pragma comment(lib, "Windowscodecs")

#include <functional>

namespace firefly
{
	HBITMAP image_drawer::make_bitmap_from_png(HINSTANCE instance, unsigned int resource_id)
	{
		IStream* stream = NULL;

		if (!create_stream_on_resource(instance, MAKEINTRESOURCE(resource_id), &stream))
			return NULL;

		IWICBitmapSource* bitmap_source = NULL;

		if (!load_bitmap_from_stream(stream, &bitmap_source))
		{
			stream->Release();
			return NULL;
		}

		HBITMAP bitmap = create_bitmap_handle(bitmap_source);

		bitmap_source->Release();
		stream->Release();

		return bitmap;
	}
	
	image_drawer::image_drawer(HINSTANCE hinstance, HWND hwnd_parent)
		: base_window(hinstance, hwnd_parent)
	{
		this->hdc_backbuffer = NULL;
		this->bitmap_backbuffer = NULL;
		this->bitmap_backbuffer_old = NULL;
	}

	image_drawer::~image_drawer()
	{
		this->release_graphics();
	}
	
	void image_drawer::reset_graphics(rectangle& rect, int offset_x, int offset_y)
	{
		this->release_graphics();
		
		HDC hdc = GetDC(this->handle());

		this->hdc_backbuffer = CreateCompatibleDC(hdc);
		this->resize_graphics(hdc, rect.width, rect.height, offset_x, offset_y);

		ReleaseDC(this->handle(), hdc);
	}

	void image_drawer::resize_graphics(HDC hdc, int width, int height, int offset_x, int offset_y)
	{
		this->release_graphics(false);
		
		this->bitmap_backbuffer = CreateCompatibleBitmap(hdc, width + (offset_x * 2), height + (offset_y * 2));
		this->bitmap_backbuffer_old = reinterpret_cast<HBITMAP>(SelectObject(this->hdc_backbuffer, bitmap_backbuffer));
		
		SetWindowOrgEx(this->hdc_backbuffer, 0 - offset_x, 0 - offset_y, NULL);
	}

	void image_drawer::release_graphics(bool release_hdc)
	{
		if (this->bitmap_backbuffer && this->bitmap_backbuffer_old)
		{
			SelectObject(this->hdc_backbuffer, this->bitmap_backbuffer_old);
			this->bitmap_backbuffer_old = NULL;

			DeleteObject(this->bitmap_backbuffer);
			this->bitmap_backbuffer = NULL;
		}

		if (this->hdc_backbuffer && release_hdc)
		{
			DeleteDC(this->hdc_backbuffer);
			this->hdc_backbuffer = NULL;
		}
	}

	void image_drawer::commit_drawing(HDC hdc, RECT* rc)
	{
		int width = rc->right - rc->left;
		int height = rc->bottom - rc->top;
		
		BitBlt(hdc, rc->left, rc->top, width, height, this->hdc_backbuffer, rc->left, rc->top, SRCCOPY);
	}
	
	void image_drawer::draw_text(std::string const& caption, RECT* rc, COLORREF foreground_color, unsigned int style, HFONT font)
	{
		int old_background_mode = SetBkMode(this->hdc_backbuffer, TRANSPARENT);
		COLORREF old_text_color = SetTextColor(this->hdc_backbuffer, foreground_color);
		HGDIOBJ old_font = SelectObject(this->hdc_backbuffer, (font ? font : this->font()));

		RECT rc_align = { rc->left, rc->top, rc->right, rc->bottom };
		DrawTextA(this->hdc_backbuffer, caption.c_str(), -1, &rc_align, style | DT_CALCRECT);
		DrawTextA(this->hdc_backbuffer, caption.c_str(), -1, rc, style);

		SelectObject(this->hdc_backbuffer, old_font);
		SetTextColor(this->hdc_backbuffer, old_text_color);
		SetBkMode(this->hdc_backbuffer, old_background_mode);
	}
	
	void image_drawer::draw_text_wide(std::wstring const& caption, RECT* rc, COLORREF foreground_color, unsigned int style, HFONT font)
	{
		int old_background_mode = SetBkMode(this->hdc_backbuffer, TRANSPARENT);
		COLORREF old_text_color = SetTextColor(this->hdc_backbuffer, foreground_color);
		HGDIOBJ old_font = SelectObject(this->hdc_backbuffer, (font ? font : this->font()));

		RECT rc_align = { rc->left, rc->top, rc->right, rc->bottom };
		DrawTextW(this->hdc_backbuffer, caption.c_str(), -1, &rc_align, style | DT_CALCRECT);
		DrawTextW(this->hdc_backbuffer, caption.c_str(), -1, rc, style);

		SelectObject(this->hdc_backbuffer, old_font);
		SetTextColor(this->hdc_backbuffer, old_text_color);
		SetBkMode(this->hdc_backbuffer, old_background_mode);
	}
	
	void image_drawer::draw_hdc(HDC hdc, RECT* rc)
	{
		BitBlt(this->hdc_backbuffer, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, hdc, rc->left, rc->top, SRCCOPY);
	}

	void image_drawer::draw_focus_rect(RECT* rc)
	{
		DrawFocusRect(this->hdc_backbuffer, rc);
	}

	void image_drawer::draw_ellipse(RECT* rc, COLORREF fill_color, bool has_border, COLORREF border_color, int border_width)
	{
		HPEN pen = CreatePen(PS_INSIDEFRAME, border_width, border_color);
		HBRUSH brush = CreateSolidBrush(fill_color);

		HGDIOBJ old_pen = SelectObject(this->hdc_backbuffer, pen);
		HGDIOBJ old_brush = SelectObject(this->hdc_backbuffer, brush);

		Ellipse(this->hdc_backbuffer, rc->left, rc->top, rc->right, rc->bottom);

		SelectObject(this->hdc_backbuffer, old_brush);
		SelectObject(this->hdc_backbuffer, old_pen);

		DeleteObject(brush);
		DeleteObject(pen);
	}

	void image_drawer::draw_rectangle(RECT* rc, COLORREF fill_color, bool has_border, COLORREF border_color, int border_width, HBRUSH preset_brush)
	{		
		HPEN pen = CreatePen(PS_INSIDEFRAME, border_width, (has_border ? border_color : fill_color));
		HBRUSH brush = CreateSolidBrush(fill_color);

		HGDIOBJ old_pen = SelectObject(this->hdc_backbuffer, pen);
		HGDIOBJ old_brush = SelectObject(this->hdc_backbuffer, (preset_brush ? preset_brush : brush));
		
		Rectangle(this->hdc_backbuffer, rc->left, rc->top, rc->right, rc->bottom);

		SelectObject(this->hdc_backbuffer, old_brush);
		SelectObject(this->hdc_backbuffer, old_pen);

		DeleteObject(brush);
		DeleteObject(pen);
	}
	
	void image_drawer::draw_round_rectangle(RECT* rc, size& ellipse, COLORREF fill_color, bool has_border, COLORREF border_color, int border_width)
	{
		HPEN pen = CreatePen(PS_INSIDEFRAME, border_width, (has_border ? border_color : fill_color));
		HBRUSH brush = CreateSolidBrush(fill_color);

		HGDIOBJ old_pen = SelectObject(this->hdc_backbuffer, pen);
		HGDIOBJ old_brush = SelectObject(this->hdc_backbuffer, brush);

		RoundRect(this->hdc_backbuffer, rc->left, rc->top, rc->right, rc->bottom, ellipse.width, ellipse.height);

		SelectObject(this->hdc_backbuffer, old_brush);
		SelectObject(this->hdc_backbuffer, old_pen);

		DeleteObject(brush);
		DeleteObject(pen);
	}

	void image_drawer::draw_icon(HWND hWnd, rectangle& rect)
	{
		HICON icon = reinterpret_cast<HICON>(GetClassLong(hWnd, GCL_HICONSM));
		DrawIconEx(this->hdc_backbuffer, rect.x, rect.y, icon, rect.width, rect.height, 0, NULL, DI_NORMAL | DI_COMPAT);
	}

	void image_drawer::draw_bitmap(HBITMAP bitmap, point dest, point source, size dimensions)
	{
		HDC hdc_bitmap = CreateCompatibleDC(this->hdc_backbuffer);
		HBITMAP bitmap_mask = this->create_bitmap_mask(bitmap);

		HGDIOBJ old_object = NULL;

		if (dimensions.width && dimensions.height)
		{
			old_object = SelectObject(hdc_bitmap, bitmap_mask);
			BitBlt(this->hdc_backbuffer, dest.x, dest.y, dimensions.width, dimensions.height, hdc_bitmap, source.x, source.y, SRCAND);

			SelectObject(hdc_bitmap, bitmap);
			BitBlt(this->hdc_backbuffer, dest.x, dest.y, dimensions.width, dimensions.height, hdc_bitmap, source.x, source.y, SRCPAINT);
		}
		else
		{
			BITMAP bitmap_info;
			GetObject(bitmap, sizeof(bitmap_info), &bitmap_info);

			old_object = SelectObject(hdc_bitmap, bitmap_mask);
			BitBlt(this->hdc_backbuffer, dest.x, dest.y, bitmap_info.bmWidth, bitmap_info.bmHeight, hdc_bitmap, source.x, source.y, SRCAND);

			SelectObject(hdc_bitmap, bitmap);
			BitBlt(this->hdc_backbuffer, dest.x, dest.y, bitmap_info.bmWidth, bitmap_info.bmHeight, hdc_bitmap, source.x, source.y, SRCPAINT);
		}

		SelectObject(hdc_bitmap, old_object);
		DeleteObject(bitmap_mask);
		DeleteDC(hdc_bitmap);
	}
	
	void image_drawer::draw_alpha_bitmap(HBITMAP bitmap, point dest, point source, size dimensions)
	{
		BLENDFUNCTION blend_function;
		blend_function.BlendOp = AC_SRC_OVER;
		blend_function.BlendFlags = 0;
		blend_function.SourceConstantAlpha = 255;
		blend_function.AlphaFormat = AC_SRC_ALPHA;

		HDC hdc_bitmap = CreateCompatibleDC(this->hdc_backbuffer);
		HGDIOBJ old_object = NULL;

		if (dimensions.width && dimensions.height)
		{
			old_object = SelectObject(hdc_bitmap, bitmap);

			AlphaBlend(this->hdc_backbuffer, dest.x, dest.y, dimensions.width, dimensions.height, hdc_bitmap,
				source.x, source.y, dimensions.width, dimensions.height, blend_function);
		}
		else
		{
			BITMAP bitmap_info;
			GetObject(bitmap, sizeof(bitmap_info), &bitmap_info);

			old_object = SelectObject(hdc_bitmap, bitmap);

			AlphaBlend(this->hdc_backbuffer, dest.x, dest.y, bitmap_info.bmWidth, bitmap_info.bmHeight, hdc_bitmap, 
				source.x, source.y,	bitmap_info.bmWidth, bitmap_info.bmHeight, blend_function);
		}

		SelectObject(hdc_bitmap, old_object);
		DeleteDC(hdc_bitmap);
	}
	
	void image_drawer::draw_alpha_bitmap_recolored(HBITMAP bitmap, int delta, point dest, point source, size dimensions)
	{
		BITMAP bitmap_info;
		GetObject(bitmap, sizeof(bitmap_info), &bitmap_info);

		HDC hdc_bitmap = CreateCompatibleDC(NULL);
		HDC hdc_source = CreateCompatibleDC(NULL);

		if (hdc_bitmap && hdc_source)
		{
			BITMAPINFO RGB32BitsBITMAPINFO;
			ZeroMemory(&RGB32BitsBITMAPINFO, sizeof(BITMAPINFO));

			RGB32BitsBITMAPINFO.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			RGB32BitsBITMAPINFO.bmiHeader.biWidth = bitmap_info.bmWidth;
			RGB32BitsBITMAPINFO.bmiHeader.biHeight = bitmap_info.bmHeight;
			RGB32BitsBITMAPINFO.bmiHeader.biPlanes = 1;
			RGB32BitsBITMAPINFO.bmiHeader.biBitCount = 32;

			UINT* pixels;
			HBITMAP direct_bitmap = CreateDIBSection(hdc_bitmap, &RGB32BitsBITMAPINFO, DIB_RGB_COLORS, reinterpret_cast<void**>(&pixels), NULL, 0);

			if (direct_bitmap)
			{
				HGDIOBJ old_object = SelectObject(hdc_bitmap, direct_bitmap);
				HGDIOBJ old_source = SelectObject(hdc_source, bitmap);

				BitBlt(hdc_bitmap, 0, 0, bitmap_info.bmWidth, bitmap_info.bmHeight, hdc_source, 0, 0, SRCCOPY);

				for (int i = 0; i < (bitmap_info.bmWidth * bitmap_info.bmHeight); i++)
				{
					unsigned char alpha = ((pixels[i] & 0xFF000000) >> 24);

					if (alpha == 0xFF)
					{
						unsigned char red = (pixels[i] & 0xFF);
						unsigned char green = ((pixels[i] & 0xFF00) >> 8);
						unsigned char blue = ((pixels[i] & 0xFF0000) >> 16);
						
						if (delta >= 0)
						{
							red = (red + delta >= 255 ? 255 : red + delta);
							green = (green + delta >= 255 ? 255 : green + delta);
							blue = (blue + delta >= 255 ? 255 : blue + delta);
						}
						else
						{
							red = (red + delta <= 0 ? 0 : red + delta);
							green = (green + delta <= 0 ? 0 : green + delta);
							blue = (blue + delta <= 0 ? 0 : blue + delta);
						}

						pixels[i] = (alpha << 24) | RGB(red, green, blue);
					}
				}
				
				SelectObject(hdc_source, old_source);
				SelectObject(hdc_bitmap, old_object);

				this->draw_alpha_bitmap(direct_bitmap, dest, source, dimensions);

				DeleteObject(direct_bitmap);
			}
			
			DeleteDC(hdc_source);
			DeleteDC(hdc_bitmap);
		}
	}

	HBITMAP image_drawer::create_bitmap_mask(HBITMAP bitmap, COLORREF transparent)
	{
		BITMAP bitmap_info;
		GetObject(bitmap, sizeof(bitmap_info), &bitmap_info);

		HBITMAP bitmap_mask = CreateBitmap(bitmap_info.bmWidth, bitmap_info.bmHeight, 1, 1, NULL);

		HDC hdc_original = CreateCompatibleDC(NULL);
		HDC hdc_mask = CreateCompatibleDC(NULL);

		HGDIOBJ old_object_original = SelectObject(hdc_original, bitmap);
		HGDIOBJ old_object_mask = SelectObject(hdc_mask, bitmap_mask);

		COLORREF old_background_color = SetBkColor(hdc_original, transparent);
		
		BitBlt(hdc_mask, 0, 0, bitmap_info.bmWidth, bitmap_info.bmHeight, hdc_original, 0, 0, SRCCOPY);
		BitBlt(hdc_original, 0, 0, bitmap_info.bmWidth, bitmap_info.bmHeight, hdc_mask, 0, 0, SRCINVERT);
		
		SetBkColor(hdc_original, old_background_color);

		SelectObject(hdc_mask, old_object_mask);
		SelectObject(hdc_original, old_object_original);

		DeleteDC(hdc_mask);
		DeleteDC(hdc_original);

		return bitmap_mask;
	}

	bool image_drawer::create_stream_on_resource(HINSTANCE instance, const char* resource_name, IStream** image_stream)
	{
		HRSRC resource = FindResource(instance, resource_name, "PNG");

		if (!resource)
			return false;

		HGLOBAL global_image = LoadResource(NULL, resource);

		if (!global_image)
			return false;

		void* source_resource_data = LockResource(global_image);

		if (!source_resource_data)
			return false;
		
		unsigned long resource_size = SizeofResource(NULL, resource);

		HGLOBAL global_resource_data = GlobalAlloc(GMEM_MOVEABLE, resource_size);

		if (!global_resource_data)
			return false;

		void* resource_data = GlobalLock(global_resource_data);

		if (!resource_data)
		{
			GlobalFree(global_resource_data);
			return false;
		}

		CopyMemory(resource_data, source_resource_data, resource_size);
		GlobalUnlock(global_resource_data);

		if (CreateStreamOnHGlobal(global_resource_data, TRUE, image_stream) != S_OK)
		{
			GlobalFree(global_resource_data);
			return false;
		}

		return true;
	}
	
	bool image_drawer::load_bitmap_from_stream(IStream* stream, IWICBitmapSource** bitmap_source)
	{
		IWICBitmapDecoder* image_decoder;

		if (CoCreateInstance(CLSID_WICPngDecoder1, NULL, CLSCTX_INPROC_SERVER, __uuidof(image_decoder), reinterpret_cast<void**>(&image_decoder)) != S_OK)
			return false;

		if (image_decoder->Initialize(stream, WICDecodeMetadataCacheOnLoad) != S_OK)
		{
			image_decoder->Release();
			return false;
		}

		unsigned int frame_count = 0;

		if (image_decoder->GetFrameCount(&frame_count) != S_OK || frame_count != 1)
		{
			image_decoder->Release();
			return false;
		}

		IWICBitmapFrameDecode* image_frame = NULL;

		if (image_decoder->GetFrame(0, &image_frame) != S_OK)
		{
			image_decoder->Release();
			return false;
		}

		bool ret = (WICConvertBitmapSource(GUID_WICPixelFormat32bppPBGRA, image_frame, bitmap_source) == S_OK);
					
		image_frame->Release();
		image_decoder->Release();
		return ret;
	}
	
	HBITMAP image_drawer::create_bitmap_handle(IWICBitmapSource* bitmap_source)
	{
		unsigned int width = 0;
		unsigned int height = 0;

		if (bitmap_source->GetSize(&width, &height) != S_OK || !width || !height)
			return NULL;

		BITMAPINFO bitmap_info;
		memset(&bitmap_info, 0, sizeof(BITMAPINFO));

		bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitmap_info.bmiHeader.biWidth = width;
		bitmap_info.bmiHeader.biHeight -= height;
		bitmap_info.bmiHeader.biPlanes = 1;
		bitmap_info.bmiHeader.biBitCount = 32;
		bitmap_info.bmiHeader.biCompression = BI_RGB;
					
		void* image_bits;

		HDC hdc_screen = GetDC(NULL);

		HBITMAP bitmap = CreateDIBSection(hdc_screen, &bitmap_info, DIB_RGB_COLORS, &image_bits, NULL, 0);

		ReleaseDC(NULL, hdc_screen);

		if (!bitmap)
			return NULL;

		const unsigned int stride = width * 4;
		const unsigned int image = stride * height;

		if (bitmap_source->CopyPixels(NULL, stride, image, static_cast<unsigned char*>(image_bits)) != S_OK)
		{
			DeleteObject(bitmap);
			return NULL;
		}

		return bitmap;
	}
}