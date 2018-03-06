#include "hacks_tabpage.hpp"
#include "resource.hpp"

#include "button.hpp"

namespace firefly
{
	hacks_tabpage::hacks_tabpage(main_window* parent)
		: custom_tabpage(parent)
	{
		this->exception_list[hack_type::full_godmode] = hack_type::s58_godmode;
		this->exception_list[hack_type::s58_godmode] = hack_type::full_godmode;

		this->exception_list[hack_type::suck_left] = hack_type::suck_right;
		this->exception_list[hack_type::suck_right] = hack_type::suck_left;

		this->exception_list[hack_type::auto_aggro] = hack_type::mob_freeze;
		this->exception_list[hack_type::mob_freeze] = hack_type::auto_aggro;
	}

	hacks_tabpage::~hacks_tabpage()
	{

	}

	void hacks_tabpage::save_to_profile(std::string const& profile_path)
	{
		this->profile_writer.set_path(profile_path);

		for (std::shared_ptr<togglebox> checkbox : this->togglebox_container)
		{
			this->profile_writer.write_int("Hacks", checkbox.get()->get_text() + "Check", checkbox.get()->get_check());
		}
	}

	void hacks_tabpage::load_from_profile(std::string const& profile_path)
	{
		this->profile_reader.set_path(profile_path);
		
		for (std::shared_ptr<togglebox> checkbox : this->togglebox_container)
		{
			checkbox.get()->set_check(this->profile_reader.get_int("Hacks", checkbox.get()->get_text() + "Check", FALSE) != FALSE);
		}
	}
	
	void hacks_tabpage::toggle_functionality(bool checked)
	{
		net_buffer packet(trainer_tabpage_toggle);
		packet.write1(tabpage_ids::hacks);
		packet.write1(-1);

		if (packet.write1(checked))
		{
			packet.write4(this->togglebox_container.size());

			for (std::shared_ptr<togglebox> checkbox : this->togglebox_container)
			{
				packet.write1(checkbox.get()->get_check());
			}
		}

		this->send_game_packet(packet);
	}
	
	void hacks_tabpage::create_controls(rectangle& rect)
	{
		/* section image */
		std::shared_ptr<button> section_image = std::make_shared<button>(*this);
		section_image.get()->create("", rectangle((rect.width / 2) - (image_size / 2), image_offset, image_size, image_size));
		section_image.get()->set_bitmap(png_button_hacks);
		this->store_control(section_image);
		
		/* Hack controls */
		typedef struct
		{
			const std::string hack_name;
			const std::string hack_description;
		} hack_entry;

		typedef struct
		{
			const std::string seperator_name;
			const std::initializer_list<hack_entry> hack_entries;
		} hack_section;
		
		hack_section hack_sections[] =
		{
			{ "Character Hacks", 
				{
					{ "Full Godmode", "Renders the user invincible." },
					{ "40 Second Godmode", "Renders the user invincible for the specified amount of time after taking initial damage." },
					{ "Face Left After Refresh", "Your character will face left when spawning." },
					{ "Aircheck (Attack On Rope)", "Allows the user to attack in mid-air, or while hanging on a rope/ladder." },
					{ "No Knockback", "Stops the user from getting knocked back upon taking damage." },
					{ "Knockback Unlimited Attack", "Knocks you back a little, whenever you reach the attack-limit for one spot. Switches between left and right." },
				}
			},
			{ "Mob Hacks", 
				{
					{ "Full Disarm", "Prevent mobs from casting spells." },
					{ "Faster Mobs", "Increases the movement speed of all mobs." },
					{ "No Animation", "Removes the spawn and death animations, making spawn-rates faster." },
					{ "No Knockback", "Stops the mobs from getting knocked back upon taking damage." },
					{ "Suck Left", "Vacs all the mobs left." },
					{ "Suck Right", "Vacs all the mobs right." },
					{ "Auto Aggro", "Forces all mobs to target you, and thus also move towards you." },
					{ "Freeze", "Forces all mobs to stand still." },
				}
			},
			{ "CPU Hacks", 
				{
					{ "No Background", "Hides the in-game background." },
					{ "No Structures/Platforms", "Hides the in-game platforms, structures, tiles, etc." },
					{ "No Fadestages", "Removes the fade-out and fade-in when changing maps or channels." },
					{ "No Hit Effect", "Hides the in-game skill-animation when hitting mobs." },
					{ "No Damage Text", "Hides the in-game damage-text when attacking mobs." },
					{ "No Kill Text", "Hides the in-game multikill-text and combokill-text when killing mobs." },
				}
			},
			{ "Automatics", 
				{
					{ "Auto NPC", "Automatically skips NPC dialogs (immitates 'next'-clicking behaviour)." },
					{ "Auto-Exit CashShop", "Automatically exits the CashShop." },
					{ "Auto Rune", "Automatically uses runes upon spawning." },
					{ "Auto Sudden Mission", "Automatically finishes sudden missions upon completion." },
				}
			},
			{ "Miscellaneous", 
				{
					{ "No Blue Boxes", "Removes all blue pop-up dialogs (e.g. Crusader Codex) and automatically answers 'yes' to the same type of Yes-No dialogs." },
					{ "No Falling Objects", "Stops falling objects from being spawned (e.g. Magnus Meteors, Pierre Hats, etc.)." },
					{ "Super Tubi", "Allows the user to loot faster than normally." },
					{ "Semi Itemvac", "Forces the item drop position (when killing mobs) to the users position." },
					{ "Familiar Disarm", "Stops your familiar from attacking, which preserves vitality." },
				}
			}
		};
		
		int section_seperator_continue = (image_offset + image_size + image_offset);

		for (int i = 0, j = 0; i < _countof(hack_sections); i++, section_seperator_continue += STANDARD_INDENT)
		{
			section_seperator_continue = this->create_seperator(hack_sections[i].seperator_name, section_seperator_continue);

			int count = 0; 

			for (hack_entry const& entry : hack_sections[i].hack_entries)
			{
				std::shared_ptr<togglebox> hack_togglebox = std::make_shared<togglebox>(*this);
				hack_togglebox.get()->create(entry.hack_name, rectangle((count % 2 == 0) ? TABPAGE_INDENT_LEFT : (rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT)), section_seperator_continue, control_width, control_height));
				hack_togglebox.get()->set_tooltip(entry.hack_description);
				hack_togglebox.get()->set_event(togglebox_event::on_state_change, std::bind(&hacks_tabpage::toggle_func, this, j++, std::placeholders::_1));
				
				this->togglebox_container.push_back(hack_togglebox);

				if (count++ % 2 != 0)
					section_seperator_continue += (control_height + control_indent);
			}
		}
	}
	
	bool hacks_tabpage::toggle_func(int index, bool enabled)
	{
		if (static_cast<hack_type>(index) == hack_type::auto_npc ||
			static_cast<hack_type>(index) == hack_type::auto_rune ||
			static_cast<hack_type>(index) == hack_type::auto_sudden_mission)
			return false;

		if (enabled)
		{
			std::map<hack_type, hack_type>::iterator iter = this->exception_list.find(static_cast<hack_type>(index));

			if (iter != this->exception_list.end())
			{
				if (this->togglebox_container.at(static_cast<std::size_t>(iter->second)).get()->get_check())
					this->togglebox_container.at(static_cast<std::size_t>(iter->second)).get()->set_check(false);
			}
		}

		if (this->toggled_on)
		{
			net_buffer packet(trainer_tabpage_toggle);
			packet.write1(tabpage_ids::hacks);
			packet.write1(index);
			packet.write1(enabled);

			this->send_game_packet(packet);
		}

		return true;
	}
}