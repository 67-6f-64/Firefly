#include "file_reader.hpp"

namespace firefly
{
	namespace io
	{
		file_reader::file_reader()
		{

		}

		file_reader::~file_reader()
		{

		}

		void file_reader::set_path(std::string const& file_path)
		{
			this->file_path = file_path;
		}

		unsigned int file_reader::get_int(std::string const& section, std::string const& key, int default_value)
		{
			return GetPrivateProfileInt(section.c_str(), key.c_str(), default_value, this->file_path.c_str());
		}

		std::string file_reader::get_string(std::string const& section, std::string const& key, std::string const& default_string)
		{
			char returned_string[1024];
			memset(returned_string, 0, sizeof(returned_string));

			GetPrivateProfileString(section.c_str(), key.c_str(), default_string.c_str(), returned_string, sizeof(returned_string), this->file_path.c_str());

			return std::string(returned_string);
		}

		void file_reader::get_section_strings(std::string const& section, std::vector<std::pair<std::string, std::string>>& strings)
		{
			char returned_strings[8192];
			memset(returned_strings, 0, sizeof(returned_strings));

			GetPrivateProfileSection(section.c_str(), returned_strings, sizeof(returned_strings), this->file_path.c_str());

			char* current_string = returned_strings;

			if (*current_string != '\0')
			{
				do
				{
					int split = strchr(current_string, '=') - current_string;

					std::string key(current_string, current_string + split);
					std::string value(current_string + split + 1);

					strings.push_back(std::make_pair(key, value));
				} while (current_string += strlen(current_string) + 1, *current_string != '\0');
			}
		}
	}
}