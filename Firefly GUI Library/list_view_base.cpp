#include "list_view_base.hpp"

namespace firefly
{
	#define LVIEW_HEADER_HOVERED 0x01

	namespace custom_control
	{
		LRESULT CALLBACK default_list_view_base_window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}

		bool register_list_view_base_control()
		{
			WNDCLASS wc;
			memset(&wc, 0, sizeof(WNDCLASS));

			wc.style = CS_GLOBALCLASS | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
			wc.lpfnWndProc = default_list_view_base_window_proc;
			wc.hCursor = LoadCursor(NULL, IDC_ARROW);
			wc.lpszClassName = WC_CUSTOM_LIST_VIEW_BASE;

			return (RegisterClass(&wc) != FALSE);
		}

		bool unregister_list_view_base_control()
		{
			return (UnregisterClass(WC_CUSTOM_LIST_VIEW_BASE, 0) != FALSE);
		}
	}
}