#pragma once

#include "native.hpp"
#include "net_buffer.hpp"
#include "net_opcodes.hpp"

#include "rusher_entry.hpp"

namespace firefly
{
	enum class rusher_stage
	{
		changing_channel,
		rushing_maps,
	};

	class rusher_data
	{
	public:
		static rusher_data& get_instance()
		{
			static rusher_data instance;
			return instance;
		}

		void handle_packet(net_buffer& packet);
		bool get_entry(unsigned int map_id, rusher_entry& entry);

		bool enabled() const;
		void disable();

		rusher_stage stage() const;
		void set_stage(rusher_stage stage);

	private:
		rusher_data();

	private:
		bool enabled_;
		std::vector<rusher_entry> rusher_list_;
		rusher_stage stage_;
	};
}