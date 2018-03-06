#pragma once

#include "net_buffer.hpp"

namespace firefly
{
	class base_stage
	{
	public:
		virtual bool handle_handshake(unsigned char locale, unsigned short version_major, unsigned short version_minor) = 0;
		virtual bool handle_packet(unsigned short header, net_buffer& packet) = 0;

		virtual bool on_enter_stage() = 0;

	protected:
		base_stage()
		{

		}
	};
}