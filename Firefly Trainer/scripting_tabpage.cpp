#include "scripting_tabpage.hpp"
#include "resource.hpp"

#include "button.hpp"
#include "label.hpp"

namespace firefly
{
	scripting_tabpage::scripting_tabpage(main_window* parent)
		: custom_tabpage(parent)
	{

	}

	scripting_tabpage::~scripting_tabpage()
	{

	}

	void scripting_tabpage::save_to_profile(std::string const& profile_path)
	{
		this->profile_writer.set_path(profile_path);
	}

	void scripting_tabpage::load_from_profile(std::string const& profile_path)
	{
		this->profile_reader.set_path(profile_path);
	}
	
	void scripting_tabpage::create_controls(rectangle& rect)
	{
		/* section image */
		std::shared_ptr<button> section_image = std::make_shared<button>(*this);
		section_image.get()->create("", rectangle((rect.width / 2) - (image_size / 2), image_offset, image_size, image_size));
		section_image.get()->set_bitmap(png_button_scripting);
		this->store_control(section_image);
	}

	void scripting_tabpage::create_handlers()
	{
		this->add_message_handler(WM_MEASUREITEM, [](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			MEASUREITEMSTRUCT* measure_item = reinterpret_cast<MEASUREITEMSTRUCT*>(lParam);

			if (measure_item->CtlType == ODT_COMBOBOX)
				measure_item->itemHeight = 18;

			return 0;
		});
	}
}