#include "generic_hooks.hpp"
#include "generic_hook_data.hpp"

#include "detours.hpp"

#include <iostream>

namespace firefly
{
	/* DR_check */
	bool Hook_DR_check(bool enable)
	{
		DR_check_t DR_check_hook = [](DR_INFO* pDrInfo, unsigned int* pCheck, HINSTANCE hImageBase) -> int
		{
			return 0;
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&DR_check), DR_check_hook);
	}

	/* Skip Logo */
	void __declspec(naked) CLogo__UpdateLogo_hook()
	{
		__asm
		{
			mov eax,[CLogo__UpdateLogo_skip]
			add eax,0x06				// size of JE FAR instruction (0F 84 XX XX XX XX)
			jmp eax
		}
	}
	
	bool Hook_CLogo__UpdateLogo(bool enable)
	{
		return detours::redirect(enable, reinterpret_cast<void**>(&CLogo__UpdateLogo_skip), CLogo__UpdateLogo_hook);
	}
	
	/* Filter hidden mobs */
	bool Hook_CMobPool__OnMobEnterField(bool enable)
	{
		CMobPool__OnMobEnterField_t CMobPool__OnMobEnterField_hook = [](void* ecx, void* edx, CInPacket* iPacket) -> void
		{
			unsigned int dwTemplateID = iPacket->fetch_at<unsigned int>(CMobPool__OnMobEnterField_dwTemplateID_offset);

			if (dwTemplateID != 9999998 && dwTemplateID != 9999999)
				return CMobPool__OnMobEnterField(ecx, edx, iPacket);
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&CMobPool__OnMobEnterField), CMobPool__OnMobEnterField_hook);
	}

	bool Hook_CMobPool__SetLocalMob(bool enable)
	{
		CMobPool__SetLocalMob_t CMobPool__SetLocalMob_hook = [](void* ecx, void* edx, int nLevel, unsigned int dwMobId, int nCalcDamageIndex, CInPacket* iPacket, int bChangeController) -> void
		{
			unsigned int dwTemplateID = iPacket->fetch_at<unsigned int>(CMobPool__SetLocalMob_dwTemplateID_offset);

			if (dwTemplateID != 9999998 && dwTemplateID != 9999999)
				return CMobPool__SetLocalMob(ecx, edx, nLevel, dwMobId, nCalcDamageIndex, iPacket, bChangeController);
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&CMobPool__SetLocalMob), CMobPool__SetLocalMob_hook);
	}
	
	void __declspec(naked) WinMain_force_splash_screen_hook()
	{
		__asm
		{
			mov eax,[WinMain_force_splash_screen]
			add eax,0x06				// size of JL FAR instruction (0F 8X XX XX XX XX)
			jmp eax
		}
	}

	bool set_force_splash_screen(bool enable)
	{
		return detours::redirect(enable, reinterpret_cast<void**>(&WinMain_force_splash_screen), WinMain_force_splash_screen_hook);
	}

	void set_generic_hooks(bool enable)
	{
		Hook_DR_check(enable);
		Hook_CLogo__UpdateLogo(enable);
		Hook_CMobPool__OnMobEnterField(enable);
		Hook_CMobPool__SetLocalMob(enable);
	}
}