#include "exclusive_hooks.hpp"
#include "exclusive_hook_data.hpp"
#include "kami_hook_data.hpp"

#include "autobotting_data.hpp"
#include "exclusive_data.hpp"
#include "detours.hpp"

#include "context_objects.hpp"
#include "drop_objects.hpp"
#include "skill_objects.hpp"

#ifdef _PRODUCTION
#include <VMProtectSDK.h>
#endif

namespace firefly
{
	/* Pet Loot */
	bool Hook_CVecCtrlPet__WorkUpdateActive(bool enable)
	{
		CDropPool__TryPickUpDropByPet_t CDropPool__TryPickUpDropByPet_hook = [](void* ecx, void* edx, CPet* pPet, POINT* ptPos, SECPOINT* ptPosCheck) -> void
		{
			if (exclusive_data::get_instance().enabled() && exclusive_data::get_instance().enable_pet_loot() && pPet != nullptr)
			{
				if (CDropPool::IsInitialized() && (GetTickCount() - exclusive_data::get_instance().pet_loot_last_tick(pPet->m_nPetIndex)) >= exclusive_data::get_instance().pet_loot_delay())
				{
					CVecCtrl* pet_vecctrl = pPet->GetVecCtrl();

					if (pet_vecctrl != nullptr)
					{
						POINT pt_drop = { 0, 0 };

						if (CDropPool::GetInstance()->get_drop(&pt_drop, pPet->m_nPetIndex))
							CVecCtrl__SetTeleportNext(pet_vecctrl, nullptr, 0, pt_drop.x, pt_drop.y);

						exclusive_data::get_instance().pet_loot_last_tick(pPet->m_nPetIndex) = GetTickCount();
					}
				}
			}
			
			return CDropPool__TryPickUpDropByPet(ecx, edx, pPet, ptPos, ptPosCheck);
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&CDropPool__TryPickUpDropByPet), CDropPool__TryPickUpDropByPet_hook);
	}

	/* Full Map Attack */
	bool Hook_CMobPool_FindHitMobInRect(bool enable)
	{
		CMobPool_FindHitMobInRect_t CMobPool_FindHitMobInRect_hook = [](void* ecx, void* edx, RECT* rc, CMob** apMob, int nMaxCount, CMob* pExcept, int nWishMobID, int rPoison, unsigned int dwWishTemplateID, int bIncludeDazzledMob, int bIncludeEscortMob, int nSelectRangeSkillID, int nSelectRangeX, int nCurUserX, int nSkillID, int bFindBoss, void* unknown) -> int
		{
			if (exclusive_data::get_instance().enabled() && exclusive_data::get_instance().enable_fullmap_attack())
			{
				if (CWvsPhysicalSpace2D::IsInitialized())
				{
					unsigned int skill_id = exclusive_data::get_instance().fullmap_attack_skill_id();

					if (nSkillID == skill_id || skill_id == -1 || ((skill_id == -2 && (nSkillID == 0 && nMaxCount == 15)) ||
						(skill_id == -3 && (nSkillID == 142001002 || nSkillID == 142111007 || nSkillID == 142121005))))
					{
						//CWvsPhysicalSpace2D::GetInstance()->GetBoundary(rc);
						return reinterpret_cast<CMobPool*>(ecx)->get_mobs(apMob, nMaxCount);
					}
				}
			}

			return CMobPool_FindHitMobInRect(ecx, edx, rc, apMob, nMaxCount, pExcept, nWishMobID, rPoison, dwWishTemplateID, bIncludeDazzledMob, bIncludeEscortMob, nSelectRangeSkillID, nSelectRangeX, nCurUserX, nSkillID, bFindBoss, unknown);
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&CMobPool_FindHitMobInRect), CMobPool_FindHitMobInRect_hook);
	}

	/* Skill Injection */
	bool Hook_CUserLocal__TryDoingFinalAttack(bool enable)
	{
		CUserLocal__TryDoingFinalAttack_t CUserLocal__TryDoingFinalAttack_hook = [](void* ecx, void* edx) -> void
		{
			CUserLocal__TryDoingFinalAttack(ecx, edx);

			if (exclusive_data::get_instance().enabled() && exclusive_data::get_instance().enable_skill_inject())
			{
				if (autobotting_data::get_instance().enabled())
				{
					if (autobotting_data::get_instance().is_people_refreshing() || autobotting_data::get_instance().is_timed_refreshing())
						return;
					else if ((GetTickCount() - autobotting_data::get_instance().last_refresh_tick()) < 1500)
						return;
					else if (autobotting_data::get_instance().enable_auto_familiar() && reinterpret_cast<CUserLocal*>(ecx)->is_familiar_inactive())
						return;
					else
					{
						for (int i = static_cast<int>(botting_type::auto_timed_skill_4); i >= static_cast<int>(botting_type::auto_attack); i--)
						{
							if (autobotting_data::get_instance().get_at(static_cast<botting_type>(i)).should_use(GetTickCount()))
								return;
						}
					}
				}

				if (CSkillInfo::IsInitialized() && CMobPool::size() > 0)
				{
					if ((GetTickCount() - exclusive_data::get_instance().skill_inject_last_tick()) >= exclusive_data::get_instance().skill_inject_delay())
					{
						void* skill_entry = 0;

						int skill_level = CSkillInfo::GetInstance()->get_skill(exclusive_data::get_instance().skill_inject_skill_id(), &skill_entry);

						if (skill_level != 0)
						{
							reinterpret_cast<CUserLocal*>(ecx)->try_doing_melee_attack(skill_entry, skill_level);
						}

						exclusive_data::get_instance().skill_inject_last_tick() = GetTickCount();
					}
				}
			}
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&CUserLocal__TryDoingFinalAttack), CUserLocal__TryDoingFinalAttack_hook);
	}
	
	/* DupeX */
	bool Hook_CMob__GenerateMovePath(bool enable)
	{
		CMob__GenerateMovePath_t CMob__GenerateMovePath_hook = [](void* ecx, void* edx, int nAction, int bLeft, CMob::TARGETINFO ti, int bChase, int nMoveType, int nMoveEndingX, int nMoveEndingY, int bMoveLeft, int bRiseByToss, int nTossHeightWeight, unsigned int eSkill, CUserLocal* pUserLocal, int _nRushDistance, bool bAirHit) -> void
		{
#ifdef _PRODUCTION
			VMProtectBeginVirtualization("CMob__GenerateMovePath_hook");
#endif
			if (exclusive_data::get_instance().enabled() && exclusive_data::get_instance().enable_dupex())
			{
				nAction = 7;
				bLeft = FALSE;
				bChase = FALSE;
				
				nMoveType = 23;
				nMoveEndingX = exclusive_data::get_instance().dupex_x();
				nMoveEndingY = exclusive_data::get_instance().dupex_y();
				bMoveLeft = FALSE;

				bRiseByToss = FALSE;
				nTossHeightWeight = 100;

				eSkill = 0;
				pUserLocal = NULL;
				_nRushDistance = 0;
				bAirHit = FALSE;
			}
#ifdef _PRODUCTION
			VMProtectEnd();
#endif
			return CMob__GenerateMovePath(ecx, edx, nAction, bLeft, ti, bChase, nMoveType, nMoveEndingX, nMoveEndingY, bMoveLeft, bRiseByToss, nTossHeightWeight, eSkill, pUserLocal, _nRushDistance, bAirHit);
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&CMob__GenerateMovePath), CMob__GenerateMovePath_hook);
	}

	void set_exclusive_hooks(bool enable)
	{
		Hook_CVecCtrlPet__WorkUpdateActive(enable);
		Hook_CMobPool_FindHitMobInRect(enable);
		Hook_CUserLocal__TryDoingFinalAttack(enable);
		Hook_CMob__GenerateMovePath(enable);
	}
}