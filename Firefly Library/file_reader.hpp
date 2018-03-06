#pragma once

#include "native.hpp"

#include <string>
#include <vector>

namespace firefly
{
	namespace io
	{
		class file_reader
		{
		public:
			file_reader();
			~file_reader();

			void set_path(std::string const& file_path);

			unsigned int get_int(std::string const& section, std::string const& key, int default_value);
			std::string get_string(std::string const& section, std::string const& key, std::string const& default_string);
			void get_section_strings(std::string const& section, std::vector<std::pair<std::string, std::string>>& strings);

		private:
			std::string file_path;
		};
	}
}