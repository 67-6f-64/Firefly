#include "base_window.hpp"
#include "resource.hpp"

#include <string>

namespace firefly
{
	HWND base_window::handle()
	{
		return this->hwnd;
	}
	
	void base_window::set_handle(HWND hwnd)
	{
		this->hwnd = hwnd;
	}

	HWND base_window::parent_handle()
	{
		return this->hwnd_parent;
	}

	HINSTANCE base_window::instance()
	{
		return this->hinstance;
	}

	COLORREF base_window::foreground()
	{
		return this->foreground_color;
	}
		
	void base_window::set_foreground(COLORREF color)
	{
		this->foreground_color = color;
	}

	COLORREF base_window::background()
	{
		return this->background_color;
	}

	void base_window::set_background(COLORREF color)
	{
		this->background_color = color;
	}

	HFONT base_window::font()
	{
		return this->font_object;
	}
		
	void base_window::set_font(HFONT font)
	{
		if (this->font_object)
			DeleteFont(this->font_object);

		this->font_object = font;
	}
	
	void base_window::show(bool show)
	{
		ShowWindow(this->hwnd, show ? SW_SHOW : SW_HIDE);
	}

	void base_window::enable(bool state)
	{
		EnableWindow(this->hwnd, state);
	}
	
	void base_window::move(int x, int y)
	{
		if (this->hwnd)
			SetWindowPos(this->hwnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}

	void base_window::resize(int width, int height, bool redraw)
	{
		if (this->hwnd)
			SetWindowPos(this->hwnd, NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | (redraw ? NULL : SWP_NOREDRAW));
	}

	base_window::base_window(HINSTANCE hinstance, HWND hwnd_parent)
		: hinstance(hinstance), hwnd(NULL), hwnd_parent(hwnd_parent)
	{
		if (!this->hinstance)
		{
			if (this->hwnd_parent)
				this->hinstance = reinterpret_cast<HINSTANCE>(GetWindowLong(this->hwnd_parent, GWLP_HINSTANCE));
			else
				this->hinstance = GetModuleHandle(NULL);
		}

		this->foreground_color = RGB(0, 0, 0);
		this->background_color = RGB(0, 0, 0);
	}

	base_window::~base_window()
	{
		if (this->font_object)
			DeleteFont(this->font_object);

		if (this->hwnd)
			DestroyWindow(this->hwnd);
	}

	HFONT base_window::create_font(HFONT font)
	{
		LOGFONT font_attributes;
		memset(&font_attributes, 0, sizeof(LOGFONT));

		GetObject(font, sizeof(LOGFONT), &font_attributes);

		return CreateFontIndirect(&font_attributes);
	}

	HFONT base_window::create_font(std::string const& font_name, int height, int width, int weight)
	{
		LOGFONT font_attributes;
		memset(&font_attributes, 0, sizeof(LOGFONT));
		
		font_attributes.lfHeight = height;
		font_attributes.lfWidth = width;
		font_attributes.lfWeight = weight;
		font_attributes.lfCharSet = ANSI_CHARSET;
		font_attributes.lfOutPrecision = OUT_DEFAULT_PRECIS;
		font_attributes.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		font_attributes.lfQuality = PROOF_QUALITY;
		font_attributes.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
		memcpy(font_attributes.lfFaceName, font_name.c_str(), LF_FACESIZE);

		return CreateFontIndirect(&font_attributes);
	}
}