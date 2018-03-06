#include "game_stage.hpp"

#include "opcodes.hpp"
#include "system_info.hpp"

namespace firefly
{
	bool game_stage::handle_handshake(unsigned char locale, unsigned short version_major, unsigned short version_minor)
	{
		unsigned char machine_id[16];
		system_info::get_machine_id(machine_id);

		net_buffer out_packet(opcode::out::CP_MigrateIn);
		out_packet.write4(this->account.physical_world_id);
		out_packet.write4(this->session.character_id);
		out_packet.write_buffer(machine_id, sizeof(machine_id));
		out_packet.write1(0x00);									// bIsUserGM
		out_packet.write1(0x00);									// <unknown>
		out_packet.write8(this->session.session_id);
			
		return this->sock->send_packet(out_packet);
	}

	bool game_stage::handle_packet(unsigned short header, net_buffer& packet)
	{
		switch (header)
		{
		default:
			break;
		}

		return true;
	}
	
	bool game_stage::on_enter_stage()
	{
		printf("[game_stage] entering stage\n");
		return true;
	}
}