#pragma once

#include "native.hpp"

namespace firefly
{
	namespace detours
	{
		bool redirect(bool enable, void** function, void* redirection);
	}
}