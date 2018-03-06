#pragma once

#include "native.hpp"

namespace firefly
{
	bool set_force_splash_screen(bool enable);
	bool set_force_skip_nexon_api(bool enable);

	void set_generic_hooks(bool enable);
}