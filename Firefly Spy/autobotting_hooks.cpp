#include "autobotting_hooks.hpp"
#include "autobotting_hook_data.hpp"
#include "autologin_hooks.hpp"

#include "autobotting_data.hpp"
#include "hack_data.hpp"
#include "rusher_data.hpp"

#include "detours.hpp"

#include "context_objects.hpp"
#include "drop_objects.hpp"
#include "keyboard_objects.hpp"
#include "mob_objects.hpp"
#include "stat_objects.hpp"

namespace firefly
{
	void no_blue_box_action(std::function<void()> no_blue_box_action_handler)
	{
		bool hack_enabled = hack_data::get_instance().set_enabled(true);
		bool boxes_enabled = hack_data::get_instance().set_hack_enabled(hack_type::no_blue_boxes, true);
					
		no_blue_box_action_handler();

		hack_data::get_instance().set_enabled(hack_enabled);
		hack_data::get_instance().set_hack_enabled(hack_type::no_blue_boxes, boxes_enabled);
	}

	bool try_changing_channel(bool consider_custom_pool)
	{
		if (*reinterpret_cast<void**>(TSingleton_CField) != nullptr && CWvsContext::IsInitialized())
		{
			no_blue_box_action([&]() -> void
			{
				CField__SendTransferChannelRequest(*reinterpret_cast<void**>(TSingleton_CField), nullptr, 
					autobotting_data::get_instance().get_appropriate_channel(CWvsContext::GetInstance()->get_channel(), consider_custom_pool));
			});

			return true;
		}

		return false;
	}
	
	void handle_refresh_method(refresh_method method, refresh_type type)
	{
		unsigned int tick = GetTickCount();

		switch (method)
		{
		case refresh_method::stop_botting: // Only available for people-evasion
			{
				if (CUserPool::size() <= static_cast<int>(autobotting_data::get_instance().auto_escape_people()))
					autobotting_data::get_instance().stop_refreshing(type);
			}
			break;

		case refresh_method::change_channel:
			{
				if (CUserLocal::IsInitialized() && CUserLocal::GetInstance()->get_breath() <= 0)
				{
					try_changing_channel(true);
				}
			}
			break;

		case refresh_method::cash_shop:
			{
				if (CUserLocal::IsInitialized() && CUserLocal::GetInstance()->get_breath() <= 0 && CWvsContext::IsInitialized())
				{
					no_blue_box_action([]() -> void
					{
						CWvsContext__SendMigrateToShopRequest(CWvsContext::GetInstance(), nullptr, NULL, false);
					});
				}
			}
			break;

		case refresh_method::disconnect:
			{
				/* do disconnection */
				autobotting_data::get_instance().stop_refreshing(type);
			}
			break;

		default:
			break;
		}
	}

	bool Hook_CUserLocal__Update(bool enable)
	{
		CUserLocal__Update_t CUserLocal__Update_hook = [](void* ecx, void* edx) -> void
		{
			if (rusher_data::get_instance().enabled())
			{
				switch (rusher_data::get_instance().stage())
				{
				case rusher_stage::changing_channel:
					try_changing_channel(true);
					break;

				case rusher_stage::rushing_maps:
					CUserLocal__HandleUpKeyDown(ecx, nullptr);
					break;

				default:
					break;
				}
			}
			else if (autobotting_data::get_instance().enabled())
			{
				unsigned int tick = GetTickCount();

				if (autobotting_data::get_instance().is_people_refreshing())
				{
					handle_refresh_method(autobotting_data::get_instance().auto_escape_method(), refresh_type::people);
					autobotting_data::get_instance().last_refresh_tick() = tick;
				}
				else if (autobotting_data::get_instance().is_timed_refreshing())
				{
					handle_refresh_method(autobotting_data::get_instance().auto_timed_escape_method(), refresh_type::timed);
					autobotting_data::get_instance().last_refresh_tick() = tick;
				}
				else if ((tick - autobotting_data::get_instance().last_refresh_tick()) >= 1500)
				{
					if (autobotting_data::get_instance().enable_auto_timed_escape() && (tick - autobotting_data::get_instance().auto_timed_escape_last_tick()) >= autobotting_data::get_instance().auto_timed_escape_delay())
					{
						autobotting_data::get_instance().set_refreshing(true, refresh_type::timed);
					}
					else if (autobotting_data::get_instance().enable_auto_escape() && (CUserPool::IsInitialized() && CUserPool::GetInstance()->filtered_size(autobotting_data::get_instance().enable_allow_names(), autobotting_data::get_instance().allow_names()) > static_cast<int>(autobotting_data::get_instance().auto_escape_people())))
					{
						autobotting_data::get_instance().set_refreshing(true, refresh_type::people);
					}
					else if (autobotting_data::get_instance().enable_auto_familiar() && reinterpret_cast<CUserLocal*>(ecx)->is_familiar_inactive())
					{
						if (reinterpret_cast<CUserLocal*>(ecx)->get_breath() <= 0 && (tick - autobotting_data::get_instance().auto_familiar_last_tick()) >= 2000)
						{
							unsigned char id = autobotting_data::get_instance().auto_familiar_value();

							if (id >= 0 && id < _countof(familiar_ids))
							{
								no_blue_box_action([&]() -> void
								{
									if (CWvsContext::IsInitialized())
										CWvsContext::GetInstance()->send_spawn_familiar_request(familiar_ids[id]);

									autobotting_data::get_instance().auto_familiar_last_tick() = tick;
								});
							}
						}
					}
					else if (autobotting_data::get_instance().enable_auto_job_mount() && reinterpret_cast<CUserLocal*>(ecx)->is_riding_vechicle() == 0)
					{
						if ((tick - autobotting_data::get_instance().auto_job_mount_last_tick()) >= 2000)
						{
							unsigned char id = autobotting_data::get_instance().auto_job_mount_value();

							if (id >= 0 && id < _countof(mount_skill_ids))
							{
								no_blue_box_action([&]() -> void
								{
									if (reinterpret_cast<CUserLocal*>(ecx)->do_active_skill(mount_skill_ids[id]))
										autobotting_data::get_instance().auto_job_mount_last_tick() = tick;
								});
							}
						}
					}
					else if (CFuncKeyMappedMan::IsInitialized())
					{
						for (int i = static_cast<int>(botting_type::auto_timed_skill_4); i >= static_cast<int>(botting_type::auto_attack); i--)
						{
							if (autobotting_data::get_instance().get_at(static_cast<botting_type>(i)).should_use(tick))
							{
								unsigned int skill_id = CFuncKeyMappedMan::GetInstance()->func_key_mapped(autobotting_data::get_instance().get_at(static_cast<botting_type>(i)).key());

								if (skill_id != 0 && reinterpret_cast<CUserLocal*>(ecx)->do_active_skill(skill_id))
								{
									autobotting_data::get_instance().get_at(static_cast<botting_type>(i)).set_last_tick(tick);
									break;
								}
							}
						}

						if (CDropPool::IsInitialized() && autobotting_data::get_instance().get_at(botting_type::auto_loot).should_use(tick))
						{
							CVecCtrl* userlocal_vecctrl = reinterpret_cast<CUserLocal*>(ecx)->GetVecCtrl();
							POINT pt_user = { static_cast<LONG>(userlocal_vecctrl->m_ap.x), static_cast<LONG>(userlocal_vecctrl->m_ap.y) };
							
							CDropPool::GetInstance()->try_pick_up_drop(&pt_user);
							
							autobotting_data::get_instance().get_at(botting_type::auto_loot).set_last_tick(tick);
						}
					}
				}
			}

			return CUserLocal__Update(ecx, edx);
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&CUserLocal__Update), CUserLocal__Update_hook);
	}
	
	bool Hook_CPet__Update(bool enable)
	{
		CPet__Update_t CPet__Update_hook = [](CPet* ecx, void* edx) -> void
		{
			if (autobotting_data::get_instance().get_at(botting_type::auto_feed_pet).enabled())
			{
				if (CUserLocal::IsInitialized() && ecx->m_pOwner == CUserLocal::GetInstance())
				{
					if (CWndMan::IsInitialized())
					{
						if (ecx->m_nPetIndex >= 0 && ecx->m_nPetIndex <= 2 && ecx->m_nRepleteness < static_cast<int>(autobotting_data::get_instance().get_at(botting_type::auto_feed_pet).value()))
							CWndMan::GetInstance()->send_key(autobotting_data::get_instance().get_at(botting_type::auto_feed_pet).key());
					}
				}
			}

			return CPet__Update(ecx, edx);
		};

		return detours::redirect(enable, reinterpret_cast<void**>(&CPet__Update), CPet__Update_hook);
	}
	
	bool Hook_GW_CharacterStat__DecodeChangeStat(bool enable)
	{
		GW_CharacterStat__DecodeChangeStat_t GW_CharacterStat__DecodeChangeStat_hook = [](GW_CharacterStat* ecx, void* edx, CInPacket *iPacket, int bBattlePvP) -> unsigned int
		{
			unsigned int flags = GW_CharacterStat__DecodeChangeStat(ecx, edx, iPacket, bBattlePvP);

			if (autobotting_data::get_instance().enabled())
			{
				const unsigned int hp_flag = 0x400;
				const unsigned int max_hp_flag = 0x800;

				if (((flags & hp_flag) || (flags & max_hp_flag)) && autobotting_data::get_instance().get_at(botting_type::auto_hp).enabled())
				{
					if (CWvsContext::IsInitialized() && CWndMan::IsInitialized())
					{
						double hp_percentage = (static_cast<double>(ecx->get_hp()) / static_cast<double>(CWvsContext::GetInstance()->get_max_hp())) * 100.0;
						
						if (hp_percentage < static_cast<float>(autobotting_data::get_instance().get_at(botting_type::auto_hp).value()))
							CWndMan::GetInstance()->send_key(autobotting_data::get_instance().get_at(botting_type::auto_hp).key());
					}
				}
				
				const unsigned int mp_flag = 0x1000;
				const unsigned int max_mp_flag = 0x2000;

				if (((flags & mp_flag) || (flags & max_mp_flag)) && autobotting_data::get_instance().get_at(botting_type::auto_mp).enabled())
				{
					if (CWvsContext::IsInitialized() && CWndMan::IsInitialized())
					{
						double mp_percentage = (static_cast<double>(ecx->get_mp()) / static_cast<double>(CWvsContext::GetInstance()->get_max_mp())) * 100.0;
						
						if (mp_percentage < static_cast<float>(autobotting_data::get_instance().get_at(botting_type::auto_mp).value()))
							CWndMan::GetInstance()->send_key(autobotting_data::get_instance().get_at(botting_type::auto_mp).key());
					}
				}

				const unsigned int ap_flag = 0x4000;

				if ((flags & ap_flag) && autobotting_data::get_instance().enable_auto_ability_point())
				{
					if (CWvsContext::IsInitialized())
					{
						unsigned char id = autobotting_data::get_instance().auto_ability_point_value();

						no_blue_box_action([&]() -> void
						{
							if (!CUIStat::IsInitialized())
								CWvsContext::GetInstance()->toggle_ui(ui_types::UI_STAT);

							if (CUIStat::IsInitialized())
							{
								CUIStat::GetInstance()->click_button(id);
								CWvsContext::GetInstance()->toggle_ui(ui_types::UI_STAT);
							}
						});
					}
				}
			}

			return flags;
		};
		
		return detours::redirect(enable, reinterpret_cast<void**>(&GW_CharacterStat__DecodeChangeStat), GW_CharacterStat__DecodeChangeStat_hook);
	}
	
	bool Hook_CStage__OnPacket(bool enable)
	{
		CStage__OnPacket_t CStage__OnPacket_hook = [](void* ecx, void* edx, int nType, CInPacket* iPacket) -> void
		{
			CLogin_ecx = nullptr;

			if (rusher_data::get_instance().enabled())
			{
				if (rusher_data::get_instance().stage() == rusher_stage::changing_channel)
				{
					rusher_data::get_instance().set_stage(rusher_stage::rushing_maps);
				}
			}
			else if (autobotting_data::get_instance().enabled())
			{
				if (autobotting_data::get_instance().enable_auto_escape() && autobotting_data::get_instance().is_people_refreshing())
				{
					autobotting_data::get_instance().stop_refreshing(refresh_type::people);
				}
				else if (autobotting_data::get_instance().enable_auto_timed_escape() && autobotting_data::get_instance().is_timed_refreshing())
				{
					autobotting_data::get_instance().stop_refreshing(refresh_type::timed);
				}
			}

			return CStage__OnPacket(ecx, edx, nType, iPacket);
		};
		
		return detours::redirect(enable, reinterpret_cast<void**>(&CStage__OnPacket), CStage__OnPacket_hook);
	}
	
	void set_autobotting_hooks(bool enable)
	{
		Hook_CUserLocal__Update(enable);
		Hook_CPet__Update(enable);
		Hook_GW_CharacterStat__DecodeChangeStat(enable);
		Hook_CStage__OnPacket(enable);
	}
}