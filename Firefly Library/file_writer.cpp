#include "file_writer.hpp"

namespace firefly
{
	namespace io
	{
		file_writer::file_writer()
		{

		}

		file_writer::~file_writer()
		{

		}

		void file_writer::set_path(std::string const& file_path)
		{
			this->file_path = file_path;
		}

		bool file_writer::write_int(std::string const& section, std::string const& key, unsigned int value)
		{
			return this->write_string(section, key, std::to_string(value));
		}

		bool file_writer::write_string(std::string const& section, std::string const& key, std::string const& string)
		{
			return (WritePrivateProfileString(section.c_str(), key.c_str(), string.c_str(), this->file_path.c_str()) != FALSE);
		}

		bool file_writer::write_section(std::string const& section, std::vector<std::pair<std::string, std::string>> strings)
		{
			std::size_t size = 1;

			for (auto x : strings)
				size += (x.first.length() + sizeof('=') + x.second.length() + sizeof('\0'));

			char* section_string = new char[size];
			unsigned int section_offset = 0;

			for (auto x : strings)
			{
				memcpy(section_string + section_offset, x.first.c_str(), x.first.length());
				section_offset += x.first.length();

				*(section_string + section_offset) = '=';
				section_offset += sizeof('=');

				memcpy(section_string + section_offset, x.second.c_str(), x.second.length());
				section_offset += x.second.length();

				*(section_string + section_offset) = '\0';
				section_offset += sizeof('\0');
			}

			*(section_string + section_offset) = '\0';

			BOOL ret = WritePrivateProfileSection(section.c_str(), section_string, this->file_path.c_str());
			delete[] section_string;

			return (ret != FALSE);
		}
	}
}