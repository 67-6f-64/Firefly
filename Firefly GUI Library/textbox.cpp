#include "textbox.hpp"

namespace firefly
{
	void textbox::init()
	{
		this->is_readonly = false;
		this->cue_banner = std::string();

		SetRectEmpty(&this->rc_client);
		SetRectEmpty(&this->rc_nonclient);

		this->background_readonly = RGB(40, 40, 40);
		this->background_focused = RGB(64, 64, 64);
		this->background_normal = RGB(51, 51, 51);

		this->function_on_return = nullptr;
		this->function_on_text_change = nullptr;
	}

	textbox::textbox(window& parent_window)
		: widget(parent_window)
	{
		this->init();
		this->set_default_message_handlers();
	}
			
	textbox::textbox(widget& parent_widget)
		: widget(parent_widget)
	{
		this->init();
		this->set_default_message_handlers();
	}

	textbox::~textbox()
	{

	}
		
	void textbox::set_event(textbox_event event_type, textbox_event_function_t event_function)
	{
		if (event_type == textbox_event::on_return)
			this->function_on_return = event_function;
		else if (event_type == textbox_event::on_text_change)
			this->function_on_text_change = event_function;
	}
		
	bool textbox::set_cue_banner(std::string const& caption)
	{
		this->cue_banner = caption;
		return (RedrawWindow(this->handle(), NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT) != FALSE);
	}

	void textbox::set_maximum_length(std::size_t max_length)
	{
		Edit_LimitText(this->handle(), max_length);
	}

	bool textbox::set_readonly(bool read_only)
	{
		if (Edit_SetReadOnly(this->handle(), static_cast<BOOL>(read_only)) != FALSE)
		{
			this->is_readonly = read_only;
			return (RedrawWindow(this->handle(), NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT | RDW_FRAME) != FALSE);
		}

		return false;
	}
	
	bool textbox::set_text(std::string const& text)
	{
		if (Edit_SetText(this->handle(), text.c_str()) != FALSE)
			return (RedrawWindow(this->handle(), NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT) != FALSE);

		return false;
	}
	
	std::string textbox::get_text()
	{
		int text_length = this->get_text_length();
				
		char* text_buffer = new char[text_length + 1];
		memset(text_buffer, 0, text_length + 1);
				
		text_length = Edit_GetText(this->handle(), text_buffer, text_length + 1);

		if (text_length <= 0)
		{
			delete[] text_buffer;
			return std::string("");
		}

		std::string text_string(text_buffer);

		delete[] text_buffer;
		return text_string;
	}
	
	int textbox::get_integer()
	{
		try
		{
			return std::stoi(this->get_text(), 0, 10);
		}
		catch (std::exception& e)
		{
#ifdef _DEBUG
			printf("An exception occured in textbox::get_integer - %s\n", e.what());
#else
			UNREFERENCED_PARAMETER(e);
#endif
		}
			
		return 0;
	}

	int textbox::get_text_length()
	{
		return Edit_GetTextLength(this->handle());
	}

	void textbox::create(rectangle& rect, bool center, bool numeric, bool password)
	{
		if (!this->build(WC_EDIT, "", rect, WS_VISIBLE | WS_CHILD | WS_TABSTOP | (center ? ES_CENTER : ES_LEFT) | (numeric ? ES_NUMBER : 0) | (password ? ES_PASSWORD : 0) | ES_AUTOHSCROLL))
			throw std::exception("Failed to create 'textbox' widget");

		this->set_foreground(RGB(255, 255, 255));
	}

	void textbox::on_mouse_enter(unsigned int keys, POINT pt)
	{
		if (this->is_readonly)
			SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_NO)));
	}

	void textbox::set_default_message_handlers()
	{
		this->add_message_handler(WM_CHAR, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			if (wParam == VK_RETURN)
			{
				if (this->function_on_return)
				{
					if (!(HIWORD(lParam) & KF_REPEAT))
						this->function_on_return(this->get_text());

					return 0;
				}
			}
			else
			{
				RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT);
			}

			return this->original_proc(hWnd, WM_CHAR, wParam, lParam);
		});
		
		this->add_message_handler(CUSTOM_COMMAND, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			switch (HIWORD(wParam))
			{
			case EN_CHANGE:
				{
					if (this->function_on_text_change)
						this->function_on_text_change(this->get_text());
				}
				break;

			case EN_SETFOCUS:
				{
					if (this->is_readonly)
						HideCaret(hWnd);
				}
				break;

			case EN_KILLFOCUS:
				{
					if (this->is_readonly)
						ShowCaret(hWnd);
				}
				break;

			default:
				break;
			}

			return this->original_proc(hWnd, CUSTOM_COMMAND, wParam, lParam);
		});

		this->add_message_handler(WM_SETCURSOR, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			if (this->is_readonly)
				return TRUE;

			return this->original_proc(hWnd, WM_SETCURSOR, wParam, lParam);
		});
		
		this->add_message_handler(EM_SHOWBALLOONTIP, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			return TRUE;
		});

		this->add_message_handler(WM_ERASEBKGND, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			return TRUE;
		});

		this->add_message_handler(CUSTOM_CTLCOLOR, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			if (IsRectEmpty(&this->rc_nonclient))
				SetWindowPos(this->handle(), NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
			
			if (IsRectEmpty(&this->rc_client))
				GetClientRect(hWnd, &this->rc_client);

			COLORREF background_color = 0;
			
			if (!IsWindowEnabled(this->handle()) || this->is_readonly)
				background_color = this->background_readonly;
			else if (this->is_focused)
				background_color = this->background_focused;
			else 
				background_color = this->background_normal;

			this->draw_rectangle(&this->rc_client, background_color);
			
			if (!Edit_GetTextLength(hWnd) && !this->cue_banner.empty())
			{
				long style = GetWindowLong(hWnd, GWL_STYLE);

				if (!(style & ES_CENTER) || !this->is_focused)
				{
					RECT rc_text(this->rc_client);
					InflateRect(&rc_text, -4, 0);

					this->draw_text(this->cue_banner, &rc_text, RGB(135, 135, 135), ((style & ES_CENTER) ? DT_CENTER : DT_LEFT) | DT_VCENTER | DT_SINGLELINE | DT_MODIFYSTRING | DT_END_ELLIPSIS);
				}
			}

			this->commit_drawing(reinterpret_cast<HDC>(wParam), &this->rc_client);
			
			SetTextColor(reinterpret_cast<HDC>(wParam), this->foreground());
			SetBkMode(reinterpret_cast<HDC>(wParam), TRANSPARENT);
			return reinterpret_cast<LRESULT>(GetStockObject(NULL_BRUSH));
		});

		this->add_message_handler(WM_NCPAINT, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			LRESULT result = this->original_proc(hWnd, WM_NCPAINT, wParam, lParam);
			
			HDC hdc = GetDC(hWnd);
			
			if (!IsRectEmpty(&this->rc_client))
			{
				COLORREF background_color = 0;
				
				if (!IsWindowEnabled(this->handle()) || this->is_readonly)
					background_color = this->background_readonly;
				else if (this->is_focused)
					background_color = this->background_focused;
				else
					background_color = this->background_normal;
				
				RECT rc_union;
				UnionRect(&rc_union, &this->rc_original, &this->rc_nonclient);

				this->draw_hdc(hdc, &rc_union);

				this->draw_rectangle(&this->rc_original, this->background());
				this->draw_rectangle(&this->rc_nonclient, background_color);

				this->commit_drawing(hdc, &rc_union);
			}

			ReleaseDC(hWnd, hdc);
			return result;
		});

		this->add_message_handler(WM_NCCALCSIZE, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{  
			if (wParam == TRUE)
			{
				HDC hdc = GetDC(hWnd);

				/* calculate font height */
				HGDIOBJ old_font = SelectObject(hdc, this->font());

				RECT rc_text;
				SetRectEmpty(&rc_text);
				DrawText(hdc, "Ky", -1, &rc_text, DT_LEFT | DT_CALCRECT);

				SelectObject(hdc, old_font);

				int font_height = (rc_text.bottom - rc_text.top);

				/* calculate non-client window size */
				GetWindowRect(hWnd, &this->rc_nonclient);

				int window_width = (this->rc_nonclient.right - this->rc_nonclient.left);
				int window_height = (this->rc_nonclient.bottom - this->rc_nonclient.top);

				int dy = (window_height - font_height) / 2;

				OffsetRect(&this->rc_nonclient, -this->rc_nonclient.left, -this->rc_nonclient.top);
				CopyRect(&this->rc_original, &this->rc_nonclient);

				OffsetRect(&this->rc_nonclient, -1, -dy);

				reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam)->rgrc[0].left += 1;
				reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam)->rgrc[0].top += dy;
				reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam)->rgrc[0].right -= 1;
				reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam)->rgrc[0].bottom -= dy;
				
				this->resize_graphics(hdc, window_width, window_height, 1, dy);

				ReleaseDC(hWnd, hdc);
				return 0;
			}

			return this->original_proc(hWnd, WM_NCCALCSIZE, wParam, lParam);
		});
	}
}