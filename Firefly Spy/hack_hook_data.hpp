#pragma once

#include "native.hpp"
#include "addresses.hpp"

#include "context_objects.hpp"
#include "mob_objects.hpp"
#include "packet_objects.hpp"

namespace firefly
{
	/* Godmodes */
	typedef void (__fastcall* CUserLocal__SetDamaged_t)(void* ecx, void* edx, int nDamage, int vx, int vy, int nIndex, unsigned int dwObstacleData, CMob *pMob, int nAttackIdx, int nDir, int bCheckHitRemain, int bSendPacket, int nDistributeCount);
	static CUserLocal__SetDamaged_t CUserLocal__SetDamaged = reinterpret_cast<CUserLocal__SetDamaged_t>(CUserLocal__SetDamaged_address);
		
	/* Character Hacks */
	static unsigned char* CAvatar__CAvatar = reinterpret_cast<unsigned char*>(CAvatar__CAvatar_address);
	
	typedef int (__fastcall* CUser__IsOnFoothold_t)(void* ecx, void* edx);
	static CUser__IsOnFoothold_t CUser__IsOnFoothold = reinterpret_cast<CUser__IsOnFoothold_t>(CUser__IsOnFoothold_address);
	
	typedef int (__fastcall* CUser__IsOnLadderOrRope_t)(void* ecx, void* edx);
	static CUser__IsOnLadderOrRope_t CUser__IsOnLadderOrRope = reinterpret_cast<CUser__IsOnLadderOrRope_t>(CUser__IsOnLadderOrRope_address);
	
	typedef void (__fastcall* CVecCtrl__SetImpactNext_t)(void* ecx, void* edx, int nAttr, int nValue1, int nValue2, short vx, short vy);
	static CVecCtrl__SetImpactNext_t CVecCtrl__SetImpactNext = reinterpret_cast<CVecCtrl__SetImpactNext_t>(CVecCtrl__SetImpactNext_address);
	
	typedef int (__fastcall* CAntiRepeat__TryRepeat_t)(CAntiRepeat* ecx, void* edx, int nX, int nY);
	static CAntiRepeat__TryRepeat_t CAntiRepeat__TryRepeat = reinterpret_cast<CAntiRepeat__TryRepeat_t>(CAntiRepeat__TryRepeat_address);

	/* Mob Hacks */
	static unsigned char* CMob__Update = reinterpret_cast<unsigned char*>(CMob__Update_address);
	static unsigned char* CMob__Update_disarm = reinterpret_cast<unsigned char*>(CMob__Update_disarm_address);
	
	static unsigned char* CMovePath__Flush = reinterpret_cast<unsigned char*>(CMovePath__Flush_address);
	
	static unsigned char* CMob__OnSpawn = reinterpret_cast<unsigned char*>(CMob__OnSpawn_address);
	
	typedef void (__fastcall* CMob__OnDie_t)(void* ecx, void* edx);
	static CMob__OnDie_t CMob__OnDie = reinterpret_cast<CMob__OnDie_t>(CMob__OnDie_address);

	typedef void (__fastcall* CMob__AddDamageInfo_t)(void* ecx, void* edx, unsigned int dwCharacterId, int nSkillID, int nSLV, int tDelayedProcess, int nHitAction, int bLeft, __int64 nDamage, int bCriticalAttack, int nAttackIdx, int nMultipleBySkill, POINT pt, void* sHitAni, int bChase, bool bLifting, int nMoveType, int nBulletCashItemID, int nMoveEndingPosX, int nMoveEndingPosY, int bMoveLeft, int eRandSkill, bool bAssist, bool bDot, void* pHitPart, int nSummonAttackIdx, int nRandomHitOffset, void* unknown1, void* unknown2);
	static CMob__AddDamageInfo_t CMob__AddDamageInfo = reinterpret_cast<CMob__AddDamageInfo_t>(CMob__AddDamageInfo_address);

	typedef void (__cdecl* AccSpeed_t)(long double* v, long double f, long double m, long double vMax, long double tSec);
	static AccSpeed_t AccSpeed = reinterpret_cast<AccSpeed_t>(AccSpeed_address);
	
	typedef int (__fastcall* CVecCtrlMob__WorkUpdateActive_t)(CVecCtrlMob* ecx, void* edx, int tElapse);
	static CVecCtrlMob__WorkUpdateActive_t CVecCtrlMob__WorkUpdateActive = reinterpret_cast<CVecCtrlMob__WorkUpdateActive_t>(CVecCtrlMob__WorkUpdateActive_address);
	
	/* CPU Hacks */
	typedef void (__fastcall* CMapLoadable__RestoreBack_t)(void* ecx, void* edx, bool bLoad);
	static CMapLoadable__RestoreBack_t CMapLoadable__RestoreBack = reinterpret_cast<CMapLoadable__RestoreBack_t>(CMapLoadable__RestoreBack_address);
	
	typedef void (__fastcall* CMapLoadable__RestoreTile_t)(void* ecx, void* edx);
	static CMapLoadable__RestoreTile_t CMapLoadable__RestoreTile = reinterpret_cast<CMapLoadable__RestoreTile_t>(CMapLoadable__RestoreTile_address);
		
	typedef void (__fastcall* CMapLoadable__RestoreObj_t)(void* ecx, void* edx, bool bLoad);
	static CMapLoadable__RestoreObj_t CMapLoadable__RestoreObj = reinterpret_cast<CMapLoadable__RestoreObj_t>(CMapLoadable__RestoreObj_address);
		
	typedef void (__fastcall* CStage__FadeIn_t)(void* ecx, void* edx, int tForceDuration);
	static CStage__FadeIn_t CStage__FadeIn = reinterpret_cast<CStage__FadeIn_t>(CStage__FadeIn_address);
		
	typedef void (__fastcall* CStage__FadeOut_t)(void* ecx, void* edx, int tForceDuration);
	static CStage__FadeOut_t CStage__FadeOut = reinterpret_cast<CStage__FadeOut_t>(CStage__FadeOut_address);

	typedef void (__fastcall* CMob__ShowDamage_t)(void* ecx, void* edx, int nDamage, CMob::ShowDamageFlag* flag, POINT* const pPtReplace, void* unknown);
	static CMob__ShowDamage_t CMob__ShowDamage = reinterpret_cast<CMob__ShowDamage_t>(CMob__ShowDamage_address);
		
	typedef void (__fastcall* CWvsContext__OnStylishKillMessage_t)(void* ecx, void* edx, CInPacket* iPacket);
	static CWvsContext__OnStylishKillMessage_t CWvsContext__OnStylishKillMessage = reinterpret_cast<CWvsContext__OnStylishKillMessage_t>(CWvsContext__OnStylishKillMessage_address);
	
	/* Automatics */
	typedef void (__fastcall* CStage__OnSetCashShop_t)(void* ecx, void* edx, CInPacket *iPacket);
	static CStage__OnSetCashShop_t CStage__OnSetCashShop = reinterpret_cast<CStage__OnSetCashShop_t>(CStage__OnSetCashShop_address);

	typedef void (__fastcall* CCashShop__SendTransferFieldPacket_t)(void* ecx, void* edx);
	static CCashShop__SendTransferFieldPacket_t CCashShop__SendTransferFieldPacket = reinterpret_cast<CCashShop__SendTransferFieldPacket_t>(CCashShop__SendTransferFieldPacket_address);

	/* Miscellaneous */
	typedef int (__cdecl* CUtilDlg__Notice_t)(ZXString<char> sMsg, const wchar_t* sSndName, ZRef<void*>* ppDialog, int bAutoSeparated, int bIsTightLine, int bOnInGameDirection, int nRelZ);
	static CUtilDlg__Notice_t CUtilDlg__Notice = reinterpret_cast<CUtilDlg__Notice_t>(CUtilDlg__Notice_address);
	
	typedef int (__cdecl* CUtilDlg__YesNo_1_t)(ZXString<char> sMsg, int bCenter, int bFixedW);
	static CUtilDlg__YesNo_1_t CUtilDlg__YesNo_1 = reinterpret_cast<CUtilDlg__YesNo_1_t>(CUtilDlg__YesNo_1_address);

	typedef int (__cdecl* CUtilDlg__YesNo_2_t)(ZXString<char> sMsg, const wchar_t *sSndName, ZRef<void*>* ppDialog, int bAutoSeparated, int bNoEnterInput, int nWaitTime, int bAskButton);
	static CUtilDlg__YesNo_2_t CUtilDlg__YesNo_2 = reinterpret_cast<CUtilDlg__YesNo_2_t>(CUtilDlg__YesNo_2_address);

	typedef void (__fastcall* CField__OnCreateObtacle_t)(void* ecx, void* edx, CInPacket* iPacket);
	static CField__OnCreateObtacle_t CField__OnCreateObtacle = reinterpret_cast<CField__OnCreateObtacle_t>(CField__OnCreateObtacle_address);
		
	typedef void (__fastcall* CField__OnCreateFallingCatcher_t)(void* ecx, void* edx, CInPacket* iPacket);
	static CField__OnCreateFallingCatcher_t CField__OnCreateFallingCatcher = reinterpret_cast<CField__OnCreateFallingCatcher_t>(CField__OnCreateFallingCatcher_address);
	
	typedef void (__fastcall* CWvsContext__SetExclRequestSent_t)(void* ecx, void* edx, int bSent);
	static CWvsContext__SetExclRequestSent_t CWvsContext__SetExclRequestSent = reinterpret_cast<CWvsContext__SetExclRequestSent_t>(CWvsContext__SetExclRequestSent_address);

	typedef POINT* (__fastcall* CMob__GetPos_t)(void* ecx, void* edx, POINT* pt);
	static CMob__GetPos_t CMob__GetPos = reinterpret_cast<CMob__GetPos_t>(CMob__GetPos_address);
	
	typedef void (__fastcall* CMonsterFamiliar__TryDoingAttack_t)(void* ecx, void* edx);
	static CMonsterFamiliar__TryDoingAttack_t CMonsterFamiliar__TryDoingAttack = reinterpret_cast<CMonsterFamiliar__TryDoingAttack_t>(CMonsterFamiliar__TryDoingAttack_address);
}