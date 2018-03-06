#pragma once

#include "native.hpp"
#include "image_drawer.hpp"

#include <functional>
#include <map>

namespace firefly
{
	typedef std::function<int(HWND, WPARAM, LPARAM)> message_handler_t;
	
	class message_controller : public image_drawer
	{
	public:
		void add_message_handler(unsigned int message, message_handler_t handler_function);
		void remove_message_handler(unsigned int message);

	protected:
		static LRESULT CALLBACK default_message_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	
		message_controller(HINSTANCE hinstance, HWND hwnd_parent = NULL, bool window = false);
		virtual ~message_controller();

		bool initialize_widget(HWND handle);

		LRESULT original_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	private:
		LRESULT window_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		
	private:
		bool is_window;
		WNDPROC original_window_proc;

		std::map<unsigned int, message_handler_t> handlers;
	};
}