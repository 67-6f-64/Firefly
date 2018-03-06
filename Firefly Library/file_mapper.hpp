#pragma once

#include "native.hpp"
#include "pe_structs.hpp"

#include "aes_constants.hpp"

#include <string>
#include <vector>

namespace firefly
{
	namespace io
	{
		class file_mapper
		{
			void init();

		public:
			file_mapper(file_mapper const& file_map);
			file_mapper(std::string const& file_name);
			file_mapper(std::vector<unsigned char>& file_data);
			file_mapper(unsigned char* file_data, std::size_t file_size);
			~file_mapper();

			bool decrypt();
			bool encrypt();

			unsigned char* data() const;
			std::size_t size() const;

			void read_file(std::string const& file_name);
			void write_file(std::string const& file_name);

		private:
			bool update_dos_header(bool decrypt);
			bool unpack_dos_header(dos_header* header);

			bool update_pe_header(bool decrypt);
			bool unpack_pe_header(pe_header* header);

			bool update_optional_header(bool decrypt);
			bool unpack_optional_header(optional_header* header);

			bool update_section_headers(bool decrypt);
			bool unpack_section_headers(section_header* header);

			bool update_exports(bool decrypt);
			bool unpack_exports(export_directory* directory);

			bool update_imports(bool decrypt);
			bool unpack_imports(import_descriptor* descriptor, bool decrypt);

			bool update_sections(bool decrypt);

			unsigned int rva_to_offset(unsigned int rva);

			void crypt_buffer(unsigned char* buffer, std::size_t size, bool decrypt);

		private:
			std::vector<unsigned char> file_data;

			aes_buffer aes;
			std::size_t offset;

			std::size_t section_count;
			std::vector<section_header> sections;

			std::pair<unsigned int, unsigned int> export_infos;
			std::pair<unsigned int, unsigned int> import_infos;
		};
	}
}