#include "vps_socket.hpp"

#ifdef _PRODUCTION
#include <VMProtectSDK.h>
#endif

namespace firefly
{
	vps_socket::vps_socket(std::string const& hostname, unsigned short port)
		: net_socket_sync(), aes(), hostname(hostname), port(port)
	{

	}

	vps_socket::~vps_socket()
	{

	}

#ifdef _PRODUCTION
#pragma optimize("g", off)
#endif
	bool vps_socket::make_vps_interaction(auth_type type, auth_version version_info, void* data, std::size_t length, interaction_handler finalize_handler)
	{
#ifdef _PRODUCTION
		VMProtectBeginVirtualization("vps_socket::make_vps_interaction");
#endif
		if (!this->initialize_socket(false))
		{
			this->output_wsa_error(error_type::initialize);
			return false;
		}

		if (!this->connect_socket(hostname, port, identifier_type::ip))
		{
			this->output_wsa_error(error_type::connect);
			return false;
		}

		if (!this->exchange_keys())
		{
			this->output_wsa_error(error_type::read);
			return false;
		}

		/* auth_buffer */
		auth_buffer auth_info;
		auth_info.type = type;
		auth_info.version.major = version_info.major;
		auth_info.version.minor = version_info.minor;

		if (!this->write_fixed(desc_info(this->sock(), this->aes), reinterpret_cast<unsigned char*>(&auth_info), sizeof(auth_buffer), true))
		{
			this->output_wsa_error(error_type::write);
			return false;
		}

		if (!this->check_status())
			return false;

		/* buffer data */
		if (!this->write_fixed(desc_info(this->sock(), this->aes), reinterpret_cast<unsigned char*>(data), length, true))
		{
			this->output_wsa_error(error_type::write);
			return false;
		}

		if (!this->check_status())
			return false;

		if (!finalize_handler(this))
			return false;

		return true;
#ifdef _PRODUCTION
		VMProtectEnd();
#endif
	}
#ifdef _PRODUCTION
#pragma optimize("g", on)
#endif
	
	bool vps_socket::read_packet(net_buffer& packet, bool decrypt)
	{
		return this->net_socket_sync::read_packet(desc_info(this->sock(), this->aes), packet, decrypt);
	}

	bool vps_socket::exchange_keys()
	{
		memcpy(this->aes.key, aes_hard_key, sizeof(aes_key));
		memcpy(this->aes.iv, aes_hard_iv, sizeof(aes_iv));

		return this->receive_key(desc_info(this->sock(), this->aes), this->aes);
	}

	bool vps_socket::check_status()
	{
		unsigned char status[status_size];
		
		if (!this->read_fixed(desc_info(this->sock(), this->aes), status, status_size, true))
			return false;

		if (memcmp(status, status_failure, status_size) == 0)
		{
			unsigned char failure_reason[failure_size];
			
			if (!this->read_fixed(desc_info(this->sock(), this->aes), failure_reason, failure_size, true))
				return false;
			
			if (!memcmp(failure_reason, failure_connection, failure_size))
			{
				MessageBox(NULL, "Server denied request. Reason: Connection failure.", 
					"Client error!", MB_OK | MB_ICONERROR | MB_TOPMOST | MB_SETFOREGROUND);
			}
			else if (!memcmp(failure_reason, failure_file_version, failure_size))
			{
				MessageBox(NULL, "Server denied request. Reason: Wrong file version.\nNotice: A new version is probably available.",
					"Networking error", MB_OK | MB_ICONERROR | MB_TOPMOST | MB_SETFOREGROUND);
			}
			else if (!memcmp(failure_reason, failure_authentication, failure_size))
			{
				MessageBox(NULL, "Server denied request. Reason: Authentication failure.",
					"Networking error", MB_OK | MB_ICONERROR | MB_TOPMOST | MB_SETFOREGROUND);
			}
			else if (!memcmp(failure_reason, failure_no_active_licenses, failure_size))
			{
				MessageBox(NULL, "Server denied request. Reason: You have no active license(s).",
					"Networking error", MB_OK | MB_ICONERROR | MB_TOPMOST | MB_SETFOREGROUND);
			}
			else
			{
				MessageBox(NULL, "Server denied request. Reason: Unknown.",
					"Networking error", MB_OK | MB_ICONERROR | MB_TOPMOST | MB_SETFOREGROUND);
			}
			
			return false;
		}
		else if (memcmp(status, status_success, status_size) != 0)
			return false;

		return true;
	}
	
	void vps_socket::output_wsa_error(error_type type)
	{
		char error_message[1024];
		memset(error_message, 0, sizeof(error_message));

		switch (type)
		{
		case error_type::connect:
			strcpy(error_message, "Failed to connect to the server.");
			break;
			
		case error_type::read:
			strcpy(error_message, "Failed to read data from the server.");
			break;
			
		case error_type::write:
			strcpy(error_message, "Failed to write data to the server.");
			break;
		}

		strcat(error_message, "\nReason: ");
		
		int error_code = WSAGetLastError();

		switch (error_code)
		{
		case WSAECONNRESET:
			strcat(error_message, "You've been disconnected; the server terminated the connection.");
			break;

		case WSAETIMEDOUT:
			strcat(error_message, "You've timed out; it took you more than 10 seconds to communicate with the server.");
			break;

		default:
			strcat(error_message, "Unknown. Error code: ");
			strcat(error_message, std::to_string(error_code).c_str());
			strcat(error_message, ".");
			break;
		}
		
		MessageBox(NULL, error_message, "Networking error", MB_OK | MB_ICONERROR | MB_TOPMOST | MB_SETFOREGROUND);
	}
}