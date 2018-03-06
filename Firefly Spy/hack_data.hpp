#pragma once

#include "native.hpp"
#include "net_buffer.hpp"
#include "net_opcodes.hpp"

#include "hack_types.hpp"

namespace firefly
{
	class hack_data
	{
	public:
		static hack_data& get_instance()
		{
			static hack_data instance;
			return instance;
		}

		void handle_packet(net_buffer& packet);
		
		bool enabled() const;
		bool set_enabled(bool enabled);

		bool is_hack_enabled(hack_type type) const;
		bool set_hack_enabled(hack_type type, bool enabled);

	private:
		hack_data();

	private:
		bool enabled_;
		std::vector<bool> hack_toggles;
	};
}