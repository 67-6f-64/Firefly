#pragma once

#include "native.hpp"
#include "addresses.hpp"

#include "packet_objects.hpp"

namespace firefly
{
	typedef struct _DR_INFO
	{
		unsigned int dr0;
		unsigned int dr1;
		unsigned int dr2;
		unsigned int dr3;
		unsigned int chk;
	} DR_INFO;

	typedef struct __declspec(align(1)) _FUNCKEY_MAPPED
	{
		unsigned int id()
		{
			return *reinterpret_cast<unsigned int*>(&this->nID);
		}

		char nType;
		char nID[4];
	} FUNCKEY_MAPPED;

	static_assert_size(sizeof(FUNCKEY_MAPPED), 0x05);

	typedef int (__cdecl* DR_check_t)(DR_INFO* pDrInfo, unsigned int* pCheck, HINSTANCE hImageBase);
	static DR_check_t DR_check = reinterpret_cast<DR_check_t>(DR_check_address);
	
	static unsigned char* CLogo__UpdateLogo_skip = reinterpret_cast<unsigned char*>(CLogo__UpdateLogo_skip_address);

	typedef void (__fastcall* CMobPool__OnMobEnterField_t)(void* ecx, void* edx, CInPacket* iPacket);
	static CMobPool__OnMobEnterField_t CMobPool__OnMobEnterField = reinterpret_cast<CMobPool__OnMobEnterField_t>(CMobPool__OnMobEnterField_address);
	
	typedef void (__fastcall* CMobPool__SetLocalMob_t)(void* ecx, void* edx, int nLevel, unsigned int dwMobId, int nCalcDamageIndex, CInPacket* iPacket, int bChangeController);
	static CMobPool__SetLocalMob_t CMobPool__SetLocalMob = reinterpret_cast<CMobPool__SetLocalMob_t>(CMobPool__SetLocalMob_address);
	
	static unsigned char* WinMain_force_splash_screen = reinterpret_cast<unsigned char*>(WinMain_force_splash_screen_address);

	typedef int (__cdecl* GetLevel_t)();
	static GetLevel_t GetLevel = reinterpret_cast<GetLevel_t>(GetLevel_address);
	
	typedef void (__fastcall* ZXString__Assign_t)(void* ecx, void* edx, const char* s, int n);
	static ZXString__Assign_t ZXString__Assign = reinterpret_cast<ZXString__Assign_t>(ZXString__Assign_address);
	
	typedef void (__fastcall* CDropPool__TryPickUpDrop_t)(void* ecx, void* edx, POINT* pt);
	static CDropPool__TryPickUpDrop_t CDropPool__TryPickUpDrop = reinterpret_cast<CDropPool__TryPickUpDrop_t>(CDropPool__TryPickUpDrop_address);

	typedef void (__fastcall* CWvsContext__UI_Toggle_t)(void* ecx, void* edx, int nUIType, int nDefaultTab);
	static CWvsContext__UI_Toggle_t CWvsContext__UI_Toggle = reinterpret_cast<CWvsContext__UI_Toggle_t>(CWvsContext__UI_Toggle_address);
	
	typedef void (__fastcall* CWvsContext__SendSpawnFamiliarRequest_t)(void* ecx, void* edx, unsigned int dwTypeID);
	static CWvsContext__SendSpawnFamiliarRequest_t CWvsContext__SendSpawnFamiliarRequest = reinterpret_cast<CWvsContext__SendSpawnFamiliarRequest_t>(CWvsContext__SendSpawnFamiliarRequest_address);

	typedef int (__fastcall* CSkillInfo__GetSkillLevel_t)(void* ecx, void* edx, void* c, int nSkillID, void** ppSkillEntry, int bNoPvPLevelCheck, int bPureStealSLV, int bNotApplySteal, int bCheckSkillRoot);
	static CSkillInfo__GetSkillLevel_t CSkillInfo__GetSkillLevel = reinterpret_cast<CSkillInfo__GetSkillLevel_t>(CSkillInfo__GetSkillLevel_address);
	
	typedef int (__fastcall* CUserLocal__DoActiveSkill_t)(void* ecx, void* edx, int nSkillID, unsigned int nScanCode, int *pnConsumeCheck, int bPreparedSkill, int bBySteal, int nSkillCastItemPos, bool bAddAttackProc, unsigned int dwOption, int nBySummonedID);
	static CUserLocal__DoActiveSkill_t CUserLocal__DoActiveSkill = reinterpret_cast<CUserLocal__DoActiveSkill_t>(CUserLocal__DoActiveSkill_address);
	
	typedef int (__fastcall* CUserLocal__TryDoingMeleeAttack_t)(void* ecx, void* edx, void* pSkill, int nSLV, int* pnShootRange0, int nSerialAttackSkillID, unsigned int dwLastAttackMobID, int tKeyDown, void* pGrenade, int nReservedSkillID, int bTimeBombAttack, int nTimeBombX, int nTimeBombY, int nShootSkillID, bool bAddAttackProc, int nBySummonedID, unsigned int dwTargetMobID, void* unknown1, void* unknown2, void* unknown3);
	static CUserLocal__TryDoingMeleeAttack_t CUserLocal__TryDoingMeleeAttack = reinterpret_cast<CUserLocal__TryDoingMeleeAttack_t>(CUserLocal__TryDoingMeleeAttack_address);
	
	typedef FUNCKEY_MAPPED* (__fastcall* CFuncKeyMappedMan__FuncKeyMapped_t)(void* ecx, void* edx, int nIdx);
	static CFuncKeyMappedMan__FuncKeyMapped_t CFuncKeyMappedMan__FuncKeyMapped = reinterpret_cast<CFuncKeyMappedMan__FuncKeyMapped_t>(CFuncKeyMappedMan__FuncKeyMapped_address);
}