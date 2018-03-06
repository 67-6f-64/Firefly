#pragma once

#include "native.hpp"
#include "net_buffer.hpp"
#include "net_opcodes.hpp"

namespace firefly
{
	class autologin_data
	{
	public:
		static autologin_data& get_instance()
		{
			static autologin_data instance;
			return instance;
		}

		void handle_packet(net_buffer& packet);

		bool enabled() const;
		
		std::string const& username() const;
		std::string const& password() const;
		std::string const& pic() const;
		
		unsigned char region() const;
		unsigned char world() const;
		unsigned char channel() const;
		unsigned char character() const;

		bool offline_mode() const;
		bool randomize_hwid() const;

	private:
		autologin_data();

	private:
		bool enabled_;
		
		std::string username_;
		std::string password_;
		std::string pic_;

		unsigned char region_;
		unsigned char world_;
		unsigned char channel_;
		unsigned char character_;

		bool offline_mode_;
		bool randomize_hwid_;
	};
}