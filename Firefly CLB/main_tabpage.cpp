#include "main_tabpage.hpp"

#include "resource.hpp"
#include "utility.hpp"

#include "button.hpp"

namespace firefly
{
	main_tabpage::main_tabpage(main_window* parent)
		: custom_tabpage(parent)
	{

	}

	main_tabpage::~main_tabpage()
	{

	}

	void main_tabpage::save_to_profile(std::string const& profile_path)
	{
		this->profile_writer.set_path(profile_path);
	}
	
	void main_tabpage::load_from_profile(std::string const& profile_path)
	{
		this->profile_reader.set_path(profile_path);
	}
	
	void main_tabpage::create_controls(rectangle& rect)
	{
		/* section image */
		std::shared_ptr<button> section_image = std::make_shared<button>(*this);
		section_image.get()->create("", rectangle((rect.width / 2) - (image_size / 2), image_offset, image_size, image_size));
		section_image.get()->set_bitmap(png_button_main);
		this->store_control(section_image);
		
		/* Auto Pot */
		int auto_pot_seperator_continue = this->create_seperator("Auto Potion", image_offset + image_size + image_offset);
		
	}
}