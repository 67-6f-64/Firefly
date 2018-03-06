#include "net_buffer.hpp"

namespace firefly
{
	net_buffer::net_buffer()
		: index(0)
	{

	}

	net_buffer::net_buffer(unsigned char* data, std::size_t size)
		: index(0), data(data, data + size)
	{

	}

	net_buffer::net_buffer(unsigned short header)
	{
		this->write<unsigned short>(header);
	}

	net_buffer::~net_buffer()
	{
		data.clear();
	}

	unsigned char net_buffer::write1(unsigned char data)
	{
		return this->write<unsigned char>(data);
	}

	unsigned short net_buffer::write2(unsigned short data)
	{
		return this->write<unsigned short>(data);
	}

	unsigned int net_buffer::write4(unsigned int data)
	{
		return this->write<unsigned int>(data);
	}

	void net_buffer::write_buffer(unsigned char* data, std::size_t size)
	{
		std::copy(data, data + size, std::back_inserter(this->data));
	}

	void net_buffer::write_string(std::string const& data)
	{
		this->write<unsigned short>(data.length() & 0xFFFF);
		std::copy(data.begin(), data.end(), std::back_inserter(this->data));
	}

	void net_buffer::write_zero(std::size_t length)
	{
		for (std::size_t i = 0; i < length; i++)
			this->data.push_back(0);
	}

	unsigned char net_buffer::read1()
	{
		return this->read<unsigned char>();
	}

	unsigned short net_buffer::read2()
	{
		return this->read<unsigned short>();
	}

	unsigned int net_buffer::read4()
	{
		return this->read<unsigned int>();
	}

	bool net_buffer::read_buffer(unsigned char* buffer, std::size_t length)
	{
		if (!this->check_index(length))
			return false;

		memcpy(buffer, this->data.data() + this->index, length);
		this->index += length;
		return true;
	}

	std::string net_buffer::read_string()
	{
		unsigned short length = this->read<unsigned short>();

		if (!this->check_index(length))
			return std::string();

		this->index += length;
		return std::string(reinterpret_cast<char*>(this->data.data() + this->index - length), length);
	}

	bool net_buffer::indent(std::size_t length)
	{
		if (!this->check_index(length))
			return false;

		this->index += length;
		return true;
	}

	unsigned char* net_buffer::get_data()
	{
		return this->data.data();
	}

	unsigned short net_buffer::get_header()
	{
		return *reinterpret_cast<unsigned short*>(this->data.data());
	}

	unsigned int net_buffer::get_size()
	{
		return this->data.size();
	}
	
	std::vector<unsigned char> net_buffer::get_vector()
	{
		return this->data;
	}

	bool net_buffer::check_index(std::size_t size)
	{
		return ((this->index + size) <= static_cast<std::size_t>(this->data.size()));
	}
}