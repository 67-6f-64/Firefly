#pragma once

#include "native.hpp"

namespace firefly
{
	enum thread_inject_api
	{
		api_create_remote_thread = 0,
		api_create_remote_thread_ex,
		api_nt_create_thread_ex,
	};

	class thread_manager
	{
	protected:
		thread_manager();
		~thread_manager();

		bool create_thread(thread_inject_api thread_api, HANDLE process, void* start_address, void* parameter, HANDLE* thread_handle = NULL);

	private:
		bool create_remote_thread(HANDLE process, void* start_address, void* parameter, HANDLE* thread_handle);
		bool create_remote_thread_ex(HANDLE process, void* start_address, void* parameter, HANDLE* thread_handle);
		bool nt_create_thread_ex(HANDLE process, void* start_address, void* parameter, HANDLE* thread_handle);
	};
}