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

	}
			
	custom_tabpage::~custom_tabpage()
	{

	}
	
	void custom_tabpage::create(rectangle& rect)
	{
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
}