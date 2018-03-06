#pragma once

#include "native.hpp"
#include "addresses.hpp"

#include "packet_objects.hpp"

namespace firefly
{
	typedef void (__fastcall* CDropPool__OnDropEnterField_t)(void* ecx, void* edx, CInPacket* iPacket);
	static CDropPool__OnDropEnterField_t CDropPool__OnDropEnterField = reinterpret_cast<CDropPool__OnDropEnterField_t>(CDropPool__OnDropEnterField_address);
}