#include "custom_tabpage.hpp"
#include "main_window.hpp"

#ifdef _PRODUCTION
#include <VMProtectSDK.h>
#endif

namespace firefly
{
	/* tabpage */
	custom_tabpage::custom_tabpage(main_window* parent)
		: tabpage(*parent), parent(parent)
	{
		this->requires_logged_in = true;
		this->toggled_on = false;
	}
			
	custom_tabpage::~custom_tabpage()
	{

	}
	
#ifdef _PRODUCTION
#pragma optimize("g", off)
#endif
	bool custom_tabpage::toggle(bool checked, bool is_logged_in)
	{
#ifdef _PRODUCTION
		VMProtectBeginVirtualization("custom_tabpage::toggle");
#endif
		if (is_logged_in)
		{
			this->toggle_functionality(checked);
		}
		else
		{
			MessageBox(this->parent->handle(), "You must be logged in to use the trainer.", "Authority Error", MB_OK | MB_ICONERROR | MB_TOPMOST | MB_SETFOREGROUND);
			return false;
		}

		return true;
#ifdef _PRODUCTION
		VMProtectEnd();
#endif
	}
#ifdef _PRODUCTION
#pragma optimize("g", on)
#endif
		
	bool custom_tabpage::get_toggle()
	{
		return this->toggled_on;
	}

	void custom_tabpage::set_toggle(bool checked, bool is_logged_in)
	{
		if (is_logged_in)
		{
			this->on_toggled(this->toggled_on = checked);
			//this->enable(this->toggled_on);
		}
	}

	void custom_tabpage::create(std::shared_ptr<custom_tabpage> self, rectangle& rect)
	{
		this->self = self;
		tabpage::create(rect);
	}
	
	void custom_tabpage::save_to_profile(std::string const& profile_path)
	{
		/* placeholder for virtual function */
	}

	void custom_tabpage::load_from_profile(std::string const& profile_path)
	{
		/* placeholder for virtual function */
	}
	
	void custom_tabpage::toggle_functionality(bool checked)
	{
		/* placeholder for virtual function */
	}
	
	void custom_tabpage::on_toggled(bool checked)
	{
		/* placeholder for virtual function */
	}

	bool custom_tabpage::send_game_packet(net_buffer& packet)
	{
		return this->parent->send_game_packet(packet);
	}
}