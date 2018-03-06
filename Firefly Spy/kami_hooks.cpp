#include "kami_hooks.hpp"
#include "kami_hook_data.hpp"

#include "kami_data.hpp"
#include "detours.hpp"

#include "drop_objects.hpp"
#include "mob_objects.hpp"
#include "portal_objects.hpp"

#include <algorithm>
#include <functional>

namespace firefly
{
	void perform_teleport(CVecCtrl* userlocal_vecctrl, POINT pt)
	{
		if (kami_data::get_instance().movement_type() == kami_movement_type_ids::teleport)
		{
			CVecCtrl__SetTeleportNext(userlocal_vecctrl, nullptr, 0, pt.x, pt.y);
		}
		else if (kami_data::get_instance().movement_type() == kami_movement_type_ids::portal)
		{
			/* Possible implementation for later */
		}
	}

	bool Hook_CUserLocal__GetJobCode(bool enable)
	{
		CUserLocal__GetJobCode_t CUserLocal__GetJobCode_hook = [](void* ecx, void* edx) -> int
		{
			if (kami_data::get_instance().enabled())
			{
				CVecCtrl* userlocal_vecctrl = CUserLocal::GetInstance()->GetVecCtrl();

				POINT pt_user = { static_cast<LONG>(userlocal_vecctrl->m_ap.x), static_cast<LONG>(userlocal_vecctrl->m_ap.y) };
				POINT pt_target = { 0, 0 };
				
				if (CDropPool::IsInitialized())
				{
					if (kami_data::get_instance().is_item_looting())
					{
						if (CDropPool::size() <= 1)
							kami_data::get_instance().set_item_looting(false);
						else
						{
							bool fetched_loot = false;

							if (kami_data::get_instance().item_loot_target_type() == kami_target_type_ids::random)
								fetched_loot = CDropPool::GetInstance()->get_random_drop(&pt_target);
							else
								fetched_loot = CDropPool::GetInstance()->get_closest_drop(&pt_target, pt_user);

							if (fetched_loot)
								perform_teleport(userlocal_vecctrl, pt_target);
							else
								kami_data::get_instance().set_item_looting(false);
						}
					}
					else
					{
						if (CDropPool::size() > kami_data::get_instance().item_loot_value())
							kami_data::get_instance().set_item_looting(true);
						else
						{
							if (CWvsPhysicalSpace2D::IsInitialized())
								CWvsPhysicalSpace2D::GetInstance()->GetTopLeft(&pt_target);

							if (CMobPool::IsInitialized() && CMobPool::size() > 0)
							{
								bool fetched_mob = false;

								if (kami_data::get_instance().target_type() == kami_target_type_ids::random)
									fetched_mob = CMobPool::GetInstance()->get_random_mob(&pt_target);
								else 
									fetched_mob = CMobPool::GetInstance()->get_closest_mob(&pt_target, pt_user);

								if (fetched_mob)
								{
									pt_target.x += kami_data::get_instance().horizontal_range() * (pt_user.x >= pt_target.x ? 1 : -1);
									pt_target.y += kami_data::get_instance().vertical_range();

									//userlocal_vecctrl->m_nMoveAction = (pt_user.x >= pt_mob.x ? 7 : 6);
								}
							}
							else if (CDropPool::size() > 0)
							{
								if (kami_data::get_instance().item_loot_target_type() == kami_target_type_ids::random)
									CDropPool::GetInstance()->get_random_drop(&pt_target);
								else
									CDropPool::GetInstance()->get_closest_drop(&pt_target, pt_user);
							}

							perform_teleport(userlocal_vecctrl, pt_target);
						}
					}
				}
			}

			return CUserLocal__GetJobCode(ecx, edx);
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&CUserLocal__GetJobCode), CUserLocal__GetJobCode_hook);
	}

	void set_kami_hooks(bool enable)
	{
		Hook_CUserLocal__GetJobCode(enable);
	}
}