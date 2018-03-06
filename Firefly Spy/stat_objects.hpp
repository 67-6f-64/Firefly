#pragma once

#include "native.hpp"
#include "addresses.hpp"

#include "generic_objects.hpp"
#include "TSingleton.hpp"
#include "ZtlSecure.hpp"

namespace firefly
{
#pragma pack(push, 1)
	class GW_CharacterStat
	{
		~GW_CharacterStat() = delete;

	public:
		int get_hp()
		{
			try
			{
				return this->nHP.Fuse();
			}
			catch (std::exception& e)
			{
				UNREFERENCED_PARAMETER(e);
			}

			return 0;
		}

		int get_mp()
		{
			try
			{
				return this->nMP.Fuse();
			}
			catch (std::exception& e)
			{
				UNREFERENCED_PARAMETER(e);
			}

			return 0;
		}
		
	private:
		unsigned int dwCharacterID;				// 0x00
		unsigned int dwCharacterIDForLog;		// 0x04
		unsigned int dwWorldIDForLog;			// 0x08
		char sCharacterName[13];				// 0x0C
		char nGender;							// 0x19
		char nSkin;								// 0x1A
		int nFace;								// 0x1B
		int nHair;								// 0x1F
		char nMixBaseHairColor;					// 0x23
		char nMixAddHairColor;					// 0x24
		char nMixHairBaseProb;					// 0x25
		ZtlSecurePack<short> nLevel;			// 0x26
		ZtlSecurePack<short> nJob;				// 0x2E
		ZtlSecurePack<short> nSTR;				// 0x36
		ZtlSecurePack<short> nDEX;				// 0x3E
		ZtlSecurePack<short> nINT;				// 0x46
		ZtlSecurePack<short> nLUK;				// 0x4E
		ZtlSecurePack<int> nHP;					// 0x56
		ZtlSecurePack<int> nMHP;				// 0x62
		ZtlSecurePack<int> nMP;					// 0x6E
		ZtlSecurePack<int> nMMP;				// 0x7A
	};
#pragma pack(pop)

	class CharacterData
	{
		~CharacterData() = delete;

	public:
		GW_CharacterStat characterStat;
	};

	struct BasicStat
	{
		~BasicStat() = delete;
		
	public:
		int get_max_hp()
		{
			try
			{
				return this->nMHP.Fuse();
			}
			catch (std::exception& e)
			{
				UNREFERENCED_PARAMETER(e);
			}

			return 0;
		}

		int get_max_mp()
		{
			try
			{
				return this->nMMP.Fuse();
			}
			catch (std::exception& e)
			{
				UNREFERENCED_PARAMETER(e);
			}

			return 0;
		}
		
	private:
		ZtlSecure<int> nGender;					// 0x00
		ZtlSecure<int> nLevel;					// 0x0C
		ZtlSecure<int> nJob;					// 0x18
		ZtlSecure<int> nSTR;					// 0x24
		ZtlSecure<int> nDEX;					// 0x30
		ZtlSecure<int> nINT;					// 0x3C
		ZtlSecure<int> nLUK;					// 0x48
		ZtlSecure<int> nPOP;					// 0x54
		ZtlSecure<int> nMHP;					// 0x60 
		ZtlSecure<int> nMHPOfItem;				// 0x6C
		ZtlSecure<int> nMMP;					// 0x78
		ZtlSecure<int> nPvPDamage;				// 0x84
		ZtlSecure<char> nCharisma;				//  
		ZtlSecure<char> nInsight;				// 
		ZtlSecure<char> nWill;					// 
		ZtlSecure<char> nCraft;					// 
		ZtlSecure<char> nSense;					// 
		ZtlSecure<char> nCharm;					// 
		ZtlSecure<int> bPvP;					// 
		ZtlSecure<int> nAttackCountIncByEquip;	// 
		ZMap<unsigned long,long,unsigned long> m_mSetItemMobDamR;
	};

	class CUIStat : public CWnd, public TSingleton<CUIStat, TSingleton_CUIStat>
	{
		~CUIStat() = delete;

	public:
		void click_button(unsigned char id)
		{
			unsigned int auto_ap_up_button_ids[] = 
			{
				2010,		// Regular
				2011,		// Pirate (Brawler)
				2012,		// Pirate (Gunslinger)
				2013,		// Xenon (STR)
				2014,		// Xenon (DEX)
				2015		// Xenon (LUK)
			};

			if (id >= 0 && id < _countof(auto_ap_up_button_ids))
				return this->OnButtonClicked(auto_ap_up_button_ids[id]);
		}
	};
	
	static_assert_size(sizeof(GW_CharacterStat), 0x86);
	static_assert_size(sizeof(BasicStat), 0xF0);
	static_assert_size(sizeof(CUIStat), sizeof(CWnd));
}