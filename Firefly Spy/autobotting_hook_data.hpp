#pragma once

#include "native.hpp"
#include "addresses.hpp"

#include "life_objects.hpp"
#include "packet_objects.hpp"
#include "stat_objects.hpp"

#include "ZRef.hpp"
#include "ZXString.hpp"

namespace firefly
{
	unsigned int mount_skill_ids[] = 
	{
		33001001,	// Wild Hunter - Jaguar Riding
		35001002,	// Mechanic - Humanoid Mech
		35111003,	// Mechanic - Tank Mech
	};

	unsigned int mode_skill_ids[] = 
	{
		110001501,	// Beast Tamer - Bear Mode
		110001502,	// Beast Tamer - Leopard Mode
		110001503,	// Beast Tamer - Hawk Mode
		110001504,	// Beast Tamer - Cat Mode
	};

	unsigned int familiar_ids[] = 
	{
		9960098,	// Jr. Boogie
		9960295,	// Big Spider
		9961073,	// Leprechaun
		9960350,	// Eye of Time
		9960452,	// Mutant Snail
		9960430,	// Wolf Underling
		9960455,	// Mutant Ribbon Pig
	};
					
	typedef void (__fastcall* CPet__Update_t)(CPet* ecx, void* edx);
	static CPet__Update_t CPet__Update = reinterpret_cast<CPet__Update_t>(CPet__Update_address);
						
	typedef void (__fastcall* CUserLocal__Update_t)(void* ecx, void* edx);
	static CUserLocal__Update_t CUserLocal__Update = reinterpret_cast<CUserLocal__Update_t>(CUserLocal__Update_address);
	
	typedef unsigned int (__fastcall* GW_CharacterStat__DecodeChangeStat_t)(GW_CharacterStat* ecx, void* edx, CInPacket *iPacket, int bBattlePvP);
	static GW_CharacterStat__DecodeChangeStat_t GW_CharacterStat__DecodeChangeStat = reinterpret_cast<GW_CharacterStat__DecodeChangeStat_t>(GW_CharacterStat__DecodeChangeStat_address);
	
	typedef void (__fastcall* CUserLocal__HandleUpKeyDown_t)(void* ecx, void* edx);
	static CUserLocal__HandleUpKeyDown_t CUserLocal__HandleUpKeyDown = reinterpret_cast<CUserLocal__HandleUpKeyDown_t>(CUserLocal__HandleUpKeyDown_address);

	typedef void (__fastcall* CField__SendTransferChannelRequest_t)(void* ecx, void* edx, int nTargetChannel);
	static CField__SendTransferChannelRequest_t CField__SendTransferChannelRequest = reinterpret_cast<CField__SendTransferChannelRequest_t>(CField__SendTransferChannelRequest_address);

	typedef int (__fastcall* CWvsContext__SendMigrateToShopRequest_t)(void* ecx, void* edx, int nMigrateToShop, bool bBonusByCashPR);
	static CWvsContext__SendMigrateToShopRequest_t CWvsContext__SendMigrateToShopRequest = reinterpret_cast<CWvsContext__SendMigrateToShopRequest_t>(CWvsContext__SendMigrateToShopRequest_address);

	typedef void (__fastcall* CStage__OnPacket_t)(void* ecx, void* edx, int nType, CInPacket* iPacket);
	static CStage__OnPacket_t CStage__OnPacket = reinterpret_cast<CStage__OnPacket_t>(CStage__OnPacket_address);
}