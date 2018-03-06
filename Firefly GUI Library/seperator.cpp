#include "seperator.hpp"
#include "resource.hpp"

namespace firefly
{
	seperator::seperator(window& parent_window)
		: widget(parent_window)
	{
		this->set_default_message_handlers();
	}
			
	seperator::seperator(widget& parent_widget)
		: widget(parent_widget)
	{
		this->set_default_message_handlers();
	}

	seperator::~seperator()
	{

	}

	void seperator::create(std::string const& caption, rectangle& rect)
	{
		if (!this->build(WC_STATIC, caption.c_str(), rect, WS_VISIBLE | WS_CHILD | SS_OWNERDRAW))
			throw std::exception("Failed to create 'panel' widget");

		this->set_foreground(RGB(210, 210, 210));
		
		this->set_font(this->create_font("Arial", 25));
		this->reset_font();
	}

	void seperator::set_default_message_handlers()
	{
		this->add_message_handler(WM_ERASEBKGND, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			return TRUE;
		});
		
		this->add_message_handler(OWNER_DRAWITEM, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			return this->draw_item(hWnd, reinterpret_cast<DRAWITEMSTRUCT*>(lParam));
		});
	}
		
	LRESULT seperator::draw_item(HWND hWnd, DRAWITEMSTRUCT* owner_draw)
	{
		/* draw background */
		RECT rc_background(owner_draw->rcItem);
		this->draw_rectangle(&rc_background, this->background());

		/* draw underline */
		rc_background.top = rc_background.bottom - 1;
		this->draw_rectangle(&rc_background, RGB(46, 47, 51));

		/* draw label */
		int text_length = Static_GetTextLength(owner_draw->hwndItem);

		char* text_buffer = new char[text_length + 1];
		text_length = Static_GetText(owner_draw->hwndItem, text_buffer, text_length + 1);

		this->draw_text(std::string(text_buffer), &owner_draw->rcItem, this->foreground(), DT_LEFT | DT_TOP | DT_SINGLELINE);
		
		delete[] text_buffer;
		
		this->commit_drawing(owner_draw->hDC, &owner_draw->rcItem);
		return TRUE;
	}
}