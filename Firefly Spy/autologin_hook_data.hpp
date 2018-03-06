#pragma once

#include "native.hpp"
#include "addresses.hpp"

#include "login_objects.hpp"
#include "packet_objects.hpp"

namespace firefly
{
	typedef void (__fastcall* CLoginUtilDlg__Error_t)(void* ecx, void* edx, int nMsg, ZRef<void*>* ppDialog);
	static CLoginUtilDlg__Error_t CLoginUtilDlg__Error = reinterpret_cast<CLoginUtilDlg__Error_t>(CLoginUtilDlg__Error_address);
	
	typedef int (__fastcall* CLoginUtilDlg__YesNo_t)(void* ecx, void* edx, int nMsg, ZRef<void*>* ppDialog);
	static CLoginUtilDlg__YesNo_t CLoginUtilDlg__YesNo = reinterpret_cast<CLoginUtilDlg__YesNo_t>(CLoginUtilDlg__YesNo_address);
	
	typedef void (__fastcall* CGateWayWnd__Update_t)(void* ecx, void* edx);
	static CGateWayWnd__Update_t CGateWayWnd__Update = reinterpret_cast<CGateWayWnd__Update_t>(CGateWayWnd__Update_address); 
			
	typedef void (__fastcall* CLogin__OnRecommendWorldMessage_t)(void* ecx, void* edx, CInPacket *iPacket);
	static CLogin__OnRecommendWorldMessage_t CLogin__OnRecommendWorldMessage = reinterpret_cast<CLogin__OnRecommendWorldMessage_t>(CLogin__OnRecommendWorldMessage_address);
	
	typedef void (__fastcall* CLogin__SendLoginPacket_t)(void* ecx, void* edx, int nWorldID, int nChannelID, int bForce);
	static CLogin__SendLoginPacket_t CLogin__SendLoginPacket = reinterpret_cast<CLogin__SendLoginPacket_t>(CLogin__SendLoginPacket_address);
	
	typedef void (__fastcall* CUISelectChar__SelectCharacter_t)(CUISelectChar* ecx, void* edx, int nIdx); 
	CUISelectChar__SelectCharacter_t CUISelectChar__SelectCharacter = reinterpret_cast<CUISelectChar__SelectCharacter_t>(CUISelectChar__SelectCharacter_address);
	
	static unsigned char* CLogin__Update = reinterpret_cast<unsigned char*>(CLogin__Update_address);
}