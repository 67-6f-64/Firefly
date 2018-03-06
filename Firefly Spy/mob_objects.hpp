#pragma once

#include "native.hpp"
#include "addresses.hpp"

#include "life_objects.hpp"
#include "space_objects.hpp"

#include "TSingleton.hpp"

#include <algorithm>
#include <functional>

namespace firefly
{
	class CVecCtrlMob : public CVecCtrl
	{
		virtual ~CVecCtrlMob() = 0;

	public:
		int m_nMoveAbility;				// CVecCtrl_size + 0x00
		int m_nHomeMass;				// CVecCtrl_size + 0x04
		IVecCtrlOwner* m_pTarget;		// CVecCtrl_size + 0x08
	};
	
	class CMob : public CLife
	{
		~CMob() = delete;

	public:
		typedef struct _ShowDamageFlag
		{
			int nAttackIdx;
			int bCriticalAttack;
			int bZigZagDamage;
			int bAdjustHeight;
			int bRandomXPos;
			bool bShowHit;
			bool bKaiserDragonSlash;
			int nSkillID;
			bool bAssist;
			int nSkinID;
			int nPartnerID;
		} ShowDamageFlag;

		typedef struct _TARGETINFO
		{
			union
			{
				unsigned int dwTargetInfo;
				
				struct ptTarget
				{
					short x;
					short y;
				};

				struct skillInfo
				{
					short nSkillIDandLev;
					short nDelay;
				};

				unsigned int dwData;
			};
		} TARGETINFO;
	};
	
	class CMobPool : public TSingleton<CMobPool, TSingleton_CMobPool>
	{
		virtual ~CMobPool() = 0;

	public:
		static int size()
		{
			if (!IsInitialized())
				return 0;

			return GetInstance()->m_lMob.GetCount();
		}

	public:		
		void* get_mob(unsigned int mob_id)
		{
			ZRef<CMob>* mob = nullptr;
			
			if (!this->m_mMob.GetAt(mob_id, reinterpret_cast<__POSITION**>(&mob)))
				return nullptr;

			return mob->p;
		}

		int get_mobs(CMob** mobs, int max_count)
		{
			int mob_count = 0;
			
			this->m_lMob.EnumerateList([&](ZRef<CMob>* mob) -> bool
			{
				POINT pt_temp;
				mob->p->GetPos(&pt_temp);

				if (CWvsPhysicalSpace2D::GetInstance()->IsInRect(pt_temp))
				{
					mobs[mob_count++] = mob->p;

					if (mob_count >= max_count)
						return true;
				}

				return false;
			});

			return mob_count;
		}

		bool get_random_mob(POINT* pt_mob)
		{
			if (pt_mob != nullptr)
			{
				POINT pt_min;
				memset(&pt_min, 0, sizeof(POINT));

				this->m_lMob.EnumerateList([&, this](ZRef<CMob>* mob) -> bool
				{
					POINT pt_temp;
					mob->p->GetPos(&pt_temp);

					if (CWvsPhysicalSpace2D::GetInstance()->IsInRect(pt_temp))
					{
						*pt_mob = pt_temp;
						return true;
					}

					return false;
				});
			}
			
			return (pt_mob && pt_mob->x != 0 && pt_mob->y != 0);
		}

		bool get_closest_mob(POINT* pt_mob, POINT& pt_user)
		{
			if (pt_mob != nullptr)
			{
				POINT pt_min;
				memset(&pt_min, 0, sizeof(POINT));

				this->m_lMob.EnumerateList([&, this](ZRef<CMob>* mob) -> bool
				{
					POINT pt_temp;
					mob->p->GetPos(&pt_temp);

					POINT pt_temp_min = std::min<POINT>(pt_min, pt_temp, [&](const POINT& pt1, const POINT& pt2)
					{
						return (get_mob_distance(pt1, pt_user) < get_mob_distance(pt2, pt_user));
					});
						
					if (CWvsPhysicalSpace2D::GetInstance()->IsInRect(pt_temp_min))
					{
						pt_min.x = pt_temp_min.x;
						pt_min.y = pt_temp_min.y;
					}

					return false;
				});
				
				if (pt_min.x != 0 || pt_min.y != 0)
				{
					pt_mob->x = pt_min.x;
					pt_mob->y = pt_min.y;
					return true;
				}
			}

			return false;
		}

	private:
		double get_mob_distance(POINT pt_mob, POINT pt_user)
		{
			std::function<int(int)> square = [](int x) 
			{ 
				return x * x; 
			};
		
			int squared = square(pt_mob.x - pt_user.x) + square(pt_mob.y - pt_user.y);
			return std::sqrt(static_cast<long double>(squared));
		}

	private:
		ZMap<unsigned long, __POSITION*, unsigned long> m_mMob;				// 0x04
		ZList<ZRef<CMob>> m_lMob;											// 0x1C
		ZList<ZRef<CMob>> m_lMobDelayedDead;								// 0x30
		ZList<ZRef<CMob>> m_lMobDeadProcess;								// 0x44	
		ZRef<CMob> m_pMobDamagedByMob;										// 0x58
		int m_tLastHitMobDamagedByMob;										// 0x60
		ZRef<CMob> m_pGuidedMob;											// 0x64
		unsigned int m_dwMobCrcKey;											// 0x6C
		ZMap<unsigned long, long, unsigned long> m_mMobCountWithTemplateID;	// 0x70
		int m_nUserControlMobCount;											// 0x88
		int m_nBossMobCount;												// 0x8C
	};
	
	static_assert_size(sizeof(CMobPool), 0x90);
	static_assert_size(sizeof(CVecCtrlMob), CVecCtrl_size + 0x10);
}