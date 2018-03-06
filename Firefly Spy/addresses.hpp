#pragma once

namespace firefly
{
	const unsigned int TSingleton_CDropPool = 0x02AF814C; // 8B 35 ? ? ? ? ? FF 50 ? ? 8B CE E8 ? ? ? ? B8
	const unsigned int TSingleton_CField = 0x02AF9844; // 
	const unsigned int TSingleton_CFuncKeyMappedMan = 0x02AF773C; // 8B 0D ? ? ? ? ? E8 ? ? ? ? 8B D8 8A 03
	const unsigned int TSingleton_CLoadClientDataThread = 0x02AF86A4; // 
	const unsigned int TSingleton_CMobPool = 0x02AF79B8; // 8B 0D ? ? ? ? 8D 45 ? 6A 00 50 E8 ? ? ? ? 8B F8 BE
	const unsigned int TSingleton_CPortalList = 0x02AF3120; // 8B 3D ? ? ? ? 8B 47 ? 85 C0
	const unsigned int TSingleton_CSkillInfo = 0x02AD95E8; // 
	const unsigned int TSingleton_CUIMiniMap = 0x02AF8658; //
	const unsigned int TSingleton_CUIStat = 0x02AFEF30; // 3D E8 03 00 00 75 ? 8B 0D ? ? ? ? E8 (right below)
	const unsigned int TSingleton_CUserLocal = 0x02AF1768; // 8B 0D ? ? ? ? 85 C9 74 ? 83 3D ? ? ? ? 00 74 ? 81
	const unsigned int TSingleton_CUserPool = 0x02AF79B4; // 83 3D ? ? ? ? 00 74 ? 81 FE ? ? ? ? 75 ? E8
	const unsigned int TSingleton_CWndMan = 0x02AF1CF4; // 8B 0D ? ? ? ? 8D 45 ? 6A 00 50 E8 ? ? ? ? 8B 0D
	const unsigned int TSingleton_CWvsContext = 0x02AD95EC; // 8B 0D ? ? ? ? 8B D8 8D 45
	const unsigned int TSingleton_CWvsPhysicalSpace2D = 0x02AF2254; // 8B 0D ? ? ? ? 85 C9 74 ? 8B 55 ? 6A 01

	const unsigned int TSingleton_CUIMiniMap_dwFieldID_offset = 0x1514; // 89 87 ? ? 00 00 C6 87 ? ? 00 00 01 E8
	const unsigned int TSingleton_CUserLocal_apFamiliar_offset = 0x125AC; // 8B 87 ? ? ? 00 8B 54 F0 04
	const unsigned int TSingleton_CWvsContext_nChannelID_offset = 0x21A0; // 8B 8F ? ? 00 00 89 8F ? ? 00 00 89
	const unsigned int TSingleton_CWvsContext_pCharacterData_offset = 0x223C; //

	const unsigned int GetLevel_address = 0x0200AA20; // 39 ? ? ? ? ? 0F 84 ? 00 00 00 E8 ? ? ? ? 8B 0D (call below)
	const unsigned int ZXString__Assign_address = 0x0078ACD0; // 83 C1 02 ? ? 8B CB E8 (call below)
	const unsigned int CDropPool__TryPickUpDrop_address = 0x00E9E9D0; // 3D FA 00 00 00 7D ? 83 ? ? 00 74 ? 83 ? ? 00 8B (start)
	const unsigned int CWvsContext__UI_Toggle_address = 0x0205B610; // 6A 00 6A 00 6A FF 68 FF 00 00 00 (start)
	const unsigned int CWvsContext__SendSpawnFamiliarRequest_address = 0x020BB600; // 6A 03 6A 00 68 88 13 00 00 (start)
	const unsigned int CSkillInfo__GetSkillLevel_address = 0x00A308C0; //
	const unsigned int CUserLocal__DoActiveSkill_address = 0x01EEE2A0; // B9 EC C8 10 00 (start)
	const unsigned int CUserLocal__TryDoingMeleeAttack_address = 0x01E353F0; // 
	const unsigned int CFuncKeyMappedMan__FuncKeyMapped_address = 0x010AB760; // 83 ? 59 77 ? 83 (start)

	const unsigned int CLife_size = 0x110; // 
	const unsigned int CUser_dwCharacterID_offset = 0xA884; // 

	const unsigned int CVecCtrl_size = 0x420; // 8B 87 ? ? 00 00 83 ? 05 0F 87 (same as CVecCtrlMob__WorkUpdateActive)
	const unsigned int CVecCtrl_pOwner_offset = 0xA0; // 8B 8F ? ? 00 00 6A 01 8D 41

	/* Generic hooks */
	const unsigned int DR_check_address = 0x00DBDE70; // C7 45 ? BD A1 DE 19 (1st call below)
	const unsigned int CLogo__UpdateLogo_skip_address = 0x011A6FF4; // 74 ? 2B F8 81 FF ? ? 00 00 0F
	
	const unsigned int CMobPool__OnMobEnterField_address = 0x01280070; // 75 ? FF 83 ? ? 00 00 FF (start)
	const unsigned int CMobPool__OnMobEnterField_dwTemplateID_offset = 0x08;

	const unsigned int CMobPool__SetLocalMob_address = 0x01280900; // E8 ? ? ? ? 8B 0D ? ? ? ? 6A 02 E8 ? ? ? ? 85 C0 (call)
	const unsigned int CMobPool__SetLocalMob_dwTemplateID_offset = 0x08;
	
	const unsigned int WinMain_force_splash_screen_address = 0x02054FDF; // 7C ? C6 45 FC ? E8 ? ? ? ? EB

	/* Auto Botting */
	const unsigned int CPet_m_nRepleteness_offset = 0x13C; // 0F B6 C0 89 87 ? ? 00 00 8D 46 
	const unsigned int CAvatar_m_nRidingVehicleID_offset = 0x7C0; // 8B 8B ? ? 00 00 B8 ? ? ? ? 0F 1F
	const unsigned int CAvatar_m_tAlertRemain_offset = 0x828; // C7 81 ? ? 00 00 88 13 00 00

	const unsigned int CPet__Update_address = 0x013B8BA0; // 8B 87 ? ? 00 00 83 ? 01 7C ? 83 ? 02 7F (start)
	const unsigned int CUserLocal__Update_address = 0x01E14120; // 6A 1E 6A 32 50 (1st) (start)
	const unsigned int GW_CharacterStat__DecodeChangeStat_address = 0x00805B80; // 83 E0 20 83 ? 00 74 ? 8B ? E8 ? ? ? ? 0F B7 C8 (start)
 
	const unsigned int CUserLocal__HandleUpKeyDown_address = 0x01E2B860; // 6A 00 68 63 43 23 00 (start)
	const unsigned int CWvsContext__SendMigrateToShopRequest_address = 0x0205A9B0; // 6A 01 FF 50 ? 8B 0D ? ? ? ? E8 (start)
	const unsigned int CField__SendTransferChannelRequest_address = 0x00F1AC90; // 33 FF FF B6 ? ? 00 00 8B (call below)
	const unsigned int CStage__OnPacket_address = 0x01476230; // 8D 87 ? ? FF FF 83 F8 0A 77 ? 8B 0D (call below)

	/* Auto Login */
	const unsigned int CUISelectChar_pLogin_offset = 0xB4; // 8B 8E ? ? 00 00 85 C9 0F 84 ? ? 00 00 8B 5D
	const unsigned int CLogin_nCharSelected_offset = 0x4A4; // E8 ? ? ? ? 84 C0 0F 84 ? ? 00 00 83 BF ? ? 00 00 00 0F 8C
	const unsigned int CLogin_sSPW_offset = 0x5F8; // Located in CLogin::Update (8B 87 ? ? 00 00 85 C0 0F 84 ? ? 00 00 80 38 00 0F 84 ? ? 00 00 85 F6)
	
	const unsigned int CLoginUtilDlg__Error_address = 0x0115B2E0; // 6A 00 6A 00 6A 00 6A 01 68 D2 00 00 00 FF 30 8B CE C6 45 ? ? 68 8E 00 00 00 68 F9 00 00 00 (1st) (start)
	const unsigned int CLoginUtilDlg__YesNo_address = 0x0115A8F0; // 6A 00 6A 54 E8 (call below)
	const unsigned int CGateWayWnd__Update_address = 0x010B0690; // 83 F8 01 75 ? 8B 0D ? ? ? ? 8D 45 ? 6A 00 (start)
	const unsigned int CLogin__OnRecommendWorldMessage_address = 0x01142500; // 83 BB ? ? ? ? 01 0F 85 ? ? ? ? 8B 4D (start)
	const unsigned int CLogin__SendLoginPacket_address = 0x01138F10; // E8 ? ? ? ? 80 3D ? ? ? ? 00 0F 85 (call)
	const unsigned int CUISelectChar__SelectCharacter_address = 0x01151830; // 83 E2 07 03 C2 C1 F8 03 39 (start)
	const unsigned int CLogin__Update_address = 0x01134D87; // 0F B6 87 ? ? 00 00 8D 4D ? 50 E8 ? ? ? ? 6A (address below)

	/* Kami */
	const unsigned int CUserLocal__GetJobCode_address = 0x01E06B50; // 56 8B F1 8B 06 FF 50 ? 8D 90 ? ? FF FF 83 FA ? 72 ? 3D D2 07 00 00 75 ? 80 (call)
	const unsigned int CVecCtrl__SetTeleportNext_address = 0x00B22AF0; // FF 75 ? 8D 71 ? FF 75 ? ? FF 50 ? 85 C0 79 ? 68 (2nd) (start)

	/* Hacks */
	const unsigned int CUserLocal__SetDamaged_address = 0x01E07870; // 74 ? 8D ? ? ? ? ? E8 ? ? ? ? 85 C0 0F 85 ? ? ? ? 8D ? ? ? ? ? E8 (start)
	
	const unsigned int CAvatar__CAvatar_address = 0x00CF951B; // 83 ? 06 6A 02

	// Ref (CDragon::DragonAttack): E8 ? ? ? ? 85 C0 75 ? 8B CE E8 ? ? ? ? 8B 75
	const unsigned int CUser__IsOnFoothold_address = 0x01D54140; // First call after reference
	const unsigned int CUser__IsOnLadderOrRope_address = 0x01D54150; // Second call after reference

	const unsigned int CVecCtrl__SetImpactNext_address = 0x00B22840; // 
	const unsigned int CVecCtrl__SetImpactNext_offset = 0x74; // 
	const unsigned int CAntiRepeat__TryRepeat_address = 0x01EE7630; // 2B C6 83 F8 FA 7E (start)
	
	const unsigned int CMob__Update_address = 0x0122AA43; // 
	const unsigned int CMob__Update_disarm_address = 0x0122B07C; //
	const unsigned int CMovePath__Flush_address = 0x012CAFA2; // 
	const unsigned int CMob__OnSpawn_address = 0x01230026; // 0F 8E ? ? 00 00 8B ? 33 ? E8
	const unsigned int CMob__OnDie_address = 0x01228A40; // 3D 33 BC 86 00 (start)
	const unsigned int CMob__AddDamageInfo_address = 0x01227920; // B9 1E 00 00 00 EB ? 81 (start)
	const unsigned int AccSpeed_address = 0x00B23BE0; //
	const unsigned int CVecCtrlMob__WorkUpdateActive_address = 0x01FE6D80; // 8B 87 ? ? 00 00 83 ? 05 0F 87 (start)

	const unsigned int CMapLoadable__RestoreBack_address = 0x011BBED0; // 
	const unsigned int CMapLoadable__RestoreTile_address = 0x011B5570; // 
	const unsigned int CMapLoadable__RestoreObj_address = 0x011B66B0; // 
	const unsigned int CMob__ShowDamage_address = 0x01241250; // 68 A4 14 06 C0 C7 00 00 00 00 00 FF (start)
	const unsigned int CStage__FadeIn_address =  0x014791D0; //
	const unsigned int CStage__FadeOut_address = 0x01479750; // 
	const unsigned int CWvsContext__OnStylishKillMessage_address = 0x020FA820; // 0F B6 C0 83 E8 00 74 ? 83 E8 01 0F 85 ? ? 00 00 8B CF (start)
	
	const unsigned int CStage__OnSetCashShop_address = 0x01478850; // 
	const unsigned int CCashShop__SendTransferFieldPacket_address = 0x00D862C0; //

	const unsigned int CUtilDlg__Notice_address = 0x01F8A330; //
	const unsigned int CUtilDlg__YesNo_1_address = 0x01F8A6E0; // E8 ? ? ? ? 83 C4 0C 83 F8 06 (call)
	const unsigned int CUtilDlg__YesNo_2_address = 0x01F8A530; // E8 ? ? ? ? 83 C4 1C 83 F8 06 (call)

	const unsigned int CField__OnCreateObtacle_address = 0x00FD8840; // 80 4A FE BF (start)
	const unsigned int CField__OnCreateFallingCatcher_address = 0x00FC5C10; // 
	const unsigned int CWvsContext__SetExclRequestSent_address = 0x02077B20; // 8D 8E ? ? 00 00 E8 ? ? ? ? E8 ? ? ? ? 50 (start)
	const unsigned int CMob__GetPos_address = 0x01236DA0; // 
	const unsigned int CMonsterFamiliar__TryDoingAttack_address = 0x00EE3200; // 

	/* Item Filter */
	const unsigned int CDropPool__OnDropEnterField_address = 0x00E9F530; // 83 F8 01 74 ? 83 F8 02 74 ? 32 C0 EB (start)
	
	const unsigned int OnDropEnterField_Offset_nEnterType = 0x03;
	const unsigned int OnDropEnterField_Offset_bIsMoney = 0x08;
	const unsigned int OnDropEnterField_Offset_nInfo = 0x15;

	/* Spawn Control */
	const unsigned int CUserLocal__Init_address = 0x01E0549D; // 83 ? F6 6A 00 6A 00 (address of call instruction) | CUserLocal::Init -> CVecCtrl::SetActive

	/* Exclusive */		
	const unsigned int CDropPool__TryPickUpDropByPet_address = 0x00E9EE80; // 83 7E ? 00 0F B7 D8 (start)
	const unsigned int CMobPool_FindHitMobInRect_address = 0x0127B4B0; //
	const unsigned int CUserLocal__TryDoingFinalAttack_address = 0x01F0DDC0; // 0F 84 ? ? ? ? 2B 9E ? ? ? ? 0F 88 ? ? ? ? 8B (start)
	const unsigned int CMob__GenerateMovePath_address = 0x0124D390; // C7 85 ? ? FF FF 01 00 00 00 C7 85 ? ? FF FF 01 00 00 00 EB (start) | or | 6A 0E 6A 00 6A 00 6A 00 6A 07 (call below)
}