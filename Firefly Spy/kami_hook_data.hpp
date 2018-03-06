#pragma once

#include "native.hpp"
#include "addresses.hpp"

#include "packet_objects.hpp"

namespace firefly
{
	typedef int (__fastcall* CUserLocal__GetJobCode_t)(void* ecx, void* edx);
	static CUserLocal__GetJobCode_t CUserLocal__GetJobCode = reinterpret_cast<CUserLocal__GetJobCode_t>(CUserLocal__GetJobCode_address);
	
	typedef void (__fastcall* CVecCtrl__SetTeleportNext_t)(void* ecx, void* edx, int nSkillID, int nX, int nY);
	static CVecCtrl__SetTeleportNext_t CVecCtrl__SetTeleportNext = reinterpret_cast<CVecCtrl__SetTeleportNext_t>(CVecCtrl__SetTeleportNext_address);
}