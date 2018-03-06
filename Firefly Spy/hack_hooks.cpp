#include "hack_hooks.hpp"
#include "hack_hook_data.hpp"

#include "hack_data.hpp"
#include "exclusive_data.hpp"
#include "detours.hpp"

#include "life_objects.hpp"

namespace firefly
{
	/* Godmodes */
	bool Hook_CUserLocal__SetDamaged(bool enable)
	{
		CUserLocal__SetDamaged_t CUserLocal__SetDamaged_hook = [](void* ecx, void* edx, int nDamage, int vx, int vy, int nIndex, unsigned int dwObstacleData, CMob *pMob, int nAttackIdx, int nDir, int bCheckHitRemain, int bSendPacket, int nDistributeCount) -> void
		{
			if (hack_data::get_instance().enabled())
			{
				if (hack_data::get_instance().is_hack_enabled(hack_type::full_godmode))
				{
					return;
				}
				else if (hack_data::get_instance().is_hack_enabled(hack_type::s58_godmode))
				{
					static int last_tick = 0;
					int current_tick = GetTickCount();

					if (current_tick - last_tick >= 40000)
					{
						last_tick = current_tick;
					}
					else
					{
						return;
					}
				}
			}

			return CUserLocal__SetDamaged(ecx, edx, nDamage, vx, vy, nIndex, dwObstacleData, pMob, nAttackIdx, nDir, bCheckHitRemain, bSendPacket, nDistributeCount);
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&CUserLocal__SetDamaged), CUserLocal__SetDamaged_hook);
	}

	/* Face Left */
	bool is_face_left_enabled()
	{
		return (hack_data::get_instance().enabled() && hack_data::get_instance().is_hack_enabled(hack_type::face_left));
	}

	void __declspec(naked) CAvatar__CAvatar_hook()
	{
		__asm
		{
			push eax
			call is_face_left_enabled
			movzx eax,al
			test eax,eax
			pop eax
			jz return_original
			
			mov eax,0x00000001

			return_original:
			jmp dword ptr [CAvatar__CAvatar]
		}
	}

	bool Hook_CAvatar__CAvatar(bool enable)
	{
		return detours::redirect(enable, reinterpret_cast<void**>(&CAvatar__CAvatar), CAvatar__CAvatar_hook);
	}

	/* Aircheck */
	bool Hook_CUser__IsOnFoothold(bool enable)
	{
		CUser__IsOnFoothold_t CUser__IsOnFoothold_hook = [](void* ecx, void* edx) -> int
		{
			if (hack_data::get_instance().enabled() && hack_data::get_instance().is_hack_enabled(hack_type::aircheck))
				return TRUE;

			return CUser__IsOnFoothold(ecx, edx);
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&CUser__IsOnFoothold), CUser__IsOnFoothold_hook);
	}
	
	bool Hook_CUser__IsOnLadderOrRope(bool enable)
	{
		CUser__IsOnLadderOrRope_t CUser__IsOnLadderOrRope_hook = [](void* ecx, void* edx) -> int
		{
			if (hack_data::get_instance().enabled() && hack_data::get_instance().is_hack_enabled(hack_type::aircheck))
				return FALSE;

			return CUser__IsOnLadderOrRope(ecx, edx);
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&CUser__IsOnLadderOrRope), CUser__IsOnLadderOrRope_hook);
	}

	/* Character No Knockback */
	bool Hook_CVecCtrl__SetImpactNext(bool enable)
	{
		CVecCtrl__SetImpactNext_t CVecCtrl__SetImpactNext_hook = [](void* ecx, void* edx, int nAttr, int nValue1, int nValue2, short vx, short vy) -> void
		{
			if (hack_data::get_instance().enabled() && hack_data::get_instance().is_hack_enabled(hack_type::no_character_knockback))
				return;

			return CVecCtrl__SetImpactNext(ecx, edx, nAttr, nValue1, nValue2, vx, vy);
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&CVecCtrl__SetImpactNext), CVecCtrl__SetImpactNext_hook);
	}
	
	/* Unlimited Attack */
	bool Hook_CAntiRepeat__TryRepeat(bool enable)
	{
		CAntiRepeat__TryRepeat_t CAntiRepeat__TryRepeat_hook = [](CAntiRepeat* ecx, void* edx, int nX, int nY) -> int
		{
			if (hack_data::get_instance().enabled() && hack_data::get_instance().is_hack_enabled(hack_type::unlimited_attack))
			{
				if (CUserLocal::IsInitialized() && ecx->m_nRepeatCount >= 90)
				{
					static bool left = false;
					CUserLocal::GetInstance()->GetVecCtrl()->SetImpactNext(43, ((left ^= true) ? -270 : 270), 0);
				}
			}

			return CAntiRepeat__TryRepeat(ecx, edx, nX, nY);
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&CAntiRepeat__TryRepeat), CAntiRepeat__TryRepeat_hook);
	}
	
	/* Mob Full Disarm */
	bool is_full_disarm_enabled()
	{
		return (hack_data::get_instance().enabled() && hack_data::get_instance().is_hack_enabled(hack_type::full_disarm));
	}

	void __declspec(naked) CMob__Update_hook()
	{
		__asm
		{
			pushfd
			call is_full_disarm_enabled
			movzx eax,al
			test eax,eax
			jz return_original
			
			popfd
			jmp dword ptr [CMob__Update_disarm]

			return_original:
			popfd
			jmp dword ptr [CMob__Update]
		}
	}

	bool Hook_CMob__Update(bool enable)
	{
		return detours::redirect(enable, reinterpret_cast<void**>(&CMob__Update), CMob__Update_hook);
	}
	
	/* Faster Mobs */
	bool is_faster_mobs_enabled()
	{
		return (hack_data::get_instance().enabled() && hack_data::get_instance().is_hack_enabled(hack_type::faster_mobs));
	}
	
	void __declspec(naked) CMovePath__Flush_hook()
	{
		__asm
		{
			pushfd
			call is_faster_mobs_enabled
			movzx eax,al
			test eax,eax
			mov eax,[CMovePath__Flush]
			jz return_original
			
			add eax,0x06				// size of JNE FAR instruction (0F 85 XX XX XX XX)

			return_original:
			popfd
			jmp eax
		}
	}

	bool Hook_CMovePath__Flush(bool enable)
	{
		return detours::redirect(enable, reinterpret_cast<void**>(&CMovePath__Flush), CMovePath__Flush_hook);
	}
	
	/* Mob No Animation */
	bool is_no_mob_animation_enabled()
	{
		return (hack_data::get_instance().enabled() && hack_data::get_instance().is_hack_enabled(hack_type::no_mob_animation));
	}
	
	void __declspec(naked) CMob__OnSpawn_hook()
	{
		__asm
		{
			pushfd
			call is_no_mob_animation_enabled
			movzx eax,al
			test eax,eax
			mov eax,[CMob__OnSpawn]
			jz return_original
			
			add eax,0x06				// size of JGE FAR instruction (0F 8E XX XX XX XX)

			return_original:
			popfd
			jmp eax
		}
	}

	bool Hook_CMob__OnSpawn(bool enable)
	{
		return detours::redirect(enable, reinterpret_cast<void**>(&CMob__OnSpawn), CMob__OnSpawn_hook);
	}
	
	bool Hook_CMob__OnDie(bool enable)
	{
		CMob__OnDie_t CMob__OnDie_hook = [](void* ecx, void* edx) -> void
		{
			if (hack_data::get_instance().enabled() && hack_data::get_instance().is_hack_enabled(hack_type::no_mob_animation))
				return;

			return CMob__OnDie(ecx, edx);
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&CMob__OnDie), CMob__OnDie_hook);
	}

	/* Mob No Knockback */
	bool Hook_CMob__AddDamageInfo(bool enable)
	{
		CMob__AddDamageInfo_t CMob__AddDamageInfo_hook = [](void* ecx, void* edx, unsigned int dwCharacterId, int nSkillID, int nSLV, int tDelayedProcess, int nHitAction, int bLeft, __int64 nDamage,	int bCriticalAttack, int nAttackIdx, int nMultipleBySkill, POINT pt, void* sHitAni, int bChase, bool bLifting, int nMoveType, int nBulletCashItemID, int nMoveEndingPosX, int nMoveEndingPosY, int bMoveLeft, int eRandSkill, bool bAssist, bool bDot, void* pHitPart, int nSummonAttackIdx, int nRandomHitOffset, void* unknown1, void* unknown2) -> void
		{
			if (hack_data::get_instance().enabled())
			{
				if (hack_data::get_instance().is_hack_enabled(hack_type::no_mob_knockback))
					nMoveType = 0;

				if (hack_data::get_instance().is_hack_enabled(hack_type::no_hit_effect))
					sHitAni = NULL;
			}

			return CMob__AddDamageInfo(ecx, edx, dwCharacterId, nSkillID, nSLV, tDelayedProcess, nHitAction, bLeft, nDamage, bCriticalAttack, nAttackIdx, nMultipleBySkill, pt, sHitAni, bChase, bLifting, nMoveType, nBulletCashItemID, nMoveEndingPosX, nMoveEndingPosY, bMoveLeft, eRandSkill, bAssist, bDot, pHitPart, nSummonAttackIdx, nRandomHitOffset, unknown1, unknown2);
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&CMob__AddDamageInfo), CMob__AddDamageInfo_hook);
	}
	
	/* Mob Suck Left, Mob Suck Right */
	bool Hook_AccSpeed(bool enable)
	{
		AccSpeed_t AccSpeed_hook = [](long double* v, long double f, long double m, long double vMax, long double tSec) -> void
		{
			if (vMax >= 0.0)
			{
				if (f > 0.0)
				{
					if (vMax > *v)
					{
						*v = (f / m) * tSec + *v;
							
						if (!hack_data::get_instance().enabled() || !hack_data::get_instance().is_hack_enabled(hack_type::suck_right))
						{
							if (*v > vMax)
								*v = vMax;
						}
					}
				}
				else if (-vMax < *v)
				{
					*v = (f / m) * tSec + *v;
					
					if (!hack_data::get_instance().enabled() || !hack_data::get_instance().is_hack_enabled(hack_type::suck_left))
					{
						if (*v < -vMax)
							*v = -vMax;
					}
				}
			}
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&AccSpeed), AccSpeed_hook);
	}
	
	/* Mob Auto Aggro, Mob Freeze */
	bool Hook_CVecCtrlMob__WorkUpdateActive(bool enable)
	{
		CVecCtrlMob__WorkUpdateActive_t CVecCtrlMob__WorkUpdateActive_hook = [](CVecCtrlMob* ecx, void* edx, int tElapse) -> int
		{
			enum
			{
				MOVEABILITY_STOP = 0x0,
				MOVEABILITY_WALK = 0x1,
				MOVEABILITY_JUMP = 0x2,
				MOVEABILITY_FLY = 0x3,
				MOVEABILITY_ESCORT = 0x4,
				MOVEABILITY_FORWARD = 0x5,
				MOVEABILITY_COUNT = 0x6,
			};

			if (exclusive_data::get_instance().enabled() && exclusive_data::get_instance().enable_dupex()) 
			{
				ecx->m_nMoveAbility = MOVEABILITY_ESCORT;
			}
			else if (hack_data::get_instance().enabled())
			{
				if (hack_data::get_instance().is_hack_enabled(hack_type::mob_freeze))
				{
					ecx->m_nMoveAbility = MOVEABILITY_ESCORT;
				}
				else if (hack_data::get_instance().is_hack_enabled(hack_type::auto_aggro))
				{
					if (CUserLocal::IsInitialized())
						ecx->m_pTarget = CUserLocal::GetInstance();
				}
			}

			return CVecCtrlMob__WorkUpdateActive(ecx, edx, tElapse);
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&CVecCtrlMob__WorkUpdateActive), CVecCtrlMob__WorkUpdateActive_hook);
	}

	/* No Background */
	bool Hook_CMapLoadable__RestoreBack(bool enable)
	{
		CMapLoadable__RestoreBack_t CMapLoadable__RestoreBack_hook = [](void* ecx, void* edx, bool bLoad) -> void
		{
			if (hack_data::get_instance().enabled() && hack_data::get_instance().is_hack_enabled(hack_type::no_background))
				return;

			return CMapLoadable__RestoreBack(ecx, edx, bLoad);
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&CMapLoadable__RestoreBack), CMapLoadable__RestoreBack_hook);
	}

	/* No Platforms */
	bool Hook_CMapLoadable__RestoreTile(bool enable)
	{
		CMapLoadable__RestoreTile_t CMapLoadable__RestoreTile_hook = [](void* ecx, void* edx) -> void
		{
			if (hack_data::get_instance().enabled() && hack_data::get_instance().is_hack_enabled(hack_type::no_structures_platforms))
				return;

			return CMapLoadable__RestoreTile(ecx, edx);
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&CMapLoadable__RestoreTile), CMapLoadable__RestoreTile_hook);
	}

	/* No Structures */
	bool Hook_CMapLoadable__RestoreObj(bool enable)
	{
		CMapLoadable__RestoreObj_t CMapLoadable__RestoreObj_hook = [](void* ecx, void* edx, bool bLoad) -> void
		{
			if (hack_data::get_instance().enabled() && hack_data::get_instance().is_hack_enabled(hack_type::no_structures_platforms))
				return;

			return CMapLoadable__RestoreObj(ecx, edx, bLoad);
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&CMapLoadable__RestoreObj), CMapLoadable__RestoreObj_hook);
	}

	/* No Fadestages (Fade In) */
	bool Hook_CStage__FadeIn(bool enable)
	{
		CStage__FadeIn_t CStage__FadeIn_hook = [](void* ecx, void* edx, int tForceDuration) -> void
		{
			if (hack_data::get_instance().enabled() && hack_data::get_instance().is_hack_enabled(hack_type::no_fadestages))
				return;

			return CStage__FadeIn(ecx, edx, tForceDuration);
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&CStage__FadeIn), CStage__FadeIn_hook);
	}
	
	bool Hook_CStage__FadeOut(bool enable)
	{
		CStage__FadeOut_t CStage__FadeOut_hook = [](void* ecx, void* edx, int tForceDuration) -> void
		{
			if (hack_data::get_instance().enabled() && hack_data::get_instance().is_hack_enabled(hack_type::no_fadestages))
				return;

			return CStage__FadeOut(ecx, edx, tForceDuration);
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&CStage__FadeOut), CStage__FadeOut_hook);
	}

	/* No Damage Text */
	bool Hook_CMob__ShowDamage(bool enable)
	{
		CMob__ShowDamage_t CMob__ShowDamage_hook = [](void* ecx, void* edx, int nDamage, CMob::ShowDamageFlag* flag, POINT* const pPtReplace, void* unknown) -> void
		{
			if (hack_data::get_instance().enabled() && hack_data::get_instance().is_hack_enabled(hack_type::no_damage_text))
				return;

			return CMob__ShowDamage(ecx, edx, nDamage, flag, pPtReplace, unknown);
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&CMob__ShowDamage), CMob__ShowDamage_hook);
	}

	/* No Kill Text */
	bool Hook_CWvsContext__OnStylishKillMessage(bool enable)
	{
		CWvsContext__OnStylishKillMessage_t CWvsContext__OnStylishKillMessage_hook = [](void* ecx, void* edx, CInPacket* iPacket) -> void
		{
			if (hack_data::get_instance().enabled() && hack_data::get_instance().is_hack_enabled(hack_type::no_kill_text))
				return;

			return CWvsContext__OnStylishKillMessage(ecx, edx, iPacket);
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&CWvsContext__OnStylishKillMessage), CWvsContext__OnStylishKillMessage_hook);
	}
	
	/* Auto NPC */

	/* Auto Exit CashShop */
	bool Hook_CStage__OnSetCashShop(bool enable)
	{
		CStage__OnSetCashShop_t CStage__OnSetCashShop_hook = [](void* ecx, void* edx, CInPacket *iPacket) -> void
		{
			CStage__OnSetCashShop(ecx, edx, iPacket);
			
			if (hack_data::get_instance().enabled() && hack_data::get_instance().is_hack_enabled(hack_type::auto_exit_cashshop))
			{
				// 'this' isn't used.
				CCashShop__SendTransferFieldPacket(nullptr, nullptr); 
			}
		};
		
		return detours::redirect(enable, reinterpret_cast<void**>(&CStage__OnSetCashShop), CStage__OnSetCashShop_hook);
	}

	/* No Blue Boxes */
	bool Hook_CUtilDlg__Notice(bool enable)
	{
		CUtilDlg__Notice_t CUtilDlg__Notice_hook = [](ZXString<char> sMsg, const wchar_t* sSndName, ZRef<void*>* ppDialog, int bAutoSeparated, int bIsTightLine, int bOnInGameDirection, int nRelZ) -> int
		{
			if (hack_data::get_instance().enabled() && hack_data::get_instance().is_hack_enabled(hack_type::no_blue_boxes))
				return 0;

			return CUtilDlg__Notice(sMsg, sSndName, ppDialog, bAutoSeparated, bIsTightLine, bOnInGameDirection, nRelZ);
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&CUtilDlg__Notice), CUtilDlg__Notice_hook);
	}

	bool Hook_CUtilDlg__YesNo_1(bool enable)
	{
		CUtilDlg__YesNo_1_t CUtilDlg__YesNo_1_hook = [](ZXString<char> sMsg, int bCenter, int bFixedW) -> int
		{
			if (hack_data::get_instance().enabled() && hack_data::get_instance().is_hack_enabled(hack_type::no_blue_boxes))
				return 6;

			return CUtilDlg__YesNo_1(sMsg, bCenter, bFixedW);
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&CUtilDlg__YesNo_1), CUtilDlg__YesNo_1_hook);
	}

	bool Hook_CUtilDlg__YesNo_2(bool enable)
	{
		CUtilDlg__YesNo_2_t CUtilDlg__YesNo_2_hook = [](ZXString<char> sMsg, const wchar_t* sSndName, ZRef<void*>* ppDialog, int bAutoSeparated, int bNoEnterInput, int nWaitTime, int bAskButton) -> int
		{
			if (hack_data::get_instance().enabled() && hack_data::get_instance().is_hack_enabled(hack_type::no_blue_boxes))
				return 6;

			return CUtilDlg__YesNo_2(sMsg, sSndName, ppDialog, bAutoSeparated, bNoEnterInput, nWaitTime, bAskButton);
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&CUtilDlg__YesNo_2), CUtilDlg__YesNo_2_hook);
	}

	/* No Falling Objects */
	bool Hook_CField__OnCreateObtacle(bool enable)
	{
		CField__OnCreateObtacle_t CField__OnCreateObtacle_hook = [](void* ecx, void* edx, CInPacket* iPacket) -> void
		{
			if (hack_data::get_instance().enabled() && hack_data::get_instance().is_hack_enabled(hack_type::no_falling_objects))
				return;

			return CField__OnCreateObtacle(ecx, edx, iPacket);
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&CField__OnCreateObtacle), CField__OnCreateObtacle_hook);
	}

	bool Hook_CField__OnCreateFallingCatcher(bool enable)
	{
		CField__OnCreateFallingCatcher_t CField__OnCreateFallingCatcher_hook = [](void* ecx, void* edx, CInPacket* iPacket) -> void
		{
			if (hack_data::get_instance().enabled() && hack_data::get_instance().is_hack_enabled(hack_type::no_falling_objects))
				return;

			return CField__OnCreateFallingCatcher(ecx, edx, iPacket);
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&CField__OnCreateFallingCatcher), CField__OnCreateFallingCatcher_hook);
	}
	
	/* Super Tubi */
	bool Hook_CWvsContext__SetExclRequestSent(bool enable)
	{
		CWvsContext__SetExclRequestSent_t CWvsContext__SetExclRequestSent_hook = [](void* ecx, void* edx, int bSent) -> void
		{
			if (hack_data::get_instance().enabled() && hack_data::get_instance().is_hack_enabled(hack_type::super_tubi))
				return CWvsContext__SetExclRequestSent(ecx, edx, 0);

			return CWvsContext__SetExclRequestSent(ecx, edx, bSent);
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&CWvsContext__SetExclRequestSent), CWvsContext__SetExclRequestSent_hook);
	}
	
	/* Semi Itemvac */
	bool Hook_CMob__GetPos(bool enable)
	{
		CMob__GetPos_t CMob__GetPos_hook = [](void* ecx, void* edx, POINT* pt) -> POINT*
		{
			if (hack_data::get_instance().enabled() && hack_data::get_instance().is_hack_enabled(hack_type::semi_itemvac))
			{
				if (CUserLocal::IsInitialized())
					return CUserLocal::GetInstance()->GetPos(pt);
			}

			return CMob__GetPos(ecx, edx, pt);
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&CMob__GetPos), CMob__GetPos_hook);
	}
	
	/* Familiar Disarm */
	bool Hook_CMonsterFamiliar__TryDoingAttack(bool enable)
	{
		CMonsterFamiliar__TryDoingAttack_t CMonsterFamiliar__TryDoingAttack_hook = [](void* ecx, void* edx) -> void
		{
			if (hack_data::get_instance().enabled() && hack_data::get_instance().is_hack_enabled(hack_type::familiar_disarm))
				return;

			return CMonsterFamiliar__TryDoingAttack(ecx, edx);
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&CMonsterFamiliar__TryDoingAttack), CMonsterFamiliar__TryDoingAttack_hook);
	}

	void set_hack_hooks(bool enable)
	{
		/* Godmodes */
		Hook_CUserLocal__SetDamaged(enable);

		/* Character Hacks */
		Hook_CAvatar__CAvatar(enable);
		Hook_CUser__IsOnFoothold(enable);
		Hook_CUser__IsOnLadderOrRope(enable);
		Hook_CVecCtrl__SetImpactNext(enable);
		Hook_CAntiRepeat__TryRepeat(enable);

		/* Mob Hacks */
		Hook_CMob__Update(enable);
		Hook_CMovePath__Flush(enable);
		Hook_CMob__OnSpawn(enable);
		Hook_CMob__OnDie(enable);
		Hook_CMob__AddDamageInfo(enable);
		Hook_AccSpeed(enable);
		Hook_CVecCtrlMob__WorkUpdateActive(enable);

		/* CPU Hacks */
		Hook_CMapLoadable__RestoreBack(enable);
		Hook_CMapLoadable__RestoreTile(enable);
		Hook_CMapLoadable__RestoreObj(enable);
		Hook_CStage__FadeIn(enable);
		Hook_CStage__FadeOut(enable);
		Hook_CMob__ShowDamage(enable);
		Hook_CWvsContext__OnStylishKillMessage(enable);

		/* Automatics */
		Hook_CStage__OnSetCashShop(enable);

		/* Miscellaneous */
		Hook_CUtilDlg__Notice(enable);
		Hook_CUtilDlg__YesNo_1(enable);
		Hook_CUtilDlg__YesNo_2(enable);
		Hook_CField__OnCreateObtacle(enable);
		Hook_CField__OnCreateFallingCatcher(enable);
		Hook_CWvsContext__SetExclRequestSent(enable);
		Hook_CMob__GetPos(enable);
		Hook_CMonsterFamiliar__TryDoingAttack(enable);
	}
}