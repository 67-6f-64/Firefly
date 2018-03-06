#pragma once

#include "net_socket.hpp"

namespace firefly
{
	class net_callback
	{
	public:
		virtual void on_socket_message(SOCKET desc, unsigned short select_event, unsigned short select_error) = 0;
	};

	/* assumed non-blocking socket */
	class net_socket_async : public net_socket
	{
		static LRESULT CALLBACK window_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		
	public:
		net_socket_async();
		~net_socket_async();
		
		bool create_callback_window(HINSTANCE instance, net_callback* window);

		bool initialize_socket(bool server, HWND hwnd, unsigned int message_id);
		bool uninitialize_socket();
		
		bool generate_key(desc_info& desc, aes_buffer& aes);
		
		bool read_packet(desc_info& desc, net_buffer& packet, bool decrypt = true);
		bool read_collect(desc_info& desc);

		bool write_packet(desc_info& desc, net_buffer& packet, bool encrypt = true);
		bool write_flush(desc_info& desc);

		void reset_buffers();

	private:
		HWND hwnd_;

		std::vector<unsigned char> read_buffer;
		std::vector<unsigned char> write_buffer;
	};
}