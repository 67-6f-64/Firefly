#include "autobotting_data.hpp"

#include "drop_objects.hpp"
#include "mob_objects.hpp"

#include <random>

namespace firefly
{
	auto_macro_entry::auto_macro_entry(std::function<bool()> condition)
		: enabled_(false), value_(0), key_(0), last_tick_(0), condition_(condition)
	{

	}
	
	bool auto_macro_entry::should_use(unsigned int tick)
	{
		return (this->enabled() && this->condition() && (tick - this->last_tick()) >= this->value());
	}

	void auto_macro_entry::set_enabled(bool enable)
	{
		this->enabled_ = enable;
	}

	void auto_macro_entry::set_value(unsigned int value)
	{
		this->value_ = value;
	}
	
	void auto_macro_entry::set_key(unsigned int key)
	{
		this->key_ = key;
	}
	void auto_macro_entry::set_last_tick(unsigned int tick)
	{
		this->last_tick_ = tick;
	}
	
	bool auto_macro_entry::enabled() const
	{
		return this->enabled_;
	}

	unsigned int auto_macro_entry::value() const
	{
		return this->value_;
	}
	
	unsigned int auto_macro_entry::key() const
	{
		return this->key_;
	}

	unsigned int auto_macro_entry::last_tick() const
	{
		return this->last_tick_;
	}
	
	bool auto_macro_entry::condition() const
	{
		return this->condition_();
	}

	void autobotting_data::handle_packet(net_buffer& packet)
	{
		unsigned char index = packet.read1();

		if (index == static_cast<unsigned char>(-1))
		{
			if (this->enabled_ = (packet.read1() != FALSE))
			{
				for (int i = static_cast<int>(botting_type::auto_hp); i <= static_cast<int>(botting_type::auto_timed_skill_4); i++)
					this->handle_type(static_cast<botting_type>(i), packet);

				this->handle_type(botting_type::auto_ability_points, packet);
				this->handle_type(botting_type::auto_familiar, packet);
				this->handle_type(botting_type::auto_job_mount, packet);
				this->handle_type(botting_type::auto_evade, packet);
				this->handle_type(botting_type::auto_refresh, packet);
				this->handle_type(botting_type::custom_channel_pool, packet);
				this->handle_type(botting_type::allow_names, packet);

				this->last_refresh_tick_ = 0;
			}
		}
		else
		{
			return this->handle_type(static_cast<botting_type>(index), packet);
		}
	}

	void autobotting_data::handle_type(botting_type type, net_buffer& packet)
	{
		switch (type)
		{
		case botting_type::auto_hp:
		case botting_type::auto_mp:
		case botting_type::auto_feed_pet:
		case botting_type::auto_loot:
		case botting_type::auto_attack:
		case botting_type::auto_timed_skill_1:
		case botting_type::auto_timed_skill_2:
		case botting_type::auto_timed_skill_3:
		case botting_type::auto_timed_skill_4:
			this->auto_macro_entries_.at(static_cast<std::size_t>(type)).set_enabled(packet.read1() != FALSE);
			this->auto_macro_entries_.at(static_cast<std::size_t>(type)).set_last_tick(0);

			if (this->auto_macro_entries_.at(static_cast<std::size_t>(type)).enabled())
			{
				this->auto_macro_entries_.at(static_cast<std::size_t>(type)).set_value(packet.read4());
				this->auto_macro_entries_.at(static_cast<std::size_t>(type)).set_key(packet.read4());
			}

			break;

		case botting_type::auto_ability_points:
			if (this->enable_auto_ability_point_ = (packet.read1() != FALSE))
				this->auto_ability_point_value_ = packet.read1();
			
			break;

		case botting_type::auto_familiar:
			if (this->enable_auto_familiar_ = (packet.read1() != FALSE))
			{
				this->auto_familiar_last_tick_ = 0;
				this->auto_familiar_value_ = packet.read1();
			}

			break;

		case botting_type::auto_job_mount:	
			if (this->enable_auto_job_mount_ = (packet.read1() != FALSE))
			{
				this->auto_job_mount_last_tick_ = 0;
				this->auto_job_mount_value_ = packet.read1();
			}

			break;

		case botting_type::auto_evade:
			if (this->enable_auto_escape_ = (packet.read1() != FALSE))
			{
				this->stop_refreshing(refresh_type::people);

				this->auto_escape_people_ = packet.read4();
				this->auto_escape_method_ = static_cast<refresh_method>(packet.read1());
			}

			break;

		case botting_type::auto_refresh:
			if (this->enable_auto_timed_escape_ = (packet.read1() != FALSE))
			{
				this->stop_refreshing(refresh_type::timed);

				this->auto_timed_escape_delay_ = packet.read4() * 1000;
				this->auto_timed_escape_method_ = static_cast<refresh_method>(packet.read1() + 1);
				this->auto_timed_escape_last_tick_ = GetTickCount();
			}

			break;

		case botting_type::custom_channel_pool:
			if (this->enable_custom_channel_pool_ = (packet.read1() != FALSE))
			{
				this->custom_channels_.clear();

				for (unsigned int i = 0, pool_size = packet.read1(); i < pool_size; i++)
				{
					if (packet.read1() != FALSE)
						this->custom_channels_.push_back(i);
				}
			}

			break;

		case botting_type::allow_names:
			if (this->enable_allow_names_ = (packet.read1() != FALSE))
			{
				this->allow_names_.clear();

				for (unsigned int i = 0, names_count = packet.read4(); i < names_count; i++)
					this->allow_names_.push_back(packet.read_string());
			}

			break;
				
		default:
			break;
		}
	}
	
	int autobotting_data::get_appropriate_channel(int current_channel, bool consider_custom_pool)
	{
		static std::random_device rd;
		static std::mt19937 mt(rd());

		int new_channel = 0;

		do
		{
			if (consider_custom_pool && this->enable_custom_channel_pool_)
			{
				std::uniform_int_distribution<int> dist(0, this->custom_channels_.size() - 1);
				new_channel = this->custom_channels_.at(dist(mt));
			}
			else
			{
				std::uniform_int_distribution<int> dist(0, 20 - 1);
				new_channel = dist(mt);
			}
		}
		while (new_channel == current_channel);

		return new_channel;
	}

	bool autobotting_data::enabled() const
	{
		return this->enabled_;
	}

	auto_macro_entry& autobotting_data::get_at(botting_type type)
	{
		return this->auto_macro_entries_.at(static_cast<std::size_t>(type));
	}

	bool autobotting_data::enable_auto_ability_point() const
	{
		return this->enable_auto_ability_point_;
	}

	unsigned char autobotting_data::auto_ability_point_value() const
	{
		return this->auto_ability_point_value_;
	}
	
	bool autobotting_data::enable_auto_familiar() const
	{
		return this->enable_auto_familiar_;
	}
	
	unsigned int& autobotting_data::auto_familiar_last_tick()
	{
		return this->auto_familiar_last_tick_;
	}

	unsigned char autobotting_data::auto_familiar_value() const
	{
		return this->auto_familiar_value_;
	}
	
	bool autobotting_data::enable_auto_job_mount() const
	{
		return this->enable_auto_job_mount_;
	}
	
	unsigned int& autobotting_data::auto_job_mount_last_tick()
	{
		return this->auto_job_mount_last_tick_;
	}

	unsigned char autobotting_data::auto_job_mount_value() const
	{
		return this->auto_job_mount_value_;
	}

	bool autobotting_data::enable_auto_escape() const
	{
		return this->enable_auto_escape_;
	}

	unsigned int autobotting_data::auto_escape_people() const
	{
		return this->auto_escape_people_;
	}

	refresh_method autobotting_data::auto_escape_method() const
	{
		return this->auto_escape_method_;
	}

	bool autobotting_data::enable_auto_timed_escape() const
	{
		return this->enable_auto_timed_escape_;
	}

	unsigned int autobotting_data::auto_timed_escape_delay() const
	{
		return this->auto_timed_escape_delay_;
	}

	refresh_method autobotting_data::auto_timed_escape_method() const
	{
		return this->auto_timed_escape_method_;
	}
	
	unsigned int& autobotting_data::auto_timed_escape_last_tick()
	{
		return this->auto_timed_escape_last_tick_;
	}
	
	bool autobotting_data::enable_allow_names() const
	{
		return this->enable_allow_names_;
	}

	std::vector<std::string> autobotting_data::allow_names() const
	{
		return this->allow_names_;
	}

	bool autobotting_data::is_people_refreshing() const
	{
		return this->people_refreshing_;
	}

	bool autobotting_data::is_timed_refreshing() const
	{
		return this->timed_refreshing_;
	}
	
	unsigned int& autobotting_data::last_refresh_tick()
	{
		return this->last_refresh_tick_;
	}

	void autobotting_data::set_refreshing(bool refreshing, refresh_type type)
	{
		switch (type)
		{
		case refresh_type::none:
			break;

		case refresh_type::people:
			this->people_refreshing_ = refreshing;
			break;

		case refresh_type::timed:
			this->timed_refreshing_ = refreshing;
			break;

		default:
			break;
		}
	}

	void autobotting_data::stop_refreshing(refresh_type type)
	{
		switch (type)
		{
		case refresh_type::none:
		case refresh_type::people:
			break;

		case refresh_type::timed:
			this->auto_timed_escape_last_tick_ = GetTickCount();
			break;
			
		default:
			break;
		}
		
		this->set_refreshing(false, type);
	}

	autobotting_data::autobotting_data()
	{
		this->enabled_ = false;
		
		std::function<bool()> conditions[] = 
		{
			nullptr, // Auto HP	
			nullptr, // Auto MP
			nullptr, // Auto Feed Pet
				[]() -> bool // Auto Loot 
			{
				return (CDropPool::size() > 0);
			},
				[]() -> bool // Auto Attack
			{
				return (CMobPool::size() > 0);
			},
			nullptr, // Auto Timed Skill 1
			nullptr, // Auto Timed Skill 2
			nullptr, // Auto Timed Skill 3
			nullptr, // Auto Timed Skill 4
		};

		for (int i = static_cast<int>(botting_type::auto_hp); i <= static_cast<int>(botting_type::auto_timed_skill_4); i++)
			this->auto_macro_entries_.push_back(conditions[i] ? auto_macro_entry(conditions[i]) : auto_macro_entry());

		this->enable_auto_ability_point_ = false;
		this->auto_ability_point_value_ = 0;
		
		this->enable_auto_familiar_ = false;
		this->auto_familiar_last_tick_ = 0;
		this->auto_familiar_value_ = 0;
		
		this->enable_auto_job_mount_ = false;
		this->auto_job_mount_last_tick_ = 0;
		this->auto_job_mount_value_ = 0;

		this->enable_auto_escape_ = false;
		this->auto_escape_people_ = 0;
		this->auto_escape_method_ = refresh_method::stop_botting;

		this->enable_auto_timed_escape_ = false;
		this->auto_timed_escape_delay_ = 0;
		this->auto_timed_escape_method_ = refresh_method::change_channel;
		this->auto_timed_escape_last_tick_ = 0;

		this->enable_custom_channel_pool_ = false;
		this->custom_channels_.clear();

		this->enable_allow_names_ = false;
		this->allow_names_.clear();
		
		this->people_refreshing_ = false;
		this->timed_refreshing_ = false;
		this->last_refresh_tick_ = 0;
	}
}