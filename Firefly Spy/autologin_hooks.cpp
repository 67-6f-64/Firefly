#include "autologin_hooks.hpp"
#include "autologin_hook_data.hpp"

#include "autobotting_data.hpp"
#include "autologin_data.hpp"

#include "context_objects.hpp"
#include "detours.hpp"

#include "windows_hooks.hpp"

#ifdef _PRODUCTION
#include <VMProtectSDK.h>
#endif

namespace firefly
{	
	void try_select_region(void* ecx)
	{
		if (autologin_data::get_instance().enabled() && ecx != NULL)
			reinterpret_cast<CGateWayWnd*>(ecx)->click_button(static_cast<unsigned char>(autologin_data::get_instance().region()));
	}

	void try_select_world_channel()
	{
		if (autologin_data::get_instance().enabled() && CLogin_ecx != nullptr)
		{
			if (CLoadClientDataThread::IsInitialized() && CLoadClientDataThread::GetInstance()->IsLoadedAllGameData())
			{
				int channel = autologin_data::get_instance().channel() - 1;

				if (channel == -1)
					channel = autobotting_data::get_instance().get_appropriate_channel(CWvsContext::GetInstance()->get_channel(), false);

				CLogin__SendLoginPacket(CLogin_ecx, nullptr, autologin_data::get_instance().world(), channel, FALSE);

				if (autobotting_data::get_instance().enabled())
				{
					for (int i = static_cast<int>(botting_type::auto_timed_skill_4); i >= static_cast<int>(botting_type::auto_loot); i--)
						autobotting_data::get_instance().get_at(static_cast<botting_type>(i)).set_last_tick(0);
				}
			}
		}
	}

	/* Remove login dialogs */
	bool Hook_CLoginUtilDlg__Error(bool enable)
	{
		CLoginUtilDlg__Error_t CLoginUtilDlg__Error_hook = [](void* ecx, void* edx, int nMsg, ZRef<void*>* ppDialog)
		{
			if (autologin_data::get_instance().enabled())
			{
				if (!ppDialog)
				{
					switch (nMsg)
					{
					case 0x17: // Disconnection error
						return;

					case 0x11: // Already logged in error
					case 0x13: // Server over-populated error
						break;

					default:
						//printf("Unknown CLoginUtilDlg::Error nMsg: %d\n", nMsg);
						break;
					}
				}
			}

			return CLoginUtilDlg__Error(ecx, edx, nMsg, ppDialog);
		};
		
		return detours::redirect(enable, reinterpret_cast<void**>(&CLoginUtilDlg__Error), CLoginUtilDlg__Error_hook);
	}
	
	bool Hook_CLoginUtilDlg__YesNo(bool enable)
	{
		CLoginUtilDlg__YesNo_t CLoginUtilDlg__YesNo_hook = [](void* ecx, void* edx, int nMsg, ZRef<void*>* ppDialog) -> int
		{
			if (autologin_data::get_instance().enabled())
			{
				if (!ppDialog)
				{
					switch (nMsg)
					{
					case 0x1F: // This account has not been verified (too fast login)
						break;

					default:
						//printf("Unknown CLoginUtilDlg::YesNo nMsg: %d\n", nMsg);
						break;
					}
				}
			}

			return CLoginUtilDlg__YesNo(ecx, edx, nMsg, ppDialog);
		};
		
		return detours::redirect(enable, reinterpret_cast<void**>(&CLoginUtilDlg__YesNo), CLoginUtilDlg__YesNo_hook);
	}

	/* Enter region */
	bool Hook_CGateWayWnd__Update(bool enable)
	{
		CGateWayWnd__Update_t CGateWayWnd__Update_hook = [](void* ecx, void* edx) -> void
		{
			if (autologin_data::get_instance().enabled())
			{
				if (CLoadClientDataThread::IsInitialized() && CLoadClientDataThread::GetInstance()->IsLoadedAllLoginData())
					PostMessage(get_maplestory_window_handle(), WM_USER + 0xDEAD, 0, reinterpret_cast<LPARAM>(ecx));
			}

			return CGateWayWnd__Update(ecx, edx);
		};
		
		return detours::redirect(enable, reinterpret_cast<void**>(&CGateWayWnd__Update), CGateWayWnd__Update_hook);
	}
	
	/* Choose world and channel */
	bool Hook_CLogin__OnRecommendWorldMessage(bool enable)
	{
		CLogin__OnRecommendWorldMessage_t CLogin__OnRecommendWorldMessage_hook = [](void* ecx, void* edx, CInPacket* iPacket)
		{
			CLogin_ecx = ecx;
			CLogin__OnRecommendWorldMessage(ecx, edx, iPacket);

			try_select_world_channel();
		};
		
		return detours::redirect(enable, reinterpret_cast<void**>(&CLogin__OnRecommendWorldMessage), CLogin__OnRecommendWorldMessage_hook);
	}

	/* Choose character and enter pic */
	bool Hook_CUISelectChar__SelectCharacter(bool enable)
	{
		CUISelectChar__SelectCharacter_t CUISelectChar__SelectCharacter_hook = [](CUISelectChar* ecx, void* edx, int nIdx)
		{
			CUISelectChar__SelectCharacter(ecx, edx, nIdx);

			if (autologin_data::get_instance().enabled())
				ecx->select_character(autologin_data::get_instance().character(), autologin_data::get_instance().pic());
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&CUISelectChar__SelectCharacter), CUISelectChar__SelectCharacter_hook);
	}
	
	/* Offline mode */
	bool is_offline_mode_enabled()
	{
		return (autologin_data::get_instance().enabled() && autologin_data::get_instance().offline_mode());
	}
	
	void __declspec(naked) CLogin__Update_hook()
	{
		__asm
		{
			push eax
			call is_offline_mode_enabled
			movzx eax,al
			test eax,eax
			pop eax
			jz return_original
			
			mov eax,0x00000001

			return_original:
			jmp dword ptr [CLogin__Update]
		}
	}
	
	bool Hook_CLogin__Update(bool enable)
	{
		return detours::redirect(enable, reinterpret_cast<void**>(&CLogin__Update), CLogin__Update_hook);
	}

	void set_autologin_hooks(bool enable)
	{
		Hook_CLoginUtilDlg__Error(enable);
		Hook_CLoginUtilDlg__YesNo(enable);
		
		Hook_CGateWayWnd__Update(enable);
		Hook_CLogin__OnRecommendWorldMessage(enable);
		Hook_CUISelectChar__SelectCharacter(enable);

		Hook_CLogin__Update(enable);
	}
}