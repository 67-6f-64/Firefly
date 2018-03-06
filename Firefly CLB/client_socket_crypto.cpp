#include "client_socket_crypto.hpp"
#include "config.hpp"

#include "native.hpp"

namespace firefly
{
	const unsigned char client_socket_crypto::shuffle[0x100] =
	{
		0xEC, 0x3F, 0x77, 0xA4, 0x45, 0xD0, 0x71, 0xBF, 0xB7, 0x98, 0x20, 0xFC, 0x4B, 0xE9, 0xB3, 0xE1,
		0x5C, 0x22, 0xF7, 0x0C, 0x44, 0x1B, 0x81, 0xBD, 0x63, 0x8D, 0xD4, 0xC3, 0xF2, 0x10, 0x19, 0xE0,
		0xFB, 0xA1, 0x6E, 0x66, 0xEA, 0xAE, 0xD6, 0xCE, 0x06, 0x18, 0x4E, 0xEB, 0x78, 0x95, 0xDB, 0xBA,
		0xB6, 0x42, 0x7A, 0x2A, 0x83, 0x0B, 0x54, 0x67, 0x6D, 0xE8, 0x65, 0xE7, 0x2F, 0x07, 0xF3, 0xAA,
		0x27, 0x7B, 0x85, 0xB0, 0x26, 0xFD, 0x8B, 0xA9, 0xFA, 0xBE, 0xA8, 0xD7, 0xCB, 0xCC, 0x92, 0xDA,
		0xF9, 0x93, 0x60, 0x2D, 0xDD, 0xD2, 0xA2, 0x9B, 0x39, 0x5F, 0x82, 0x21, 0x4C, 0x69, 0xF8, 0x31,
		0x87, 0xEE, 0x8E, 0xAD, 0x8C, 0x6A, 0xBC, 0xB5, 0x6B, 0x59, 0x13, 0xF1, 0x04, 0x00, 0xF6, 0x5A,
		0x35, 0x79, 0x48, 0x8F, 0x15, 0xCD, 0x97, 0x57, 0x12, 0x3E, 0x37, 0xFF, 0x9D, 0x4F, 0x51, 0xF5,
		0xA3, 0x70, 0xBB, 0x14, 0x75, 0xC2, 0xB8, 0x72, 0xC0, 0xED, 0x7D, 0x68, 0xC9, 0x2E, 0x0D, 0x62,
		0x46, 0x17, 0x11, 0x4D, 0x6C, 0xC4, 0x7E, 0x53, 0xC1, 0x25, 0xC7, 0x9A, 0x1C, 0x88, 0x58, 0x2C,
		0x89, 0xDC, 0x02, 0x64, 0x40, 0x01, 0x5D, 0x38, 0xA5, 0xE2, 0xAF, 0x55, 0xD5, 0xEF, 0x1A, 0x7C,
		0xA7, 0x5B, 0xA6, 0x6F, 0x86, 0x9F, 0x73, 0xE6, 0x0A, 0xDE, 0x2B, 0x99, 0x4A, 0x47, 0x9C, 0xDF,
		0x09, 0x76, 0x9E, 0x30, 0x0E, 0xE4, 0xB2, 0x94, 0xA0, 0x3B, 0x34, 0x1D, 0x28, 0x0F, 0x36, 0xE3,
		0x23, 0xB4, 0x03, 0xD8, 0x90, 0xC8, 0x3C, 0xFE, 0x5E, 0x32, 0x24, 0x50, 0x1F, 0x3A, 0x43, 0x8A,
		0x96, 0x41, 0x74, 0xAC, 0x52, 0x33, 0xF0, 0xD9, 0x29, 0x80, 0xB1, 0x16, 0xD3, 0xAB, 0x91, 0xB9,
		0x84, 0x7F, 0x61, 0x1E, 0xCF, 0xC5, 0xD1, 0x56, 0x3D, 0xCA, 0xF4, 0x05, 0xC6, 0xE5, 0x08, 0x49
	};
	
	bool client_socket_crypto::initialize()
	{
		unsigned char* module = reinterpret_cast<unsigned char*>(LoadLibrary("G:\\Games\\MapleStory (Global)\\maplestory\\appdata\\ZLZ.dll"));

		if (!module)
			return false;
				
		typedef void (__cdecl* ZLZ__SetKey_t)();
		reinterpret_cast<ZLZ__SetKey_t>(module + 0x3310)();

		unsigned char aes_key[32];
		memset(aes_key, 0, sizeof(aes_key));

		for (int i = 0, j = 0; i < (16 * 8); i += (4 * 4), j += 4)
			aes_key[j] = *reinterpret_cast<unsigned char*>(module + 0x14028 + i);

		return this->aes_crypto.initialize(aes_key);
	}

	/* CAESCipher::Encrypt & CAESCipher::Decrypt compilation */
	void client_socket_crypto::aes_crypt(unsigned char* data, unsigned int length, unsigned int& iv)
	{
		unsigned char morph_key[16];
		unsigned char* data_pointer = data;

		unsigned int size = 0x5B0;

		while (length)
		{
			unsigned char* iv_pointer = reinterpret_cast<unsigned char*>(&iv);

			for (int i = 0; i < 16; i++)
				morph_key[i] = iv_pointer[i % 4];

			if (length < size)
				size = length;

			for (unsigned int offset = 0; offset < size; offset++)
			{
				if ((offset % 16) == 0)
					this->aes_crypto.transform_block(morph_key, 16, morph_key);

				data_pointer[offset] ^= morph_key[offset % 16];
			}

			data_pointer += size;
			length -= size;

			size = 0x5B4;
		}

		iv = shuffle_iv(iv);
	}

	/* CIOBufferManipulator::_En (Shanda encryption) */
	void client_socket_crypto::encode_data(unsigned char* data, unsigned int length)
	{
		unsigned char a = 0;
		unsigned char b = 0;

		for (unsigned int i = 0; i < 3; i++)
		{
			b = 0;

			for (unsigned int j = length; j > 0; j--)
			{
				a = data[length - j];
				a = _rotl8(a, 3);
				a = static_cast<unsigned char>(a + j);
				a ^= b;
				b = a;
				a = _rotr8(a, j);
				a = ~a;											/* ~ (bitwise not) is the same as ^ (xor) 0xFF */
				a += 0x48;
				data[length - j] = a;
			}

			b = 0;

			for (unsigned int j = length; j > 0; j--)
			{
				a = data[j - 1];
				a = _rotl8(a, 4);
				a = static_cast<unsigned char>(a + j);
				a ^= b;
				b = a;
				a ^= 0x13;
				a = _rotr8(a, 3);
				data[j - 1] = a;
			}
		}
	}

	/* CIOBufferManipulator::_De (Shanda decryption) */
	void client_socket_crypto::decode_data(unsigned char* data, unsigned int length)
	{
		unsigned char a = 0;
		unsigned char b = 0;
		unsigned char c = 0;

		for (unsigned int i = 0; i < 3; i++)
		{
			b = 0;
			c = 0;

			for (unsigned int j = length; j > 0; j--)
			{
				a = data[j - 1];
				a = _rotl8(a, 3);
				a ^= 0x13;
				b = a;
				a ^= c;
				a = static_cast<unsigned char>(a - j);
				a = _rotr8(a, 4);
				c = b;
				data[j - 1] = a;
			}

			b = 0;
			c = 0;

			for (unsigned int j = length; j > 0; j--)
			{
				a = data[length - j];
				a -= 0x48;
				a = ~a;											/* ~ (bitwise not) is the same as ^ (xor) 0xFF */
				a = _rotl8(a, j);
				b = a;
				a ^= c;
				a = static_cast<unsigned char>(a - j);
				a = _rotr8(a, 3);
				c = b;
				data[length - j] = a;
			}
		}
	}

	void client_socket_crypto::create_header(unsigned char* data, unsigned int length, unsigned short version_major, unsigned int iv)
	{
		unsigned char* temp_iv = reinterpret_cast<unsigned char*>(&iv);

		unsigned int a = (temp_iv[3] * 0x100 + temp_iv[2]) ^ version_major;
		unsigned int b = a ^ length;

		data[0] = static_cast<unsigned char>(a);
		data[1] = static_cast<unsigned char>(a >> 8);
		data[2] = static_cast<unsigned char>(b);
		data[3] = static_cast<unsigned char>(b >> 8);
	}

	unsigned int client_socket_crypto::get_packet_length(unsigned char* data)
	{
		return (data[0] + (data[1] << 8)) ^ (data[2] + (data[3] << 8));
	}

	client_socket_crypto::client_socket_crypto()
	{

	}

	client_socket_crypto::~client_socket_crypto()
	{

	}

	/* CIGCipher::innoHash */
	unsigned int client_socket_crypto::shuffle_iv(unsigned int iv)
	{
		unsigned int default_key = 0xC65053F2;

		unsigned char* new_iv_pointer = reinterpret_cast<unsigned char*>(&default_key);
		unsigned char* old_iv_pointer = reinterpret_cast<unsigned char*>(&iv);

		for (int i = 0; i < 4; i++)
		{
			new_iv_pointer[0] += client_socket_crypto::shuffle[new_iv_pointer[1]] - old_iv_pointer[i];
			new_iv_pointer[1] -= client_socket_crypto::shuffle[old_iv_pointer[i]] ^ new_iv_pointer[2];
			new_iv_pointer[2] ^= client_socket_crypto::shuffle[new_iv_pointer[3]] + old_iv_pointer[i];
			new_iv_pointer[3] += client_socket_crypto::shuffle[old_iv_pointer[i]] - new_iv_pointer[0];

			default_key = (default_key >> 0x1D) | (default_key << 0x03);
		}

		return default_key;
	}
}