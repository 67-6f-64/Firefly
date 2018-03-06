#pragma once

namespace firefly
{
	enum net_opcodes
	{
		aes_init_key = 1,
		trainer_tabpage_toggle,
		trainer_fetch_data,
	};

	enum fetch_data_ids
	{
		spawncontrol_data = 0,
		rusher_data,
		dupex_data,
	};

	enum tabpage_ids
	{
		autobotting = 0,
		autoshop,
		autologin,
		hacks,
		kami,
		itemfilter,
		spawncontrol,
		exclusive,
		rusher,
		scripting,
		settings
	};
	
	enum autobotting_ap_type_ids
	{
		xenon_distributionx = 0,
		xenon_distributiony,
		xenon_distribution,
	};

	enum autoshop_type_ids
	{
		autoshop_type_unknown = 1,
		autoshop_toggle,
		autoshop_test_transfer_rush,
		autoshop_test_sell_rush,
	};

	enum kami_movement_type_ids
	{
		teleport = 0,
		portal,
	};

	enum kami_target_type_ids
	{
		random = 0,
		closest
	};

	enum itemfilter_type_ids
	{
		filter_everything = 0,
		filter_by_type,
		custom_filter
	};

	enum itemfilter_block_type_ids
	{
		blacklist = 0,
		whitelist
	};
}