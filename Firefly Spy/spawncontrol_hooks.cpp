#include "spawncontrol_hooks.hpp"
#include "spawncontrol_hook_data.hpp"

#include "generic_hook_data.hpp"

#include "rusher_data.hpp"
#include "spawncontrol_data.hpp"
#include "detours.hpp"

#include "map_objects.hpp"

#include <iostream>

namespace firefly
{
	void set_spawn_point(POINT* pt)
	{
		if (rusher_data::get_instance().enabled())
		{
			rusher_entry entry;

			if (rusher_data::get_instance().get_entry(CUIMiniMap::GetInstance()->get_map_id(), entry))
			{
				pt->x = entry.x();
				pt->y = entry.y();
			}
			else
			{
				rusher_data::get_instance().disable();
			}
		}
		else if (spawncontrol_data::get_instance().enabled())
		{
			spawncontrol_entry entry;

			if (spawncontrol_data::get_instance().get_entry(CUIMiniMap::GetInstance()->get_map_id(), GetLevel(), entry))
			{
				pt->x = entry.x();
				pt->y = entry.y();
			}
		}
	}
	
	void __declspec(naked) CUserLocal__Init_hook()
	{
		__asm
		{
			push edx
			push ecx

			lea eax,[esp+0x0C]
			push eax
			call set_spawn_point
			add esp,0x04

			pop ecx
			pop edx

			jmp dword ptr [CUserLocal__Init]
		}
	}

	void set_spawncontrol_hooks(bool enable)
	{
		detours::redirect(enable, reinterpret_cast<void**>(&CUserLocal__Init), CUserLocal__Init_hook);
	}
}