#pragma once

#include "native.hpp"
#include "addresses.hpp"

#include "TSingleton.hpp"
#include "ZList.hpp"
#include "ZMap.hpp"
#include "ZRefCounted.hpp"

#include "generic_objects.hpp"
#include "space_objects.hpp"

#include <algorithm>
#include <functional>

namespace firefly
{
	struct DROP : public ZRefCounted
	{
	public:
		SECPOINT pt1;				// 0x10
		void* posList;				// 0x28
		void* posListForPet;		// 0x2C
		char nEnterType;			// 0x30
		char bReal;					// 0x31
		unsigned int dwId;			// 0x34
		unsigned int dwOwnerID;		// 0x38
		unsigned int dwSourceID;	// 0x3C
		int nOwnType;				// 0x40
		int bIsMoney;				// 0x44
		int nInfo;					// 0x48
		void* pLayer;				// 0x4C
		void* pEnterLayer;			// 0x50
		void* pStayLayer;			// 0x54
		void* pLeaveLayer;			// 0x58
		void* pvec;					// 0x5C
		int tCreateTime;			// 0x60
		int tLeaveTime;				// 0x64
		int nState;					// 0x68
		int tTickTime;				// 0x6C
		int tEndParabolicMotion;	// 0x70
		int tLastTryPickUp;			// 0x74
		unsigned int dwPickupID;	// 0x78
		long double fAngle;			// 0x7C
		SECPOINT pt2;				// 0x88
	};

	class CDropPool : public TSingleton<CDropPool, TSingleton_CDropPool>
	{
		virtual ~CDropPool() = 0;

	public:
		static int size()
		{
			if (!IsInitialized())
				return 0;

			return GetInstance()->m_lDrop.GetCount();
		}

	public:		
		void try_pick_up_drop(POINT* pt_user)
		{
			return CDropPool__TryPickUpDrop(this, nullptr, pt_user);
		}

		bool get_drop(POINT* pt_drop, unsigned int number)
		{
			if (pt_drop != nullptr && this->m_lDrop.GetCount() > number)
			{
				this->m_lDrop.EnumerateList([&](ZRef<DROP>* drop) -> bool
				{
					if (drop->p->nInfo != 0)
					{
						pt_drop->x = drop->p->pt2.x.GetData();
						pt_drop->y = drop->p->pt2.y.GetData();

						if (CWvsPhysicalSpace2D::GetInstance()->IsInRect(*pt_drop))
						{
							if (number-- == 0)
								return true;
						}
					}

					return false;
				});
			}

			return (number == -1);
		}

		bool get_random_drop(POINT* pt_drop)
		{
			if (pt_drop != nullptr)
			{
				this->m_lDrop.EnumerateList([&](ZRef<DROP>* drop) -> bool
				{
					if (drop->p->nInfo != 0)
					{
						pt_drop->x = drop->p->pt2.x.GetData();
						pt_drop->y = drop->p->pt2.y.GetData();

						if (CWvsPhysicalSpace2D::GetInstance()->IsInRect(*pt_drop))
							return true;
					}

					return false;
				});
			}

			return (pt_drop && pt_drop->x != 0 && pt_drop->y != 0);
		}
		
		bool get_closest_drop(POINT* pt_drop, POINT pt_user)
		{
			if (pt_drop != nullptr)
			{
				POINT pt_min;
				memset(&pt_min, 0, sizeof(POINT));

				this->m_lDrop.EnumerateList([&, this](ZRef<DROP>* drop) -> bool
				{
					if (drop->p->nInfo != 0)
					{
						POINT pt_temp;
						pt_temp.x = drop->p->pt2.x.GetData();
						pt_temp.y = drop->p->pt2.y.GetData();
						
						POINT pt_temp_min = std::min<POINT>(pt_min, pt_temp, [&](const POINT& pt1, const POINT& pt2)
						{
							return (get_drop_distance(pt1, pt_user) < get_drop_distance(pt2, pt_user));
						});
						
						if (CWvsPhysicalSpace2D::GetInstance()->IsInRect(pt_temp_min))
						{
							pt_min.x = pt_temp_min.x;
							pt_min.y = pt_temp_min.y;
						}
					}

					return false;
				});
				
				if (pt_min.x != 0 || pt_min.y != 0)
				{
					pt_drop->x = pt_min.x;
					pt_drop->y = pt_min.y;
					return true;
				}
			}

			return false;
		}

	private:
		double get_drop_distance(POINT pt_drop, POINT pt_user)
		{
			std::function<int(int)> square = [](int x) 
			{ 
				return x * x; 
			};
		
			int squared = square(pt_drop.x - pt_user.x) + square(pt_drop.y - pt_user.y);
			return std::sqrt(static_cast<long double>(squared));
		}

	private:
		int m_tLastExplodeSound;											// 0x04
		ZMap<unsigned long, ZRef<DROP>, unsigned long> m_mDrop;				// 0x08
		ZList<ZRef<DROP>> m_lDrop;											// 0x20
		ZList<ZRef<DROP>> m_lDropPickUpByPet;								// 0x34
		ZMap<unsigned long, POINT, unsigned long> m_mDropPt;				// 0x48
		void* m_pPropMoneyIcon;												// 0x60
		ZList<POINT> m_lpRecentPickupedItem;								// 0x64
		int m_bRecentPickupedItemCheck;										// 0x78
	};

	static_assert_size(sizeof(DROP), 0xA0);
	static_assert_size(sizeof(CDropPool), 0x7C);
}