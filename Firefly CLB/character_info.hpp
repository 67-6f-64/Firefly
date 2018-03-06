#pragma once

#include "native.hpp"
#include "net_buffer.hpp"

#include "job.hpp"

namespace firefly
{
	/* GW_CharacterStat */
	typedef struct _SPSet
	{
		unsigned char nJobLevel;
		unsigned int nSP;
	} SPSet;

	typedef struct _NonCombatStatDayLimit
	{
		__int16 nCharisma;
		__int16 nInsight;
		__int16 nWill;
		__int16 nCraft;
		__int16 nSense;
		__int16 nCharm;
		char nCharmByCashPR;
		FILETIME ftLastUpdateCharmByCashPR;
	} NonCombatStatDayLimit;

	typedef struct _CHARACTERCARD
	{
		typedef struct _CARD
		{
			unsigned int dwCharacterID;
			unsigned char nLevel;
			unsigned int nJob;
		} CARD;

		void decode(net_buffer& packet)
		{
			for (int i = 0; i < (sizeof(this->card) / sizeof(CARD)); i++)
			{
				this->card[i].dwCharacterID = packet.read4();
				this->card[i].nLevel = packet.read1();
				this->card[i].nJob = packet.read4();
			}
		}

		CARD card[9];
	} CHARACTERCARD;

	typedef struct _character_info
	{
		void decode(net_buffer& packet)
		{
			this->dwCharacterID = packet.read4();
			this->dwCharacterIDForLog = packet.read4();
			this->dwWorldIDForLog = packet.read4();

			packet.read_buffer(reinterpret_cast<unsigned char*>(this->sCharacterName), 13);
			
			this->nGender = packet.read1();
			this->nSkin = packet.read1();
			this->nFace = packet.read4();
			this->nHair = packet.read4();
		
			this->nMixBaseHairColor = packet.read1();
			this->nMixAddHairColor = packet.read1();
			this->nMixHairBaseProb = packet.read1();

			this->nLevel = packet.read1();
			this->nJob = packet.read2();

			this->nSTR = packet.read2();
			this->nDEX = packet.read2();
			this->nINT = packet.read2();
			this->nLUK = packet.read2();

			this->nHP = packet.read4();
			this->nMHP = packet.read4();

			this->nMP = packet.read4();
			this->nMMP = packet.read4();
			
			this->nAP = packet.read2();
		
			this->extendSP.clear();

			if (this->is_extendsp_job(this->nJob))
			{
				unsigned char nSP = 0;
				unsigned char nExtendSPCount = packet.read1();

				for (unsigned char i = 0; i < nExtendSPCount; i++)
				{
					SPSet spset;
					spset.nJobLevel = packet.read1();
					spset.nSP = packet.read4();

					this->extendSP.push_back(spset);
				}
			}
			else
			{
				this->nSP = packet.read2();
			}

			this->nEXP64 = packet.read8();
			this->nPOP = packet.read4();
			this->nTempEXP = packet.read4();
			this->nWP = packet.read4();

			this->dwPosMap = packet.read4();
			this->nPortal = packet.read1();
			
			this->nPlayTime = packet.read4();
			this->nSubJob = packet.read2();
			
			if (job::is_demon_slayer_job(this->nJob) || job::is_xenon_job(this->nJob) || job::is_beast_tamer_job(this->nJob))
				this->nDefFaceAcc = packet.read4();
			else
				this->nDefFaceAcc = 0;
			
			this->nFatigue = packet.read1();
			this->nLastFatigueUpdateTime = packet.read4();
		
			this->nCharismaEXP = packet.read4();
			this->nInsightEXP = packet.read4();
			this->nWillEXP = packet.read4();
			this->nCraftEXP = packet.read4();
			this->nSenseEXP = packet.read4();
			this->nCharmEXP = packet.read4();

			packet.read_buffer(reinterpret_cast<unsigned char*>(&this->DayLimit), 21);
			
			this->nPvPExp = packet.read4();
			this->nPvPGrade = packet.read1();
			this->nPvPPoint = packet.read4();
			this->nPvPModeLevel = packet.read1();
			this->nPvPModeType = packet.read1();

			this->nEventPoint = packet.read4();
		
			this->nAlbaActivityID = packet.read1();
			this->ftAlbaStartTime.dwHighDateTime = packet.read4();
			this->ftAlbaStartTime.dwLowDateTime = packet.read4();

			this->nAlbaDuration = packet.read4();
			this->bAlbaSpecialReward = packet.read1();

			this->characterCard.decode(packet);
		
			FILETIME ftLastLogout;
			ftLastLogout.dwHighDateTime = packet.read4();
			ftLastLogout.dwLowDateTime = packet.read4();
			FileTimeToSystemTime(&ftLastLogout, &this->stAccount_LastLogout);

			this->bBurning = packet.read1();
			packet.read_buffer(nullptr, 40); // Random shit, skip pls.
		}

	private:
		bool is_extendsp_job(int nJob)
		{
			return (!nJob || 
				job::is_adventurer_warrior(nJob) || 
				job::is_adventurer_mage(nJob) || 
				job::is_adventurer_archer(nJob) || 
				job::is_adventurer_rogue(nJob) || 
				job::is_adventurer_pirate(nJob) || 
				job::is_jett_job(nJob) ||
				job::is_unknown_job_2(nJob) ||
				job::is_cygnus_job(nJob)	|| 
				job::is_resistance_job(nJob) || 
				job::is_evan_job(nJob) || 
				job::is_mercedes_job(nJob) || 
				job::is_phantom_job(nJob) || 
				job::is_leader_job(nJob) || 
				job::is_luminous_job(nJob) || 
				job::is_dragonborn_job(nJob) || 
				job::is_zero_job(nJob) || 
				job::is_hidden_job(nJob) || 
				job::is_aran_job(nJob) ||
				job::is_kinesis_job(nJob));
		}
		
	public:
		unsigned int dwCharacterID;
		unsigned int dwCharacterIDForLog;
		unsigned int dwWorldIDForLog;
		char sCharacterName[13];

		char nGender;
		char nSkin;
		int nFace;
		int nHair;

		char nMixBaseHairColor;
		char nMixAddHairColor;
		char nMixHairBaseProb;

		unsigned char nLevel;
		unsigned short nJob;

		unsigned short nSTR;
		unsigned short nDEX;
		unsigned short nINT;
		unsigned short nLUK;

		unsigned int nHP;
		unsigned int nMHP;

		unsigned int nMP;
		unsigned int nMMP;

		unsigned short nAP;

		unsigned int nSP;
		std::vector<SPSet> extendSP;

		unsigned __int64 nEXP64;
		unsigned int nPOP;
		unsigned int nTempEXP;
		unsigned int nWP;

		unsigned int dwPosMap;
		unsigned char nPortal;
		
		unsigned int nPlayTime;
		unsigned short nSubJob;

		unsigned int nDefFaceAcc;
		unsigned char nFatigue;
		unsigned int nLastFatigueUpdateTime;
		
		unsigned int nCharismaEXP;
		unsigned int nInsightEXP;
		unsigned int nWillEXP;
		unsigned int nCraftEXP;
		unsigned int nSenseEXP;
		unsigned int nCharmEXP;

		NonCombatStatDayLimit DayLimit;

		unsigned int nPvPExp;
		unsigned char nPvPGrade;
		unsigned int nPvPPoint;
		unsigned char nPvPModeLevel;
		unsigned char nPvPModeType;

		unsigned int nEventPoint;
		
		unsigned char nAlbaActivityID;
		FILETIME ftAlbaStartTime;

		unsigned int nAlbaDuration;
		unsigned char bAlbaSpecialReward;

		CHARACTERCARD characterCard;
		SYSTEMTIME stAccount_LastLogout;

		unsigned char bBurning;
	} character_info;

	/* AvatarLook */
	typedef struct _character_look
	{
		unsigned int decode(net_buffer& packet)
		{
			this->nGender = packet.read1();
			this->nSkin = packet.read1();
			this->nFace = packet.read4();
			this->nJob = packet.read4();

			memset(&this->anHairEquip, 0, sizeof(this->anHairEquip));
			memset(&this->anUnseenEquip, 0, sizeof(this->anUnseenEquip));

			for (unsigned char pos = packet.read1(); pos != 0xFF; pos = packet.read1())
				this->anHairEquip[pos] = packet.read4();

			for (unsigned char pos = packet.read1(); pos != 0xFF; pos = packet.read1())
				this->anUnseenEquip[pos] = packet.read4();

			for (unsigned char pos = packet.read1(); pos != 0xFF; pos = packet.read1())
				continue; // <unknown>

			this->nWeaponStickerID = packet.read4();
			this->nWeaponID = packet.read4();
			this->nSubWeaponID = packet.read4();

			this->bDrawElfEar = packet.read1();
			packet.read_buffer(reinterpret_cast<unsigned char*>(this->anPetID), sizeof(this->anPetID));

			if (job::is_demon_slayer_job(this->nJob))
				this->nDemonSlayerDefFaceAcc = packet.read4();
			else if (job::is_xenon_job(this->nJob))
				this->nXenonDefFaceAcc = packet.read4();
			else if (job::is_zero_job(this->nJob))
				this->bIsZeroBetaLook = packet.read1();
			else if (job::is_beast_tamer_job(this->nJob))
			{
				packet.read4();	// <unknown>
				packet.read1();	// <unknown>
				packet.read4(); // <unknown>
				packet.read1(); // <unknown>
				packet.read4(); // <unknown>
			}

			this->nMixedHairColor = packet.read1();
			this->nMixHairPercent = packet.read1();
			return this->nJob;
		}

	public:
		unsigned char nGender;
		unsigned int nSkin;
		unsigned int nFace;
		unsigned int nJob;
		
		unsigned int anHairEquip[32];
		unsigned int anUnseenEquip[32];

		unsigned int nWeaponStickerID;
		unsigned int nWeaponID;
		unsigned int nSubWeaponID;
		
		unsigned char bDrawElfEar;
		unsigned int anPetID[3];

		unsigned int nDemonSlayerDefFaceAcc;
		unsigned int nXenonDefFaceAcc;
		unsigned char bIsZeroBetaLook;

		unsigned char nMixedHairColor;
		unsigned char nMixHairPercent;
	} character_look;
}