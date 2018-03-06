#include "net_socket_sync.hpp"

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
	net_socket_sync::net_socket_sync()
		: net_socket()
	{

	}

	net_socket_sync::~net_socket_sync()
	{

	}

	bool net_socket_sync::initialize_socket(bool server)
	{
		return this->initialize_net_socket(server);
	}
	
	bool net_socket_sync::uninitialize_socket()
	{
		return this->uninitialize_net_socket();
	}

	bool net_socket_sync::receive_key(desc_info& desc, aes_buffer& aes)
	{
		return this->read_fixed(desc, reinterpret_cast<unsigned char*>(&aes), sizeof(aes_buffer), true);
	}

	bool net_socket_sync::read_packet(desc_info& desc, net_buffer& packet, bool decrypt)
	{
		unsigned int data_length = 0;

		if (!this->read_raw(desc, reinterpret_cast<unsigned char*>(&data_length), sizeof(unsigned int), decrypt))
			return false;

		unsigned char* data_buffer = new unsigned char[data_length];
		
		if (!this->read_raw(desc, data_buffer, data_length, decrypt))
		{
			delete[] data_buffer;
			return false;
		}

		packet = net_buffer(data_buffer, data_length);

		delete[] data_buffer;
		return true;
	}
	
	bool net_socket_sync::write_packet(desc_info& desc, net_buffer& packet, bool encrypt)
	{
		unsigned int data_length = packet.get_size();

		if (!this->write_raw(desc, reinterpret_cast<unsigned char*>(&data_length), sizeof(unsigned int), encrypt))
			return false;

		return this->write_raw(desc, packet.get_data(), packet.get_size(), encrypt);
	}
	
	bool net_socket_sync::read_fixed(desc_info& desc, unsigned char* buffer, std::size_t assumed_size, bool decrypt)
	{
		unsigned int data_length = 0;

		if (!this->read_raw(desc, reinterpret_cast<unsigned char*>(&data_length), sizeof(unsigned int), decrypt))
			return false;

		if (assumed_size && data_length != assumed_size)
			return false;

		return this->read_raw(desc, buffer, data_length, decrypt);
	}

	bool net_socket_sync::write_fixed(desc_info& desc, unsigned char* buffer, std::size_t size, bool encrypt)
	{
		unsigned int data_length = static_cast<unsigned int>(size);

		if (!this->write_raw(desc, reinterpret_cast<unsigned char*>(&data_length), sizeof(unsigned int), encrypt))
			return false;

		return this->write_raw(desc, buffer, data_length, encrypt);
	}

	bool net_socket_sync::read_raw(desc_info& desc, unsigned char* buffer, std::size_t size, bool decrypt)
	{
		for (int data_read = 0, offset = 0, data_to_read = size; data_to_read > 0; data_to_read -= data_read, offset += data_read)
		{
			data_read = recv(desc.desc(), reinterpret_cast<char*>(buffer) + offset, data_to_read, 0);

			if (data_read == 0 || data_read == SOCKET_ERROR)
				return false;
		}
		
		if (decrypt)
		{
			CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption decryptor(desc.aes().key, sizeof(desc.aes().key), desc.aes().iv);
			decryptor.ProcessData(buffer, buffer, size);
		}
		
		return true;
	}

	bool net_socket_sync::write_raw(desc_info& desc, unsigned char const* buffer, std::size_t size, bool encrypt)
	{
		unsigned char* data_buffer = new unsigned char[size];

		if (encrypt)
		{
			CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption encryptor(desc.aes().key, sizeof(desc.aes().key), desc.aes().iv);
			encryptor.ProcessData(data_buffer, buffer, size);
		}
		else
		{
			memcpy(data_buffer, buffer, size);
		}

		for (int data_sent = 0, offset = 0, data_to_send = size; data_to_send > 0; data_to_send -= data_sent, offset += data_sent)
		{
			data_sent = send(desc.desc(), reinterpret_cast<const char*>(data_buffer) + offset, data_to_send, 0);

			if (data_sent == 0 || data_sent == SOCKET_ERROR)
			{
				delete[] data_buffer;
				return false;
			}
		}
		
		delete[] data_buffer;
		return true;
	}
}