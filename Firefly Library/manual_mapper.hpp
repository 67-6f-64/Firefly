#pragma once

#include "native.hpp"
#include "windows_injector.hpp"

#include "file_mapper.hpp"
#include "pe_structs.hpp"

#include <string>

namespace firefly
{
	class manual_mapper : public windows_injector
	{
	public:
		manual_mapper();
		~manual_mapper();

		bool manual_map_memory_file(io::file_mapper& file_map, int process_id, bool exclude_header, thread_inject_api thread_api);

	private:
		unsigned char* allocate_image_base(HANDLE process, nt_header* nt_headers);
		unsigned char* allocate_manual_func(HANDLE process, unsigned char* destination, void* source, std::size_t size);

		section_header* get_rva_section(unsigned int rva, nt_header* nt_headers);
		unsigned int get_rva_offset(unsigned int rva, nt_header* nt_headers);

		bool write_headers(HANDLE process, nt_header* nt_headers, unsigned char* image_base, io::file_mapper& file_map);
		bool write_sections(HANDLE process, nt_header* nt_headers, unsigned char* image_base, io::file_mapper& file_map);

		bool build_import_table(HANDLE process, nt_header* nt_headers, unsigned char* image_base, io::file_mapper& file_map, thread_inject_api thread_api);
		bool relocate_base(HANDLE process, nt_header* nt_headers, unsigned char* image_base, io::file_mapper& file_map);
		bool set_page_permissions(HANDLE process, nt_header* nt_headers, unsigned char* image_base);
		bool execute_tls_entrypoints(HANDLE process, nt_header* nt_headers, unsigned char* image_base, thread_inject_api thread_api);
		bool execute_entrypoint(HANDLE process, nt_header* nt_headers, unsigned char* image_base, thread_inject_api thread_api);

	private:
		HMODULE get_remote_library(HANDLE process, std::string const& library_name);
		HMODULE load_remote_library(HANDLE process, std::string const& library_name, thread_inject_api thread_api);

		FARPROC get_remote_proc_address(HANDLE process, HMODULE module, const char* proc_name);
	};
}