#pragma once

#include "native.hpp"

namespace firefly
{
	static void* CLogin_ecx = nullptr;

	void try_select_region(void* ecx);
	void try_select_world_channel();

	void set_autologin_hooks(bool enable);
}