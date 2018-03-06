#pragma once

#include "native.hpp"
#include "utility.hpp"
#include "addresses.hpp"

#include "generic_objects.hpp"

#include "TSingleton.hpp"
#include "ZList.hpp"
#include "ZMap.hpp"
#include "ZRef.hpp"
#include "ZRefCounted.hpp"
#include "ZXString.hpp"

#include <vector>

namespace firefly
{
	struct AbsPosEx
	{
		long double x;
		long double y;
		long double vx;
		long double vy;
	};

	struct RelPosEx
	{
		long double pos;
		long double v;
	};
	 
	class IVecCtrlOwner;

	class CVecCtrl : public ZRefCounted, public IGObj, public IWzVector2D
	{
		virtual ~CVecCtrl() = 0;

	public:
		virtual int GetType() = 0;
		virtual int CollisionDetectWalk(RelPosEx* p1, int* tElapse, int bOffsetSkip) = 0;
		virtual int CollisionDetectFloat(AbsPosEx* p1, int* tElapse, int bCalcFloat) = 0;
		virtual int GetMoveAbility() = 0;

		void SetImpactNext(int nAttr, short vx, short vy)
		{
			void** vtable = *reinterpret_cast<void***>(this);

			typedef void (__fastcall* SetImpactNext_t)(void* ecx, void* edx, int nAttr, short vx, short vy);
			reinterpret_cast<SetImpactNext_t>(vtable[CVecCtrl__SetImpactNext_offset / sizeof(void*)])(this, nullptr, nAttr, vx, vy);
		}

	public:
		padding_sub(CVecCtrl_pOwner_offset, sizeof(ZRefCounted) + sizeof(IGObj) + sizeof(IWzVector2D));
		IVecCtrlOwner* m_pOwner;											// 0xA0
		int m_bActive;
		void* m_pVecAlternate;
		AbsPosEx m_ap;														// 0xB0
		AbsPosEx m_apl;														// 0xD0
		padding_sub(CVecCtrl_size, CVecCtrl_pOwner_offset + sizeof(IVecCtrlOwner*) + 0xC + (sizeof(AbsPosEx) * 2));
	};
	
	struct IMPACTNEXT
	{
		int bValid;
		int nReason;
		long double vx;
		long double vy;
	};

	class IVecCtrlOwner
	{
	public:
		virtual int GetType() = 0;											// 0x00
		virtual int OnResolveMoveAction(int, int, int, CVecCtrl*) = 0;		// 0x04
		virtual void OnLayerZChanged(CVecCtrl*) = 0;						// 0x08
		virtual void OnFootholdChanged(void* const, void* const);			// 0x0C
		virtual void* GetShoeAttr() = 0;									// 0x10
		virtual POINT* GetPos(POINT* pt) = 0;								// 0x14
		virtual POINT* GetPosPrev(POINT* pt) = 0;							// 0x18
		virtual int GetZMass() = 0;											// 0x1C
		virtual CVecCtrl* GetVecCtrl() = 0;						 			// 0x20
		virtual CVecCtrl* GetVecCtrl_2() = 0;					 			// 0x24
		virtual void OnCompleteCalcMovePathAttrInfo(void*) = 0;				// 0x28
		virtual void* GetRTTI() = 0;										// 0x2C
		virtual int IsKindOf(void*) = 0;									// 0x30
	};

	class CLife : public IGObj, public IVecCtrlOwner
	{
		~CLife() = delete;
		
	private:
		padding_sub(CLife_size, sizeof(IGObj) + sizeof(IVecCtrlOwner));
	};
	
	class CAvatar
	{
		virtual ~CAvatar() = delete;

	protected:
		padding_sub(CAvatar_m_nRidingVehicleID_offset, sizeof(void*));				
		int m_nRidingVehicleID;	
		padding_sub(CAvatar_m_tAlertRemain_offset, CAvatar_m_nRidingVehicleID_offset + 4);
		int m_tAlertRemain;
	};

	class CUser : public CLife, public CAvatar
	{
		~CUser() = delete;

	public:
		padding_sub(CUser_dwCharacterID_offset, sizeof(CLife) + sizeof(CAvatar));
		unsigned int m_dwCharacterID;
		ZXString<char> m_sCharacterName;
	};
	
	class CPet : public CLife
	{
		~CPet() = delete;
	
	public:
		__POSITION* m_posPetRepeatEffect[1];		// 0x108
		void* m_pTemplate;							// 0x10C
		ZArray<long> m_aExceptionList;				// 0x110
		void* m_pvc;								// 0x114
		CUser* m_pOwner;							// 0x118
		ZXString<char> m_sName;						// 0x11C
		int m_nPetIndex;							// 0x120
		padding(0x0C);								// 0x124
		int m_nTameness;							// 0x130
		int m_nRepleteness;							// 0x134
	};
	
	class CUserLocal : public CUser, public TSingleton<CUserLocal, TSingleton_CUserLocal>
	{
		~CUserLocal() = delete;

	public:
		bool is_riding_vechicle()
		{
			return (this->m_nRidingVehicleID != 0);
		}

		bool is_familiar_inactive()
		{
			return (this->familiars[0].p == nullptr);
		}

		int get_breath()
		{
			return this->m_tAlertRemain;
		}

		bool do_active_skill(unsigned int skill_id)
		{
			return (CUserLocal__DoActiveSkill(this, nullptr, skill_id, 0, NULL, FALSE, FALSE, 0, false, 0, 0) != FALSE);
		}

		bool try_doing_melee_attack(void* skill, int skill_level)
		{
			return (CUserLocal__TryDoingMeleeAttack(this, nullptr, skill, skill_level, NULL, 0, 0, 0, NULL, 0, FALSE, 0, 0, 0, false, 0, 0, NULL, NULL, NULL) != FALSE);
		}

	private:
		padding_sub(TSingleton_CUserLocal_apFamiliar_offset, sizeof(CUser));
		ZArray<ZRef<void*>> familiars;
	};

	class CUserRemote : public CUser
	{
		~CUserRemote() = delete;

	public:
		const char* get_name()
		{
			return this->m_sCharacterName;
		}
	};

	struct USERREMOTE_ENTRY : public ZRefCounted
	{
		ZRef<CUserRemote> pUserRemote;
		__POSITION* posList;
		__POSITION* posDelayedLoad;
	};

	class CUserPool : public IGObj, public TSingleton<CUserPool, TSingleton_CUserPool>
	{
		~CUserPool() = delete;

	public:
		static int size()
		{
			if (!IsInitialized())
				return 0;

			return GetInstance()->m_lUserRemote.GetCount();
		}

		int filtered_size(bool enable_whitelist, std::vector<std::string> const& whitelist)
		{
			int size = this->m_lUserRemote.GetCount();

			if (enable_whitelist)
			{
				this->m_lUserRemote.EnumerateList([&](ZRef<USERREMOTE_ENTRY>* entry) -> bool
				{
					if (entry->p)
					{
						CUserRemote* user = entry->p->pUserRemote.p;

						if (user)
						{
							if (std::find(whitelist.begin(), whitelist.end(), utility::lower(std::string(user->m_sCharacterName))) != whitelist.end())
								size--;
						}
					}

					return false;
				});
			}

			return size;
		}

	private:
		ZRef<CUserLocal> m_pUserLocal;
		ZMap<unsigned long, ZRef<USERREMOTE_ENTRY>, unsigned long> m_mUserRemote;
		ZList<ZRef<USERREMOTE_ENTRY>> m_lUserRemote;
	};
	
	static_assert_size(sizeof(AbsPosEx), 0x20);
	static_assert_size(sizeof(CVecCtrl), CVecCtrl_size);
	static_assert_size(sizeof(IVecCtrlOwner), 0x04);
	static_assert_size(sizeof(CLife), CLife_size);
	static_assert_size(sizeof(CPet), CPet_m_nRepleteness_offset + 0x04);
	static_assert_size(sizeof(CAvatar), CAvatar_m_tAlertRemain_offset + 4);
	static_assert_size(sizeof(CUser), CUser_dwCharacterID_offset + 0x04 + 0x04);
	static_assert_size(sizeof(CUserLocal), TSingleton_CUserLocal_apFamiliar_offset + 4);
	static_assert_size(sizeof(CUserPool), 0x38);
}