#pragma once

#include "native.hpp"
#include "thread_manager.hpp"

#include <string>

namespace firefly
{
	enum windows_inject_api
	{
		api_load_library_a = 0,
		api_load_library_w,
		api_load_library_ex_a,
		api_load_library_ex_w,
		api_ldr_load_dll
	};

#pragma warning(disable: 4200)

#pragma pack(push, 1)
	struct stub_data
	{
		void* inject_api;
		void* init_unicode_string;

		union
		{
			char file_path[0];
			wchar_t wide_file_path[0];
		};
	};
#pragma pack(pop)

	class windows_injector : public thread_manager
	{
	public:
		bool inject_file(std::string const& file, int process_id, windows_inject_api inject_api, thread_inject_api thread_api);
		bool inject_file(std::string const& file, HANDLE process, windows_inject_api inject_api, thread_inject_api thread_api, HANDLE* thread_handle = NULL);

	protected:
		windows_injector();
		~windows_injector();

	private:
		bool write_apis(windows_inject_api inject_api, HANDLE process, stub_data* memory_data);
		bool write_file_path(windows_inject_api inject_api, HANDLE process, stub_data* memory_data, std::string const& file);
		bool write_api_stub(windows_inject_api inject_api, HANDLE process, void* memory_stub);
	};
}