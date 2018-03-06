#pragma once

#include "native.hpp"

namespace firefly
{
	HWND get_maplestory_window_handle();
	void initialize_windows_hooks(HMODULE module);
}