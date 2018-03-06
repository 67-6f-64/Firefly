#pragma once

#include "native.hpp"

#include <string>
#include <vector>

namespace firefly
{
	class net_buffer
	{
	public:
		net_buffer();
		net_buffer(unsigned char* data, std::size_t size);
		net_buffer(unsigned short header);
		virtual ~net_buffer();

		unsigned char write1(unsigned char data);
		unsigned short write2(unsigned short data);
		unsigned int write4(unsigned int data);
		unsigned int write8(unsigned __int64 data);
		void write_buffer(unsigned char* data, std::size_t size);
		void write_string(std::string const& data);
		void write_zero(std::size_t length);

		unsigned char read1();
		unsigned short read2();
		unsigned int read4();
		unsigned __int64 read8();
		bool read_buffer(unsigned char* buffer, std::size_t length);
		std::string read_string();

		template<typename T>
		T read_string_as();

		bool indent(std::size_t length);

		unsigned char* get_data();
		unsigned short get_header();
		unsigned int get_size();

	private:
		template <typename T>
		T write(T data);

		template <typename T>
		T read();

		bool check_index(std::size_t size);
		
		std::size_t index;
		std::vector<unsigned char> data;
	};

	template <typename T>
	T net_buffer::write(T data)
	{
		unsigned char* temp_data = reinterpret_cast<unsigned char*>(&data);
		std::copy(temp_data, temp_data + sizeof(T), std::back_inserter(this->data));

		return data;
	}

	template <typename T>
	T net_buffer::read()
	{
		if (this->check_index(sizeof(T)))
		{
			this->index += sizeof(T);
			return *reinterpret_cast<T*>(this->data.data() + this->index - sizeof(T));
		}

		return static_cast<T>(0);
	}

	template<typename T>
	T net_buffer::read_string_as()
	{
		unsigned short length = this->read<unsigned short>();

		if (this->check_index(length))
		{
			std::string buffer(reinterpret_cast<char*>(this->data.data() + this->index), length);
			this->index += length;

			return static_cast<T>(atoi(buffer.c_str()));
		}

		return static_cast<T>(0);
	}
}