#include "net_socket_async.hpp"
#include "net_opcodes.hpp"

#include <cryptopp\osrng.h>
#include <cryptopp\aes.h>
#include <cryptopp\modes.h>
#include <cryptopp\filters.h>

#ifdef _DEBUG
#pragma comment(lib, "cryptopp\\cryptlib_debug_dll")
#else
#pragma comment(lib, "cryptopp\\cryptlib_release_dll")
#endif

namespace firefly
{
	LRESULT CALLBACK net_socket_async::window_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		net_callback* object = nullptr;

		if (uMsg == WM_NCCREATE)
		{
			CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lParam);
			object = static_cast<net_callback*>(create_struct->lpCreateParams);

			SetWindowLong(hWnd, GWLP_USERDATA, reinterpret_cast<LONG>(object));
		}
		else
		{
			object = reinterpret_cast<net_callback*>(GetWindowLong(hWnd, GWLP_USERDATA));
		}

		if (object && uMsg == WM_GAME_SOCKET)
			object->on_socket_message(static_cast<SOCKET>(wParam), WSAGETSELECTEVENT(lParam), WSAGETSELECTERROR(lParam));

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	
	net_socket_async::net_socket_async()
		: net_socket()
	{
		this->hwnd_ = NULL;
		
		this->read_buffer.clear();
		this->write_buffer.clear();
	}

	net_socket_async::~net_socket_async()
	{
		if (this->hwnd_)
			DestroyWindow(this->hwnd_);
	}

	bool net_socket_async::create_callback_window(HINSTANCE instance, net_callback* window)
	{
		WNDCLASSEXA wcex;
		memset(&wcex, 0, sizeof(WNDCLASSEXA));

		wcex.cbSize = sizeof(WNDCLASSEXA);
		wcex.lpfnWndProc = net_socket_async::window_proc;
		wcex.hInstance = instance;
		wcex.lpszClassName = "firefly_network_callback_class";

		if (!RegisterClassExA(&wcex))
			return false;

		return ((this->hwnd_ = CreateWindowA("firefly_network_callback_class", NULL, NULL, 0, 0, 0, 0,
			HWND_MESSAGE, NULL, instance, reinterpret_cast<void*>(window))) != NULL);
	}

	bool net_socket_async::initialize_socket(bool server, HWND hwnd, unsigned int message_id)
	{
		if (!this->initialize_net_socket(server))
			return false;

		return (WSAAsyncSelect(this->sock(), (hwnd ? hwnd : this->hwnd_), message_id, 
			FD_ACCEPT | FD_CONNECT | FD_CLOSE | FD_READ | FD_WRITE) != SOCKET_ERROR);
	}
	
	bool net_socket_async::uninitialize_socket()
	{
		return this->uninitialize_net_socket();
	}
	
	bool net_socket_async::generate_key(desc_info& desc, aes_buffer& aes)
	{
		CryptoPP::AutoSeededRandomPool prng;
		prng.GenerateBlock(aes.key, sizeof(aes.key));
		prng.GenerateBlock(aes.iv, sizeof(aes.iv));
		
		net_buffer key_packet(aes_init_key);
		key_packet.write_buffer(reinterpret_cast<unsigned char*>(&aes), sizeof(aes));

		return this->write_packet(desc, key_packet);
	}
	
	bool net_socket_async::read_packet(desc_info& desc, net_buffer& packet, bool decrypt)
	{
		if (this->read_buffer.size() < sizeof(unsigned int))
			return false;

		unsigned int packet_data_length = *reinterpret_cast<unsigned int*>(&this->read_buffer[0]);
		unsigned int packet_full_length = sizeof(unsigned int) + packet_data_length;

		if (this->read_buffer.size() < packet_full_length)
			return false;
			
		if (decrypt)
		{
			CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption decryptor(desc.aes().key, sizeof(desc.aes().key), desc.aes().iv);
			decryptor.ProcessData(&this->read_buffer[4], &this->read_buffer[4], packet_data_length);
		}

		packet = net_buffer(&this->read_buffer[4], packet_data_length);
		this->read_buffer.erase(this->read_buffer.begin(), this->read_buffer.begin() + packet_full_length);
		return true;
	}
	
	bool net_socket_async::read_collect(desc_info& desc)
	{
		for (int bytes_read = 0;;)
		{
			std::vector<unsigned char> temp_buffer(4096);
			bytes_read = recv(desc.desc(), reinterpret_cast<char*>(&temp_buffer[0]), temp_buffer.size(), 0);

			if (bytes_read == 0)
				break;

			if (bytes_read == SOCKET_ERROR)
			{
				if (WSAGetLastError() != WSAEWOULDBLOCK)
					return false;

				break;
			}

			this->read_buffer.insert(this->read_buffer.end(), temp_buffer.begin(), temp_buffer.begin() + bytes_read);
		}

		return true;
	}

	bool net_socket_async::write_packet(desc_info& desc, net_buffer& packet, bool encrypt)
	{
		std::vector<unsigned char> packet_data = packet.get_vector();

		unsigned int packet_size = packet_data.size();
		unsigned char* packet_size_data = reinterpret_cast<unsigned char*>(&packet_size);

		std::copy(packet_size_data, packet_size_data + sizeof(unsigned int), std::back_inserter(this->write_buffer));
		
		if (encrypt)
		{
			CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption encryptor(desc.aes().key, sizeof(desc.aes().key), desc.aes().iv);
			encryptor.ProcessData(&packet_data[0], &packet_data[0], packet_data.size());
		}

		this->write_buffer.insert(this->write_buffer.end(), &packet_data[0], &packet_data[packet_data.size()]);

		return this->write_flush(desc);
	}

	bool net_socket_async::write_flush(desc_info& desc)
	{
		for (int bytes_sent = 0; this->write_buffer.size() > 0;)
		{
			bytes_sent = send(desc.desc(), reinterpret_cast<char const*>(&this->write_buffer[0]), this->write_buffer.size(), 0);

			if (bytes_sent == SOCKET_ERROR)
			{
				if (WSAGetLastError() != WSAEWOULDBLOCK)
					return false;
			
				break;
			}

			this->write_buffer.erase(this->write_buffer.begin(), this->write_buffer.begin() + bytes_sent);
		}

		return true;
	}
	
	void net_socket_async::reset_buffers()
	{
		this->read_buffer.clear();
		this->write_buffer.clear();
	}
}