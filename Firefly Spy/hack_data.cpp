#include "hack_data.hpp"

namespace firefly
{
	void hack_data::handle_packet(net_buffer& packet)
	{
		unsigned char index = packet.read1();
		
		if (index == static_cast<unsigned char>(-1))
		{
			if (this->enabled_ = (packet.read1() != FALSE))
			{
				for (int i = 0, count = packet.read4(); i < count; i++)
				{
					this->hack_toggles.at(i) = (packet.read1() != FALSE);
				}
			}
		}
		else
		{
			this->hack_toggles.at(index) = (packet.read1() != FALSE);	
		}
	}

	bool hack_data::enabled() const
	{
		return this->enabled_;
	}
	
	bool hack_data::set_enabled(bool enabled)
	{
		bool old_state = this->enabled_;
		this->enabled_ = enabled;
		return old_state;
	}

	bool hack_data::is_hack_enabled(hack_type type) const
	{
		return this->hack_toggles.at(static_cast<std::size_t>(type));
	}
	
	bool hack_data::set_hack_enabled(hack_type type, bool enabled)
	{
		bool old_state = this->hack_toggles.at(static_cast<std::size_t>(type));
		this->hack_toggles.at(static_cast<std::size_t>(type)) = enabled;
		return old_state;
	}

	hack_data::hack_data()
	{
		this->enabled_ = false;

		for (int i = 0; i < static_cast<int>(hack_type::hack_type_count); i++)
			this->hack_toggles.push_back(false);
	}
}