#include "autologin_data.hpp"

namespace firefly
{
	void autologin_data::handle_packet(net_buffer& packet)
	{
		if (this->enabled_ = (packet.read1() != FALSE))
		{
			this->username_ = packet.read_string();
			this->password_ = packet.read_string();
			this->pic_ = packet.read_string();

			this->region_ = packet.read1();
			this->world_ = packet.read1();
			this->channel_ = packet.read1();
			this->character_ = packet.read1();

			this->offline_mode_ = (packet.read1() != FALSE);
			this->randomize_hwid_ = (packet.read1() != FALSE);

			if (this->region_ == 0)
			{
				/* For Reboot */
				if (this->world_ == 17)
					this->world_ = 45;
			}
			else if (this->region_ == 1)
			{
				/* For Luna */
				if (this->world_ == 0)
					this->world_ = 30;
			}
		}
	}
	
	bool autologin_data::enabled() const
	{
		return this->enabled_;
	}
	
	std::string const& autologin_data::username() const
	{
		return this->username_;
	}

	std::string const& autologin_data::password() const
	{
		return this->password_;
	}

	std::string const& autologin_data::pic() const
	{
		return this->pic_;
	}
	
	unsigned char autologin_data::region() const
	{
		return this->region_;
	}

	unsigned char autologin_data::world() const
	{
		return this->world_;
	}

	unsigned char autologin_data::channel() const
	{
		return this->channel_;
	}

	unsigned char autologin_data::character() const
	{
		return this->character_;
	}
	
	bool autologin_data::offline_mode() const
	{
		return this->offline_mode_;
	}

	bool autologin_data::randomize_hwid() const
	{
		return this->randomize_hwid_;
	}
	
	autologin_data::autologin_data()
	{
		this->enabled_ = false;
		
		this->username_ = std::string("");
		this->password_ = std::string("");
		this->pic_ = std::string("");
		
		this->region_ = 0;
		this->world_ = 0;
		this->channel_ = 0;
		this->character_ = 0;

		this->offline_mode_ = false;
		this->randomize_hwid_ = false;
	}
}