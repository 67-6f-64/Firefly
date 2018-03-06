#pragma once

#include "native.hpp"
#include "net_buffer.hpp"

#include "botting_types.hpp"

#include <functional>

namespace firefly
{
	enum class refresh_type
	{
		none = 0,
		people,
		timed,
	};

	enum class refresh_method
	{
		stop_botting = 0,
		change_channel,
		cash_shop,
		disconnect,
	};
	
	class auto_macro_entry
	{
	public:
		auto_macro_entry(std::function<bool()> condition = []() -> bool { return true; });

		bool should_use(unsigned int tick);

		void set_enabled(bool enable);
		void set_value(unsigned int value);
		void set_key(unsigned int key);
		void set_last_tick(unsigned int tick);

		bool enabled() const;
		unsigned int value() const;
		unsigned int key() const;
		unsigned int last_tick() const;
		bool condition() const;
		
	private:
		bool enabled_;
		unsigned int value_;
		unsigned int key_;
		unsigned int last_tick_;
		std::function<bool()> condition_;
	};

	class autobotting_data
	{
	public:
		static autobotting_data& get_instance()
		{
			static autobotting_data instance;
			return instance;
		}

		void handle_packet(net_buffer& packet);
		void handle_type(botting_type type, net_buffer& packet);

		int get_appropriate_channel(int current_channel, bool consider_custom_pool);

		bool enabled() const;
		auto_macro_entry& get_at(botting_type type);

		bool enable_auto_ability_point() const;
		unsigned char auto_ability_point_value() const;
		
		bool enable_auto_familiar() const;
		unsigned int& auto_familiar_last_tick();
		unsigned char auto_familiar_value() const;
		
		bool enable_auto_job_mount() const;
		unsigned int& auto_job_mount_last_tick();
		unsigned char auto_job_mount_value() const;

		bool enable_auto_escape() const;
		unsigned int auto_escape_people() const;
		refresh_method auto_escape_method() const;

		bool enable_auto_timed_escape() const;
		unsigned int auto_timed_escape_delay() const;
		refresh_method auto_timed_escape_method() const;
		unsigned int& auto_timed_escape_last_tick();
		
		bool enable_allow_names() const;
		std::vector<std::string> allow_names() const;

		bool is_people_refreshing() const;
		bool is_timed_refreshing() const;
		unsigned int& last_refresh_tick();

		void set_refreshing(bool refreshing, refresh_type type = refresh_type::none);
		void stop_refreshing(refresh_type type = refresh_type::none);

	private:
		autobotting_data();

	private:
		bool enabled_;
		
		std::vector<auto_macro_entry> auto_macro_entries_;

		bool enable_auto_ability_point_;
		unsigned char auto_ability_point_value_;
		
		bool enable_auto_familiar_;
		unsigned int auto_familiar_last_tick_;
		unsigned char auto_familiar_value_;
		
		bool enable_auto_job_mount_;
		unsigned int auto_job_mount_last_tick_;
		unsigned char auto_job_mount_value_;

		bool enable_auto_escape_;
		unsigned int auto_escape_people_;
		refresh_method auto_escape_method_;

		bool enable_auto_timed_escape_;
		unsigned int auto_timed_escape_delay_;
		refresh_method auto_timed_escape_method_;
		unsigned int auto_timed_escape_last_tick_;

		bool enable_custom_channel_pool_;
		std::vector<int> custom_channels_;
		
		bool enable_allow_names_;
		std::vector<std::string> allow_names_;

		bool people_refreshing_;
		bool timed_refreshing_;
		unsigned int last_refresh_tick_;
	};
}