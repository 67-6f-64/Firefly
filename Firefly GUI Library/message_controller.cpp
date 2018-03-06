#include "message_controller.hpp"

namespace firefly
{
	void message_controller::add_message_handler(unsigned int message, message_handler_t handler_function)
	{
		this->handlers[message] = handler_function;
	}

	void message_controller::remove_message_handler(unsigned int message)
	{
		this->handlers.erase(message);
	}
	
	LRESULT CALLBACK message_controller::default_message_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		message_controller* object = nullptr;

		if (uMsg == WM_NCCREATE)
		{
			CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lParam);
			object = static_cast<message_controller*>(create_struct->lpCreateParams);

			SetWindowLong(hWnd, GWLP_USERDATA, reinterpret_cast<LONG>(object));
		}
		else
		{
			object = reinterpret_cast<message_controller*>(GetWindowLong(hWnd, GWLP_USERDATA));
		}
		
		if (object)
		{
			if (uMsg == WM_NCDESTROY)
			{
				if (object->original_window_proc != nullptr)
					SetWindowLong(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG>(object->original_window_proc));
				
				return 0;
			}
			else
			{
				return object->window_proc(hWnd, uMsg, wParam, lParam);
			}
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	
	message_controller::message_controller(HINSTANCE hinstance, HWND hwnd_parent, bool is_window)
		: image_drawer(hinstance, hwnd_parent)
	{
		this->is_window = is_window;
		this->original_window_proc = nullptr;

		this->handlers.clear();
	}

	message_controller::~message_controller()
	{
		if (this->hwnd && this->original_window_proc)
			SetWindowLong(this->hwnd, GWLP_WNDPROC, reinterpret_cast<LONG>(this->original_window_proc));
	}
	
	bool message_controller::initialize_widget(HWND handle)
	{
		SetWindowLong(handle, GWLP_USERDATA, reinterpret_cast<LONG>(this));
		this->original_window_proc = reinterpret_cast<WNDPROC>(SetWindowLong(handle, GWLP_WNDPROC, reinterpret_cast<LONG>(default_message_proc)));
		
		return (this->original_window_proc != NULL);
	}
	
	LRESULT message_controller::original_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (is_window)
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		else
			return CallWindowProc(this->original_window_proc, hWnd, uMsg, wParam, lParam);
	}
	
	LRESULT message_controller::window_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (this->handlers[uMsg])
			return this->handlers[uMsg](hWnd, wParam, lParam);

		return this->original_proc(hWnd, uMsg, wParam, lParam);
	}
}