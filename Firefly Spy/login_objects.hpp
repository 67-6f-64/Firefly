#pragma once

#include "native.hpp"
#include "addresses.hpp"

#include "TSingleton.hpp"
#include "ZRef.hpp"
#include "ZXString.hpp"

#include "generic_objects.hpp"

namespace firefly
{
	class ZThread
	{
		virtual ~ZThread() = 0;
		virtual int Run(void*, void*);

	public:
		unsigned int _m_dwThreadId;
		void *_m_hThread;
	};

	class CLoadClientDataThread : public ZThread, public TSingleton<CLoadClientDataThread, TSingleton_CLoadClientDataThread>
	{
		virtual ~CLoadClientDataThread() = 0;

	public:
		bool IsLoadedAllLoginData()
		{
			return (this->m_nLoadClientState == 1);
		}

		bool IsLoadedAllGameData()
		{
			return (this->m_nLoadClientState == 3);
		}

	private:
		int m_bLoadQuest;
		volatile int m_bTerminate;
		void* m_hEventTerminate;
		int m_nLoadClientState;
		int m_nLoadFlag;
		int m_bIsNowLoading;
	};

	class CLogin
	{
	public:
		void select_character(int character_id, std::string const& pic)
		{
			this->m_nCharSelected = character_id;
			this->m_sSPW.SetText(pic);
		}

	private:
		padding(CLogin_nCharSelected_offset);
		int m_nCharSelected;			
		padding_sub(CLogin_sSPW_offset, CLogin_nCharSelected_offset + 0x04);
		ZXString<char> m_sSPW;			
	};

	class CUISelectChar
	{
	public:
		void select_character(int character_id, std::string const& pic)
		{
			return this->m_pLogin->select_character(character_id, pic);
		}

	private:
		padding(CUISelectChar_pLogin_offset);
		CLogin* m_pLogin;
	};
	
	class CGateWayWnd : public CWnd
	{
		~CGateWayWnd() = delete;

	public:
		void click_button(unsigned char region)
		{
			unsigned int region_button_ids[] = 
			{
				2000,		// North America
				2001,		// Europe
			};

			if (region >= 0 && region < _countof(region_button_ids))
				return this->OnButtonClicked(region_button_ids[region]);
		}
	};

	static_assert_size(sizeof(ZThread), 0x0C);
	static_assert_size(sizeof(CLoadClientDataThread), 0x24);
	static_assert_size(sizeof(CLogin), CLogin_sSPW_offset + 0x04);
	static_assert_size(sizeof(CUISelectChar), CUISelectChar_pLogin_offset + 0x04);
	static_assert_size(sizeof(CGateWayWnd), sizeof(CWnd));
}