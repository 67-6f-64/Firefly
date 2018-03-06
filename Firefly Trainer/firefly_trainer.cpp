#include "firefly_trainer.hpp"
#include "vps_socket.hpp"

#include "nmco_api\NMCOClientObject.h"

#ifdef _PRODUCTION
#include <VMProtectSDK.h>
#endif

namespace firefly
{
	firefly_trainer::firefly_trainer(HINSTANCE hinstance, std::string const& class_name, std::string const& window_name)
		: main_window(hinstance, class_name, window_name), game_sock(), game_aes(), manager_sock(), manager_aes(), launcher()
	{
		this->game_port = 0;
		this->game_desc = INVALID_SOCKET;

		this->load_data();

		this->game_sock.initialize_socket(true, this->handle(), WM_GAME_SOCKET);
		this->game_port = this->game_sock.bind_socket();
		this->game_sock.listen_socket();
	}

	firefly_trainer::~firefly_trainer()
	{

	}
	
#ifdef _PRODUCTION
#pragma optimize("g", off)
#endif
	bool firefly_trainer::authenticate(std::string const& email, std::string const& password)
	{
#ifdef _PRODUCTION
		VMProtectBeginVirtualization("firefly_trainer::authenticate");
#endif
		vps_socket sock(vps_hostname, vps_port);

		firefly_auth_buffer auth_buffer;
		memset(&auth_buffer, 0, sizeof(auth_buffer));

		strcpy(auth_buffer.email, email.c_str());
		strcpy(auth_buffer.password, password.c_str());

		return sock.make_vps_interaction(auth_type::type_firefly_authenticate, { version_major, version_minor }, &auth_buffer, sizeof(firefly_auth_buffer),
			[this](firefly::vps_socket* client) -> bool
		{
#ifdef _PRODUCTION
			VMProtectBeginVirtualization("firefly_trainer::authenticate::vps_interaction_finalize_handler");
#endif
			firefly::net_buffer packet;
			
			if (!client->read_packet(packet))
				return false;

			this->launcher.set_spy_file_map(packet.get_vector());

			if (!client->read_packet(packet))
				return false;

			this->launcher.set_blackcipher_spy_file_map(packet.get_vector());

			/* DISABLED DURING ALPHA - No active licenses will be on the alpha test account */
			//if (!client->read_packet(packet))
			//	return false;

			//for (unsigned int i = 0, count = packet.read4(); i < count; i++)
			//{
			//	unsigned int user_license_id = packet.read4();
			//	std::string bound_game_id = packet.read_string();
			//	std::string expires_at = packet.read_string();

			//	printf("License %d (Bound to ID: %s, expires at: %s)\n", user_license_id, bound_game_id.c_str(), expires_at.c_str());
			//}

			return true;
#ifdef _PRODUCTION
			VMProtectEnd();
#endif
		});
#ifdef _PRODUCTION
		VMProtectEnd();
#endif
	}

	bool firefly_trainer::launch_game(std::string const& directory, std::string const& username, std::string const& password)
	{
#ifdef _PRODUCTION
		VMProtectBeginVirtualization("firefly_trainer::launch_game");
#endif
		if (this->game_desc == INVALID_SOCKET)
		{
			if (this->launcher.start_process(directory, username, password))
			{
				this->game_sock.reset_buffers();
					
				if (this->launcher.send_communication_data(this->handle(), this->game_port))
					return true;

				this->launcher.terminate_process();
				return false;
			}
		}

		return false;
#ifdef _PRODUCTION
		VMProtectEnd();
#endif
	}
#ifdef _PRODUCTION
#pragma optimize("g", on)
#endif

	void firefly_trainer::handle_select_message(SOCKET sock, unsigned short select_event, unsigned short select_error, bool game)
	{
		if (select_error)
		{
			switch (select_error)
			{
			case WSAECONNABORTED:
				return this->on_close(sock, game);

			default:
				printf("Async select error: %d\n", select_error);
				break;
			}
		}
		else
		{
			switch (select_event)
			{
			case FD_ACCEPT:
				return this->on_accept(sock);

			case FD_CONNECT:
				return this->on_connect(sock);

			case FD_CLOSE:
				return this->on_close(sock, game);

			case FD_READ:
				return this->on_read(sock, game);

			case FD_WRITE:
				return this->on_write(sock, game);

			default:
				break;
			}
		}
	}

	void firefly_trainer::on_accept(SOCKET sock)
	{
		sockaddr_in sender_addr;
		int addr_length = sizeof(sockaddr_in);

		if ((this->game_desc = accept(sock, reinterpret_cast<sockaddr*>(&sender_addr), &addr_length)) != INVALID_SOCKET)
		{
			memcpy(this->game_aes.key, aes_hard_key, sizeof(this->game_aes.key));
			memcpy(this->game_aes.iv, aes_hard_iv, sizeof(this->game_aes.iv));
			
			if (this->game_sock.generate_key(desc_info(this->game_desc, this->game_aes), this->game_aes))
				this->on_accepted_game_connection(this->launcher.get_process_id());
		}
	}
	
	void firefly_trainer::on_connect(SOCKET sock)
	{
		memcpy(this->manager_aes.key, aes_hard_key, sizeof(this->manager_aes.key));
		memcpy(this->manager_aes.iv, aes_hard_iv, sizeof(this->manager_aes.iv));
	}

	void firefly_trainer::on_close(SOCKET sock, bool game)
	{
		if (game)
		{
			CNMCOClientObject::GetInstance().LogoutAuth();
			this->game_desc = INVALID_SOCKET;
			this->on_closed_game_connection();
		}
		else
		{
			this->manager_sock.uninitialize_socket();
			this->on_closed_manager_connection();
		}
	}

	void firefly_trainer::on_read(SOCKET sock, bool game)
	{
		if (game)
		{
			this->game_sock.read_collect(desc_info(this->game_desc, this->game_aes));

			net_buffer packet;

			while (this->game_sock.read_packet(desc_info(this->game_desc, this->game_aes), packet))
				this->handle_packet(packet);
		}
		else
		{
		
		}
	}

	void firefly_trainer::on_write(SOCKET sock, bool game)
	{
		if (game)
		{
			this->game_sock.write_flush(desc_info(this->game_desc, this->game_aes));
		}
		else
		{
		
		}
	}

	bool firefly_trainer::send_game_packet(net_buffer& packet)
	{
		if (this->game_desc == INVALID_SOCKET)
			return false;

		return this->game_sock.write_packet(desc_info(this->game_desc, this->game_aes), packet);
	}
	
	void firefly_trainer::handle_packet(net_buffer& packet)
	{
		unsigned short header = packet.read2();

		switch (header)
		{
		case trainer_fetch_data:
			return this->on_fetch_data(packet);

		default:
			break;
		}
	}

	void firefly_trainer::load_data()
	{
#ifndef _DEBUG
		this->items.load_items("Data\\Items.dat");
		this->maps.load_items("Data\\Maps.dat");
#endif
	}
}