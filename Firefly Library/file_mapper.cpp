#include "file_mapper.hpp"

#include <fstream>

#include <cryptopp\osrng.h>
#include <cryptopp\aes.h>
#include <cryptopp\modes.h>
#include <cryptopp\filters.h>

#ifdef _DEBUG
#pragma comment(lib, "cryptopp\\cryptlib_debug_dll")
#else
#pragma comment(lib, "cryptopp\\cryptlib_release_dll")
#endif

namespace firefly
{
	namespace io
	{
		void file_mapper::init()
		{
			memcpy(this->aes.key, aes_hard_key, sizeof(aes_key));
			memcpy(this->aes.iv, aes_hard_iv, sizeof(aes_iv));

			this->offset = 0;
			this->section_count = 0;
		}
		
		file_mapper::file_mapper(file_mapper const& file_map)
			: file_data(file_map.data(), file_map.data() + file_map.size())
		{
			this->init();
		}

		file_mapper::file_mapper(std::string const& file_name)
		{
			this->init();
			this->read_file(file_name);
		}

		file_mapper::file_mapper(std::vector<unsigned char>& file_data)
			: file_data(file_data)
		{
			this->init();
		}

		file_mapper::file_mapper(unsigned char* file_data, std::size_t file_size)
			: file_data(file_data, file_data + file_size)
		{
			this->init();
		}


		file_mapper::~file_mapper()
		{

		}

		bool file_mapper::decrypt()
		{
			return (this->update_dos_header(true) &&
				this->update_pe_header(true) &&
				this->update_optional_header(true) &&
				this->update_section_headers(true) &&
				this->update_sections(true) &&
				this->update_exports(true) &&
				this->update_imports(true));
		}

		bool file_mapper::encrypt()
		{
			return (this->update_dos_header(false) &&
				this->update_pe_header(false) &&
				this->update_optional_header(false) &&
				this->update_section_headers(false) &&
				this->update_exports(true) &&
				this->update_imports(false) &&
				this->update_sections(false));
		}

		unsigned char* file_mapper::data() const
		{
			return const_cast<unsigned char*>(this->file_data.data());
		}

		std::size_t file_mapper::size() const
		{
			return this->file_data.size();
		}
		
		void file_mapper::read_file(std::string const& file_name)
		{
			std::ifstream f(file_name.c_str(), std::ios_base::binary | std::ios_base::ate);
			std::size_t size = static_cast<std::size_t>(f.tellg());

			unsigned char* buffer = new unsigned char[size];

			f.seekg(0, std::ios_base::beg);
			f.read(reinterpret_cast<char*>(buffer), size);

			std::copy(&buffer[0], &buffer[size], std::back_inserter(this->file_data));

			delete[] buffer;
			f.close();
		}

		void file_mapper::write_file(const std::string& file_path)
		{
			std::ofstream f(file_path.c_str(), std::ios_base::binary);
			f.write(reinterpret_cast<char*>(this->file_data.data()), this->file_data.size());
			f.close();
		}

		bool file_mapper::update_dos_header(bool decrypt)
		{
			dos_header* header = reinterpret_cast<dos_header*>(this->data());

			if (decrypt)
				this->crypt_buffer(reinterpret_cast<unsigned char*>(header), sizeof(dos_header), decrypt);

			if (!this->unpack_dos_header(header))
				return false;

			if (!decrypt)
				this->crypt_buffer(reinterpret_cast<unsigned char*>(header), sizeof(dos_header), decrypt);

			return true;
		}

		bool file_mapper::unpack_dos_header(dos_header* header)
		{
			const unsigned int dos_signature = 'M' | ('Z' << 8);

			if (header->magic != dos_signature || !header->pe_offset)
				return false;

			this->offset = header->pe_offset;
			return true;
		}

		bool file_mapper::update_pe_header(bool decrypt)
		{
			pe_header* header = reinterpret_cast<pe_header*>(this->data() + this->offset);

			if (decrypt)
				this->crypt_buffer(reinterpret_cast<unsigned char*>(header), sizeof(pe_header), decrypt);

			if (!this->unpack_pe_header(header))
				return false;

			if (!decrypt)
				this->crypt_buffer(reinterpret_cast<unsigned char*>(header), sizeof(pe_header), decrypt);

			return true;
		}

		bool file_mapper::unpack_pe_header(pe_header* header)
		{
			const unsigned int pe_signature = 'P' | ('E' << 8);

			if (header->signature != pe_signature)
				return false;

			if (!(header->characteristics & file_executable_image))
				return false;

			this->section_count = header->section_count;
			this->offset += sizeof(pe_header);
			return true;
		}

		bool file_mapper::update_optional_header(bool decrypt)
		{
			optional_header* header = reinterpret_cast<optional_header*>(this->data() + this->offset);

			if (decrypt)
				this->crypt_buffer(reinterpret_cast<unsigned char*>(header), sizeof(optional_header), decrypt);

			if (!this->unpack_optional_header(header))
				return false;

			if (!decrypt)
				this->crypt_buffer(reinterpret_cast<unsigned char*>(header), sizeof(optional_header), decrypt);

			return true;
		}

		bool file_mapper::unpack_optional_header(optional_header* header)
		{
			const unsigned int optional_signature_32 = 0x10B;

			if (header->magic != optional_signature_32)
				return false;

			data_directory export_directory = header->data_directory[directory_entry_export];
			this->export_infos = std::make_pair(export_directory.virtual_address, export_directory.size);

			data_directory import_directory = header->data_directory[directory_entry_import];
			this->import_infos = std::make_pair(import_directory.virtual_address, import_directory.size);

			this->offset += sizeof(optional_header);
			return true;
		}

		bool file_mapper::update_section_headers(bool decrypt)
		{
			section_header* header = reinterpret_cast<section_header*>(this->data() + this->offset);

			for (std::size_t i = 0; i < this->section_count; i++, header++, this->offset += sizeof(section_header))
			{
				if (decrypt)
					this->crypt_buffer(reinterpret_cast<unsigned char*>(header), sizeof(section_header), decrypt);

				if (!this->unpack_section_headers(header))
					return false;

				if (!decrypt)
					this->crypt_buffer(reinterpret_cast<unsigned char*>(header), sizeof(section_header), decrypt);
			}

			return true;
		}

		bool file_mapper::unpack_section_headers(section_header* header)
		{
			this->sections.push_back(*header);
			return true;
		}

		bool file_mapper::update_exports(bool decrypt)
		{
			if (!export_infos.first || !export_infos.second)
				return true;

			export_directory* directory = reinterpret_cast<export_directory*>(this->data() + this->rva_to_offset(this->export_infos.first));

			if (decrypt)
				this->crypt_buffer(reinterpret_cast<unsigned char*>(directory), sizeof(export_directory), decrypt);

			return this->unpack_exports(directory);

			if (!decrypt)
				this->crypt_buffer(reinterpret_cast<unsigned char*>(directory), sizeof(export_directory), decrypt);
		}

		bool file_mapper::unpack_exports(export_directory* directory)
		{
			if (directory->function_count <= 0 || directory->name_count <= 0)
				return true;

			char** name_addresses = reinterpret_cast<char**>(this->data() + this->rva_to_offset(directory->names_address));
			unsigned char** function_addresses = reinterpret_cast<unsigned char**>(this->data() + this->rva_to_offset(directory->functions_address));
			unsigned short* name_ordinals_addresses = reinterpret_cast<unsigned short*>(this->data() + this->rva_to_offset(directory->name_ordinals_address));

			for (unsigned int i = 0; i < directory->name_count; i++)
			{
				char* function_name = reinterpret_cast<char*>(this->data() + this->rva_to_offset(reinterpret_cast<unsigned int>(name_addresses[i])));
				unsigned short function_ordinal = name_ordinals_addresses[i];
				unsigned char* function = reinterpret_cast<unsigned char*>(this->data() + this->rva_to_offset(reinterpret_cast<unsigned int>(function_addresses[function_ordinal])));
			}

			return true;
		}

		bool file_mapper::update_imports(bool decrypt)
		{
			if (!import_infos.first || !import_infos.second)
				return false;

			for (import_descriptor* descriptor = reinterpret_cast<import_descriptor*>(this->data() + this->rva_to_offset(this->import_infos.first)); descriptor->characteristics != 0; descriptor++)
			{
				if (decrypt)
					this->crypt_buffer(reinterpret_cast<unsigned char*>(descriptor), sizeof(import_descriptor), decrypt);

				if (!this->unpack_imports(descriptor, decrypt))
					return false;

				if (!decrypt)
					this->crypt_buffer(reinterpret_cast<unsigned char*>(descriptor), sizeof(import_descriptor), decrypt);
			}

			return true;
		}

		bool file_mapper::unpack_imports(import_descriptor* descriptor, bool decrypt)
		{
			const unsigned int ordinal_flag_32 = 0x80000000;

			if (descriptor->original_first_thunk)
			{
				for (thunk_data* thunk = reinterpret_cast<thunk_data*>(this->data() + this->rva_to_offset(descriptor->original_first_thunk)); thunk->data_address != 0; thunk++)
				{
					if (!(thunk->ordinal & ordinal_flag_32))
					{
						unsigned char* function_name = reinterpret_cast<import_by_name*>(this->data() + this->rva_to_offset(thunk->function))->name;
						//this->crypt_buffer(function_name, strlen(reinterpret_cast<char*>(function_name)));
					}
				}

				unsigned char* library_name = this->data() + this->rva_to_offset(descriptor->name);
				this->crypt_buffer(library_name, strlen(reinterpret_cast<char*>(library_name)), decrypt);
			}

			return true;
		}

		bool file_mapper::update_sections(bool decrypt)
		{
			for (section_header& section : this->sections)
			{
				unsigned char* section_raw_pointer = this->data() + section.raw_data_pointer;
				this->crypt_buffer(section_raw_pointer, section.raw_data_size, decrypt);
			}

			return true;
		}

		unsigned int file_mapper::rva_to_offset(unsigned int rva)
		{
			if (rva)
			{
				for (section_header& section : this->sections)
				{
					if (section.virtual_address <= rva && (section.virtual_address + section.misc.virtual_size) > rva)
						return (rva + (section.raw_data_pointer - section.virtual_address));
				}
			}

			return rva;
		}

		void file_mapper::crypt_buffer(unsigned char* buffer, std::size_t size, bool decrypt)
		{
			if (decrypt)
			{
				CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption decryptor(this->aes.key, sizeof(aes_key), this->aes.iv);
				decryptor.ProcessData(buffer, buffer, size);
			}
			else
			{
				CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption encryptor(this->aes.key, sizeof(aes_key), this->aes.iv);
				encryptor.ProcessData(buffer, buffer, size);
			}
		}
	}
}