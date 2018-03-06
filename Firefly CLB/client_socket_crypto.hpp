#pragma once

#include "rijndael.hpp"

namespace firefly
{
	class client_socket_crypto
	{
		static const unsigned char shuffle[256];

	public:
		static client_socket_crypto& get_instance()
		{
			static client_socket_crypto crypto_client;
			return crypto_client;
		}
		
		bool initialize();

		void aes_crypt(unsigned char* data, unsigned int length, unsigned int& iv);

		void encode_data(unsigned char* data, unsigned int length);
		void decode_data(unsigned char* data, unsigned int length);

		void create_header(unsigned char* data, unsigned int length, unsigned short version_major, unsigned int iv);
		unsigned int get_packet_length(unsigned char* data);

	private:
		client_socket_crypto();
		~client_socket_crypto();

		unsigned int shuffle_iv(unsigned int iv);

		rijndael aes_crypto;
	};
}