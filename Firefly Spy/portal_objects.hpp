#pragma once

#include "native.hpp"
#include "addresses.hpp"

#include "ZArray.hpp"
#include "ZRef.hpp"
#include "ZXString.hpp"

namespace firefly
{
	struct PORTAL
	{
		int nIdx;										// + 0x00
		ZXString<char> sName;							// + 0x04
		int nType;										// + 0x08
		POINT ptPos;									// + 0x0C
		int nHRange;									// + 0x14
		int nVRange;									// + 0x18
		int nTMap;										// + 0x1C
		unsigned int dwFreeMarketPortalFieldID;			// + 0x20
		ZXString<char> sTName;							// + 0x24
		int bHideTooltip;								// + 0x28
		int bOnlyOnce;									// + 0x2C
		int bShownAtMinimap;							// + 0x30
		bool bOverlappedTransfer;						// + 0x34
		bool bEnchantPortalInMinimap;					// + 0x36
		int nDelayTime;									// + 0x38
		ZXString<char> sImage;							// + 0x3C
		int nVImpact;									// + 0x40
		int nHImpact;									// + 0x44
		ZXString<char> sReactorName;					// + 0x48
		ZXString<char> sSessionValueKey;				// + 0x4C
		ZXString<char> sSessionValue;					// + 0x50
		ZXString<char> sGroupName;						// + 0x54
		int nSideType;									// + 0x58
		int nPortalGroupIdx;							// + 0x5C
	};
	
	class CPortalList : public TSingleton<CPortalList, TSingleton_CPortalList>
	{
		virtual ~CPortalList() = 0;

	public:
		ZArray<ZRef<PORTAL>> m_aPortal;					// + 0x04
		ZArray<ZRef<PORTAL>> m_aPortal_Collision;		// + 0x08
		ZArray<ZRef<PORTAL>> m_aPortal_Hidden;			// + 0x0C
		ZArray<ZRef<PORTAL>> m_aPortal_StartPoint;		// + 0x10
	};

	static_assert_size(sizeof(PORTAL), 0x60);
	static_assert_size(sizeof(CPortalList), 0x14);
}