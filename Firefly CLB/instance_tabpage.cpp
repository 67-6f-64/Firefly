#include "instance_tabpage.hpp"

#include "config.hpp"
#include "resource.hpp"

#include "utility.hpp"

#include "button.hpp"

namespace firefly
{
	instance_tabpage::instance_tabpage(main_window* parent)
		: custom_tabpage(parent), client_socket()
	{

	}

	instance_tabpage::~instance_tabpage()
	{

	}

	void instance_tabpage::save_to_profile(std::string const& profile_path)
	{
		this->profile_writer.set_path(profile_path);
	}
	
	void instance_tabpage::load_from_profile(std::string const& profile_path)
	{
		this->profile_reader.set_path(profile_path);
	}
	
	bool instance_tabpage::on_connect(SOCKET sock)
	{
		printf("Instance connected..\n");
		return true;
	}

	bool instance_tabpage::on_close(SOCKET sock)
	{
		printf("Instance closed..\n");
		return true;
	}

	void instance_tabpage::create_handlers()
	{
		this->add_message_handler(WM_SOCKET, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			return this->on_socket_message(static_cast<SOCKET>(wParam), WSAGETSELECTEVENT(lParam), WSAGETSELECTERROR(lParam));
		});
	}
	
	void instance_tabpage::create_controls(rectangle& rect)
	{
		this->prepare_connection(this->handle());

		/* section image */
		std::shared_ptr<button> section_image = std::make_shared<button>(*this);
		section_image.get()->create("", rectangle((rect.width / 2) - (image_size / 2), image_offset, image_size, image_size));
		section_image.get()->set_bitmap(png_button_instance);
		section_image.get()->set_event(button_event::on_click, [this]() -> bool
		{
			this->account.username = "<censored>";
			this->account.password = "<censored>";
			this->account.pic = "<censored>";

			this->account.world_id = 30;
			this->account.channel_id = 17;
			this->account.character_name = "Kannanymous";

			return this->migrate(server_type::login);
		});
		this->store_control(section_image);
		
		/* Auto Pot */
		int auto_pot_seperator_continue = this->create_seperator("Auto Potion", image_offset + image_size + image_offset);
		
		this->auto_hp_togglebox = std::make_unique<togglebox>(*this);
		this->auto_hp_togglebox.get()->create("Use Health Potion", rectangle(TABPAGE_INDENT_LEFT, auto_pot_seperator_continue, control_width, control_height + 2));
		this->auto_hp_togglebox.get()->set_tooltip("Uses a health potion everytime your health goes below a certain %.");
		this->auto_hp_togglebox.get()->set_event(togglebox_event::on_state_change, [](bool enabled) -> bool
		{
			return true;
		});
	}
}