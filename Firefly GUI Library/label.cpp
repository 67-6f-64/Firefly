#include "label.hpp"

namespace firefly
{
	label::label(window& parent_window)
		: widget(parent_window)
	{
		this->set_default_message_handlers();
	}
			
	label::label(widget& parent_widget)
		: widget(parent_widget)
	{
		this->set_default_message_handlers();
	}

	label::~label()
	{

	}

	std::string label::get_text()
	{
		int text_length = Static_GetTextLength(this->handle());

		char* text_buffer = new char[text_length + 1];
		memset(text_buffer, 0, text_length + 1);

		text_length = Static_GetText(this->handle(), text_buffer, text_length + 1);

		if (text_length <= 0)
		{
			delete[] text_buffer;
			return std::string("");
		}

		std::string text_string(text_buffer);

		delete[] text_buffer;
		return text_string;
	}

	void label::set_text(std::string const& caption)
	{
		Static_SetText(this->handle(), caption.c_str());
	}

	void label::create(std::string const& caption, rectangle& rect, bool center, bool vertical_center, bool path_ellipsis)
	{
		if (!this->build(WC_STATIC, caption, rect, SS_NOTIFY | WS_VISIBLE | WS_CHILD | (center ? SS_CENTER : SS_LEFT) | (vertical_center ? SS_CENTERIMAGE : 0) | (path_ellipsis ? SS_PATHELLIPSIS : 0)))
			throw std::exception("Failed to create 'label' widget");
	}

	void label::set_default_message_handlers()
	{
		this->add_message_handler(CUSTOM_CTLCOLOR, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			SetTextColor(reinterpret_cast<HDC>(wParam), this->foreground());
			SetBkMode(reinterpret_cast<HDC>(wParam), TRANSPARENT);
			SetDCBrushColor(reinterpret_cast<HDC>(wParam), this->background());
			return reinterpret_cast<LRESULT>(GetStockObject(DC_BRUSH));
		});
	}
}