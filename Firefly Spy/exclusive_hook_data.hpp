#pragma once

#include "native.hpp"
#include "addresses.hpp"

#include "mob_objects.hpp"

namespace firefly
{
	typedef void (__fastcall* CDropPool__TryPickUpDropByPet_t)(void* ecx, void* edx, CPet* pPet, POINT* ptPos, SECPOINT* ptPosCheck);
	static CDropPool__TryPickUpDropByPet_t CDropPool__TryPickUpDropByPet = reinterpret_cast<CDropPool__TryPickUpDropByPet_t>(CDropPool__TryPickUpDropByPet_address);

	typedef void (__fastcall* CUserLocal__TryDoingFinalAttack_t)(void* ecx, void* edx);
	static CUserLocal__TryDoingFinalAttack_t CUserLocal__TryDoingFinalAttack = reinterpret_cast<CUserLocal__TryDoingFinalAttack_t>(CUserLocal__TryDoingFinalAttack_address);

	typedef int (__fastcall* CMobPool_FindHitMobInRect_t)(void* ecx, void* edx, RECT* rc, CMob** apMob, int nMaxCount, CMob* pExcept, int nWishMobID, int rPoison, unsigned int dwWishTemplateID, int bIncludeDazzledMob, int bIncludeEscortMob, int nSelectRangeSkillID, int nSelectRangeX, int nCurUserX, int nSkillID, int bFindBoss, void* unknown);
	static CMobPool_FindHitMobInRect_t CMobPool_FindHitMobInRect = reinterpret_cast<CMobPool_FindHitMobInRect_t>(CMobPool_FindHitMobInRect_address);
	
	typedef void (__fastcall* CMob__GenerateMovePath_t)(void* ecx, void* edx, int nAction, int bLeft, CMob::TARGETINFO ti, int bChase, int nMoveType, int nMoveEndingX, int nMoveEndingY, int bMoveLeft, int bRiseByToss, int nTossHeightWeight, unsigned int eSkill, CUserLocal* pUserLocal, int _nRushDistance, bool bAirHit);
	static CMob__GenerateMovePath_t CMob__GenerateMovePath = reinterpret_cast<CMob__GenerateMovePath_t>(CMob__GenerateMovePath_address);
}