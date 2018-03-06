#pragma once

#include "native.hpp"

#include <exception>

namespace firefly
{
#pragma pack(push, 1)
	template <typename T, bool shortorbyte>
	class ZtlSecureImpl
	{
	protected:
		T Fuse(unsigned int authenticity)
		{
			unsigned int checksum = 0xBAADF00D;
			unsigned char value[sizeof(T)] = { 0 };

			for (std::size_t i = 0; i < sizeof(T); i++)
			{
				value[i] = (this->key[i] ^ this->encrypted_data[i]);
				checksum = this->encrypted_data[i] + _rotr(this->key[i] ^ checksum, 5);
			}

			if (checksum != authenticity)
				throw std::exception("ZtlSecure<>::Fuse() failed.");

			return *reinterpret_cast<T*>(&value[0]);
		}

		unsigned int Tear(T value)
		{
			unsigned int checksum = 0xBAADF00D;
			unsigned char* p = reinterpret_cast<unsigned char*>(&value);

			for (std::size_t i = 0; i < sizeof(T); i++)
			{
				this->key[i] = (rand() & 0xFF);
				this->encrypted_data[i] = (p[i] ^ this->key[i]);

				checksum = this->encrypted_data[i] + _rotr(this->key[i] ^ checksum, 5);
			}

			return checksum;
		}

	private:
		unsigned char key[sizeof(T)];
		unsigned char encrypted_data[sizeof(T)];
	};

	template <typename T>
	class ZtlSecureImpl<T, false>
	{
	protected:
		T Fuse(unsigned int authenticity)
		{
			unsigned int checksum = 0xBAADF00D;
			unsigned int value[sizeof(T) / sizeof(unsigned int)] = { 0 };

			for (std::size_t i = 0; i < (sizeof(T) / sizeof(unsigned int)); i++)
			{
				value[i] = (this->key[i] ^ _rotl(this->encrypted_data[i], 5));
				checksum = this->encrypted_data[i] + _rotr(this->key[i] ^ checksum, 5);
			}

			if (checksum != authenticity)
				throw std::exception("ZtlSecure<>::Fuse() failed.");

			return *reinterpret_cast<T*>(&value[0]);
		}
		
		unsigned int Tear(T value)
		{
			unsigned int checksum = 0xBAADF00D;
			unsigned char* p = reinterpret_cast<unsigned int*>(&value);

			for (std::size_t i = 0; i < (sizeof(T) / sizeof(unsigned int)); i++)
			{
				this->key[i] = rand();
				this->encrypted_data[i] = _rotr(p[i] ^ this->key[i], 5);
				checksum = this->encrypted_data[i] + _rotr(this->key[i] ^ checksum, 5);
			}

			return checksum;
		}

	private:
		unsigned int key[sizeof(T) / sizeof(unsigned int)];
		unsigned int encrypted_data[sizeof(T) / sizeof(unsigned int)];
	};

	template <class T>
	class ZtlSecurePack : public ZtlSecureImpl<T, sizeof(T) <= sizeof(unsigned short)> 
	{ 
		~ZtlSecurePack() = delete;

	public:
		T Fuse()
		{
			return this->ZtlSecureImpl::Fuse(this->checksum);
		}

		unsigned int Tear()
		{
			return (this->checksum = this->ZtlSecureImpl::Tear());
		}

	private:
		unsigned int checksum;
	};
#pragma pack(pop)

	template <class T>
	class ZtlSecure : public ZtlSecureImpl<T, sizeof(T) <= sizeof(unsigned short)> 
	{ 
		~ZtlSecure() = delete;

	public:
		T Fuse()
		{
			return this->ZtlSecureImpl::Fuse(this->checksum);
		}

		unsigned int Tear()
		{
			return (this->checksum = this->ZtlSecureImpl::Tear());
		}

	private:
		unsigned int checksum;
	};
	
	static_assert_size(sizeof(ZtlSecurePack<bool>), 0x06);
	static_assert_size(sizeof(ZtlSecurePack<char>), 0x06);
	static_assert_size(sizeof(ZtlSecurePack<short>), 0x08);
	static_assert_size(sizeof(ZtlSecurePack<int>), 0x0C); 

	static_assert_size(sizeof(ZtlSecure<bool>), 0x08);
	static_assert_size(sizeof(ZtlSecure<char>), 0x08);
	static_assert_size(sizeof(ZtlSecure<short>), 0x08);
	static_assert_size(sizeof(ZtlSecure<int>), 0x0C); 
}