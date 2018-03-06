#include "network_client.hpp"
#include "net_opcodes.hpp"

#include "utility.hpp"
#include "output.hpp"

#include "autobotting_data.hpp"
#include "autoshop_data.hpp"
#include "autologin_data.hpp"

#include "kami_data.hpp"
#include "hack_data.hpp"
#include "itemfilter_data.hpp"
#include "spawncontrol_data.hpp"
#include "exclusive_data.hpp"

#include "rusher_data.hpp"
//#include "scripting_data.hpp"

#include "life_objects.hpp"
#include "map_objects.hpp"

namespace firefly
{
	void network_client::initialize(HMODULE module)
	{
		this->sock.create_callback_window(static_cast<HINSTANCE>(module), this);
		this->sock.initialize_socket(false, NULL, WM_GAME_SOCKET);
	}

	void network_client::connect_client(unsigned short port)
	{
		this->sock.connect_socket("127.0.0.1", port);
	}

	void network_client::on_socket_message(SOCKET desc, unsigned short select_event, unsigned short select_error)
	{
		if (select_error)
		{
			switch (select_error)
			{
			case WSAECONNABORTED:
				return this->on_close(desc);

			default:
				printf("[Client] Async select error: %d\n", select_error);
				break;
			}
		}
		else
		{
			switch (select_event)
			{
			case FD_CONNECT:
				return this->on_connect(desc);

			case FD_CLOSE:
				return this->on_close(desc);

			case FD_READ:
				return this->on_read(desc);

			case FD_WRITE:
				return this->on_write(desc);

			default:
				break;
			}
		}
	}

	bool network_client::send_firefly_packet(net_buffer& packet)
	{
		if (this->sock.sock() == INVALID_SOCKET)
			return false;

		return this->sock.write_packet(desc_info(this->sock.sock(), this->aes), packet);
	}
	
	network_client::network_client()
		: sock(), aes()
	{

	}

	network_client::~network_client()
	{

	}

	void network_client::on_connect(SOCKET desc)
	{
		memcpy(this->aes.key, aes_hard_key, sizeof(this->aes.key));
		memcpy(this->aes.iv, aes_hard_iv, sizeof(this->aes.iv));
	}

	void network_client::on_close(SOCKET desc)
	{
		utility::invoke_crash();
	}

	void network_client::on_read(SOCKET desc)
	{
		this->sock.read_collect(desc_info(this->sock.sock(), this->aes));

		net_buffer packet;

		while (this->sock.read_packet(desc_info(this->sock.sock(), this->aes), packet))
			this->handle_packet(packet);
	}

	void network_client::on_write(SOCKET desc)
	{
		this->sock.write_flush(desc_info(this->sock.sock(), this->aes));
	}
	
	void network_client::handle_packet(net_buffer& packet)
	{
		unsigned short header = packet.read2();

		switch (header)
		{
		case aes_init_key:
			return this->on_aes_key(packet);

		case trainer_tabpage_toggle:
			return this->on_trainer_toggle(packet);

		case trainer_fetch_data:
			return this->on_fetch_data(packet);

		default:
			printf("header: %d\n", header);
			break;
		}
	}

	void network_client::on_aes_key(net_buffer& packet)
	{
		packet.read_buffer(reinterpret_cast<unsigned char*>(&this->aes), sizeof(this->aes));
	}
	
	void network_client::on_trainer_toggle(net_buffer& packet)
	{
		tabpage_ids tabpage_id = static_cast<tabpage_ids>(packet.read1());

		switch (tabpage_id)
		{
		case tabpage_ids::autobotting:
			return autobotting_data::get_instance().handle_packet(packet);

		case tabpage_ids::autoshop:
			return autoshop_data::get_instance().handle_packet(packet);

		case tabpage_ids::autologin:
			return autologin_data::get_instance().handle_packet(packet);

		case tabpage_ids::kami:
			return kami_data::get_instance().handle_packet(packet);

		case tabpage_ids::hacks:
			return hack_data::get_instance().handle_packet(packet);
			
		case tabpage_ids::itemfilter:
			return itemfilter_data::get_instance().handle_packet(packet);

		case tabpage_ids::spawncontrol:
			return spawncontrol_data::get_instance().handle_packet(packet);

		case tabpage_ids::exclusive:
			return exclusive_data::get_instance().handle_packet(packet);

		case tabpage_ids::rusher:
			return rusher_data::get_instance().handle_packet(packet);

		case tabpage_ids::scripting:
			break;

		default:
			break;
		}
	}
	
	void network_client::on_fetch_data(net_buffer& packet)
	{
		fetch_data_ids fetch_data_id = static_cast<fetch_data_ids>(packet.read1());

		switch (fetch_data_id)
		{
		case fetch_data_ids::spawncontrol_data:
			return this->on_fetch_spawncontrol_data();

		case fetch_data_ids::rusher_data:
			return this->on_fetch_rusher_data(packet);

		case fetch_data_ids::dupex_data:
			return this->on_fetch_dupex_data();

		default:
			break;
		}
	}
	
	void network_client::on_fetch_spawncontrol_data()
	{
		net_buffer fetch_packet(trainer_fetch_data);
		fetch_packet.write1(fetch_data_ids::spawncontrol_data);
		fetch_packet.write4(CUIMiniMap::IsInitialized() ? CUIMiniMap::GetInstance()->get_map_id() : 0);
		
		if (CUserLocal::IsInitialized())
		{
			CVecCtrl* user_vector = CUserLocal::GetInstance()->GetVecCtrl();
			fetch_packet.write4(static_cast<unsigned int>(user_vector->m_ap.x));
			fetch_packet.write4(static_cast<unsigned int>(user_vector->m_ap.y));
		}
		else
		{
			fetch_packet.write4(0);
			fetch_packet.write4(0);
		}

		this->send_firefly_packet(fetch_packet);
	}
	
	void network_client::on_fetch_rusher_data(net_buffer& packet)
	{
		net_buffer fetch_packet(trainer_fetch_data);
		fetch_packet.write1(fetch_data_ids::rusher_data);
		fetch_packet.write4(CUIMiniMap::IsInitialized() ? CUIMiniMap::GetInstance()->get_map_id() : 0);
		fetch_packet.write4(packet.read4());
		fetch_packet.write1(packet.read1());

		this->send_firefly_packet(fetch_packet);
	}
	
	void network_client::on_fetch_dupex_data()
	{
		net_buffer fetch_packet(trainer_fetch_data);
		fetch_packet.write1(fetch_data_ids::dupex_data);
		
		if (CUserLocal::IsInitialized())
		{
			CVecCtrl* user_vector = CUserLocal::GetInstance()->GetVecCtrl();
			fetch_packet.write4(static_cast<unsigned int>(user_vector->m_ap.x));
			fetch_packet.write4(static_cast<unsigned int>(user_vector->m_ap.y));
		}
		else
		{
			fetch_packet.write4(0);
			fetch_packet.write4(0);
		}

		this->send_firefly_packet(fetch_packet);
	}
}