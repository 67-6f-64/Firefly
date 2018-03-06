#pragma once

#include "native.hpp"

#include <string>
#include <vector>

namespace firefly
{
	namespace io
	{
		class file_writer
		{
		public:
			file_writer();
			~file_writer();

			void set_path(std::string const& file_path);

			bool write_int(std::string const& section, std::string const& key, unsigned int value);
			bool write_string(std::string const& section, std::string const& key, std::string const& string);
			bool write_section(std::string const& section, std::vector<std::pair<std::string, std::string>> strings);

		private:
			std::string file_path;
		};
	}
}