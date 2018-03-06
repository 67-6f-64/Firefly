#include "detours.hpp"

#include <detours.h>
#pragma comment(lib, "detours")

namespace firefly
{
	namespace detours
	{
		bool redirect(bool enable, void** function, void* redirection)
		{
			if (DetourTransactionBegin() != NO_ERROR)
			{
				return false;
			}

			if (DetourUpdateThread(GetCurrentThread()) != NO_ERROR)
			{
				return false;
			}

			if ((enable ? DetourAttach : DetourDetach)(function, redirection) != NO_ERROR)
			{
				return false;
			}

			if (DetourTransactionCommit() == NO_ERROR)
			{
				return true;
			}

			DetourTransactionAbort();
			return false;
		}
	}
}