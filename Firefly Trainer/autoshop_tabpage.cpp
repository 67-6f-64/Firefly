#include "autoshop_tabpage.hpp"
#include "resource.hpp"

#include "button.hpp"
#include "label.hpp"

namespace firefly
{
	autoshop_tabpage::autoshop_tabpage(main_window* parent)
		: custom_tabpage(parent)
	{

	}

	autoshop_tabpage::~autoshop_tabpage()
	{

	}

	void autoshop_tabpage::save_to_profile(std::string const& profile_path)
	{
		this->profile_writer.set_path(profile_path);
		
		this->profile_writer.write_int("AutoShop", "AutoPotRestockHpCheck", this->auto_potion_restock_hp_togglebox.get()->get_check());
		this->profile_writer.write_string("AutoShop", "AutoPotRestockHpFrom", this->auto_potion_restock_hp_from_textbox.get()->get_text());
		this->profile_writer.write_string("AutoShop", "AutoPotRestockHpTo", this->auto_potion_restock_hp_to_textbox.get()->get_text());
		
		this->profile_writer.write_int("AutoShop", "AutoPotRestockMpCheck", this->auto_potion_restock_mp_togglebox.get()->get_check());
		this->profile_writer.write_string("AutoShop", "AutoPotRestockMpFrom", this->auto_potion_restock_mp_from_textbox.get()->get_text());
		this->profile_writer.write_string("AutoShop", "AutoPotRestockMpTo", this->auto_potion_restock_mp_to_textbox.get()->get_text());

		this->profile_writer.write_int("AutoShop", "AutoStarBulletRestockCheck", this->auto_restock_stars_bullets_togglebox.get()->get_check());

		this->profile_writer.write_int("AutoShop", "AutoSellCheck", this->auto_sell_togglebox.get()->get_check());
		
		for (std::size_t i = 0; i < this->auto_sell_type_togglebox_container.size(); i++)
			this->profile_writer.write_int("AutoShop", "AutoSellType" + std::to_string(i) + "Check", this->auto_sell_type_togglebox_container.at(i).get()->get_check());
		
		this->profile_writer.write_string("AutoShop", "AutoSellDelay", this->auto_sell_delay_textbox.get()->get_text());
		
		for (std::size_t i = 0; i < this->auto_sell_filter_type_togglebox_container.size(); i++)
			this->profile_writer.write_int("AutoShop", "AutoSellFilterType" + std::to_string(i) + "Check", this->auto_sell_filter_type_togglebox_container.at(i).get()->get_check());
		
		this->profile_writer.write_int("AutoShop", "AutoTransferMesosCheck", this->auto_transfer_mesos_togglebox.get()->get_check());

		this->profile_writer.write_string("AutoShop", "AutoTransferMesosFrom", this->auto_transfer_mesos_textbox.get()->get_text());
		this->profile_writer.write_string("AutoShop", "AutoTransferMesosTo", this->auto_transfer_mesos_limit_textbox.get()->get_text());
		this->profile_writer.write_string("AutoShop", "AutoTransferMesosDelay", this->auto_transfer_mesos_delay_textbox.get()->get_text());

		this->profile_writer.write_int("AutoShop", "AutoTransferChannelCombo", this->auto_transfer_channel_combobox.get()->get_selection());
		this->profile_writer.write_int("AutoShop", "AutoTransferFreeMarketCombo", this->auto_transfer_free_market_combobox.get()->get_selection());
		this->profile_writer.write_string("AutoShop", "AutoTransferShopId", this->auto_transfer_shop_id_textbox.get()->get_text());
		this->profile_writer.write_string("AutoShop", "AutoTransferItemId", this->auto_transfer_item_textbox.get()->get_text());
		this->profile_writer.write_string("AutoShop", "AutoTransferItemPrice", this->auto_transfer_item_price_textbox.get()->get_text());
	}
	
	void autoshop_tabpage::load_from_profile(std::string const& profile_path)
	{
		this->profile_reader.set_path(profile_path);
		
		this->auto_potion_restock_hp_togglebox.get()->set_check(this->profile_reader.get_int("AutoShop", "AutoPotRestockHpCheck", FALSE) != FALSE);
		this->auto_potion_restock_hp_from_textbox.get()->set_text(this->profile_reader.get_string("AutoShop", "AutoPotRestockHpFrom", ""));
		this->auto_potion_restock_hp_to_textbox.get()->set_text(this->profile_reader.get_string("AutoShop", "AutoPotRestockHpTo", ""));
		
		this->auto_potion_restock_mp_togglebox.get()->set_check(this->profile_reader.get_int("AutoShop", "AutoPotRestockMpCheck", FALSE) != FALSE);
		this->auto_potion_restock_mp_from_textbox.get()->set_text(this->profile_reader.get_string("AutoShop", "AutoPotRestockMpFrom", ""));
		this->auto_potion_restock_mp_to_textbox.get()->set_text(this->profile_reader.get_string("AutoShop", "AutoPotRestockMpTo", ""));

		this->auto_restock_stars_bullets_togglebox.get()->set_check(this->profile_reader.get_int("AutoShop", "AutoStarBulletRestockCheck", FALSE) != FALSE);

		this->auto_sell_togglebox.get()->set_check(this->profile_reader.get_int("AutoShop", "AutoSellCheck", FALSE) != FALSE);
		
		for (std::size_t i = 0; i < this->auto_sell_type_togglebox_container.size(); i++)
			this->auto_sell_type_togglebox_container.at(i).get()->set_check(this->profile_reader.get_int("AutoShop", "AutoSellType" + std::to_string(i) + "Check", FALSE) != FALSE);

		this->auto_sell_delay_textbox.get()->set_text(this->profile_reader.get_string("AutoShop", "AutoSellDelay", ""));
		
		for (std::size_t i = 0; i < this->auto_sell_type_togglebox_container.size(); i++)
			this->auto_sell_filter_type_togglebox_container.at(i).get()->set_check(this->profile_reader.get_int("AutoShop", "AutoSellFilterType" + std::to_string(i) + "Check", FALSE) != FALSE);

		this->auto_transfer_mesos_togglebox.get()->set_check(this->profile_reader.get_int("AutoShop", "AutoTransferMesosCheck", FALSE) != FALSE);
		
		this->auto_transfer_mesos_textbox.get()->set_text(this->profile_reader.get_string("AutoShop", "AutoTransferMesosFrom", ""));
		this->auto_transfer_mesos_limit_textbox.get()->set_text(this->profile_reader.get_string("AutoShop", "AutoTransferMesosTo", ""));
		this->auto_transfer_mesos_delay_textbox.get()->set_text(this->profile_reader.get_string("AutoShop", "AutoTransferMesosDelay", ""));

		this->auto_transfer_channel_combobox.get()->set_selection(this->profile_reader.get_int("AutoShop", "AutoTransferChannelCombo", 0));
		this->auto_transfer_free_market_combobox.get()->set_selection(this->profile_reader.get_int("AutoShop", "AutoTransferFreeMarketCombo", 0));

		this->auto_transfer_shop_id_textbox.get()->set_text(this->profile_reader.get_string("AutoShop", "AutoTransferShopId", ""));
		this->auto_transfer_item_textbox.get()->set_text(this->profile_reader.get_string("AutoShop", "AutoTransferItemId", ""));
		this->auto_transfer_item_price_textbox.get()->set_text(this->profile_reader.get_string("AutoShop", "AutoTransferItemPrice", ""));
	}

	void autoshop_tabpage::toggle_functionality(bool checked)
	{
		net_buffer packet(trainer_tabpage_toggle);
		packet.write1(tabpage_ids::autoshop);
		packet.write1(autoshop_type_ids::autoshop_toggle);
		
		if (packet.write1(checked))
		{
			if (packet.write1(this->auto_potion_restock_hp_togglebox.get()->get_check()))
			{
				packet.write2(this->auto_potion_restock_hp_from_textbox.get()->get_integer());
				packet.write2(this->auto_potion_restock_hp_to_textbox.get()->get_integer());
			}

			if (packet.write1(this->auto_potion_restock_mp_togglebox.get()->get_check()))
			{
				packet.write2(this->auto_potion_restock_mp_from_textbox.get()->get_integer());
				packet.write2(this->auto_potion_restock_mp_to_textbox.get()->get_integer());
			}

			packet.write1(this->auto_restock_stars_bullets_togglebox.get()->get_check());

			if (packet.write1(this->auto_sell_togglebox.get()->get_check()))
			{
				for (std::size_t i = 0; i < this->auto_sell_type_togglebox_container.size(); i++)
					packet.write1(this->auto_sell_type_togglebox_container.at(i).get()->get_check());

				packet.write4(this->auto_sell_delay_textbox.get()->get_integer());
		
				for (std::size_t i = 0; i < this->auto_sell_type_togglebox_container.size(); i++)
					packet.write1(this->auto_sell_filter_type_togglebox_container.at(i).get()->get_check());
			}

			if (packet.write1(this->auto_transfer_mesos_togglebox.get()->get_check()))
			{
				packet.write4(this->auto_transfer_mesos_textbox.get()->get_integer());
				packet.write4(this->auto_transfer_mesos_limit_textbox.get()->get_integer());
				packet.write4(this->auto_transfer_mesos_delay_textbox.get()->get_integer());
				
				packet.write1(this->auto_transfer_channel_combobox.get()->get_selection());
				packet.write1(this->auto_transfer_free_market_combobox.get()->get_selection());
				
				packet.write4(this->auto_transfer_shop_id_textbox.get()->get_integer());
				packet.write4(this->auto_transfer_item_textbox.get()->get_integer());
				packet.write4(this->auto_transfer_item_price_textbox.get()->get_integer());
			}
		}

		this->send_game_packet(packet);
	}
	
	void autoshop_tabpage::create_controls(rectangle& rect)
	{
		/* section image */
		std::shared_ptr<button> section_image = std::make_shared<button>(*this);
		section_image.get()->create("", rectangle((rect.width / 2) - (image_size / 2), image_offset, image_size, image_size));
		section_image.get()->set_bitmap(png_button_autoshop);
		this->store_control(section_image);

		/* Auto Restock */
		int auto_restock_seperator_continue = this->create_seperator("Auto Restock", image_offset + image_size + image_offset);
		int potion_restock_textbox_width = ((control_width + key_indent + key_width) / 2) - 10;

		this->auto_potion_restock_hp_togglebox = std::make_unique<togglebox>(*this);
		this->auto_potion_restock_hp_togglebox.get()->create("Restock Health Potions", rectangle(TABPAGE_INDENT_LEFT, auto_restock_seperator_continue, control_width, control_height));
		
		std::shared_ptr<label> auto_potion_restock_hp_from_label = std::make_shared<label>(*this);
		auto_potion_restock_hp_from_label.get()->create("from <", rectangle(rect.width - (label_width + key_indent + control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_restock_seperator_continue, label_width, control_height), true);
		this->store_control(auto_potion_restock_hp_from_label);
		
		this->auto_potion_restock_hp_from_textbox = std::make_unique<textbox>(*this);
		this->auto_potion_restock_hp_from_textbox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_restock_seperator_continue, potion_restock_textbox_width, control_height), true, true);
		this->auto_potion_restock_hp_from_textbox.get()->set_maximum_length(40);
		this->auto_potion_restock_hp_from_textbox.get()->set_cue_banner("potions");
		
		std::shared_ptr<label> auto_potion_restock_hp_to_label = std::make_shared<label>(*this);
		auto_potion_restock_hp_to_label.get()->create("to", rectangle(rect.width - (20 + potion_restock_textbox_width + TABPAGE_INDENT_RIGHT), auto_restock_seperator_continue, 20, control_height), true);
		this->store_control(auto_potion_restock_hp_to_label);
		
		this->auto_potion_restock_hp_to_textbox = std::make_unique<textbox>(*this);
		this->auto_potion_restock_hp_to_textbox.get()->create(rectangle(rect.width - (potion_restock_textbox_width + TABPAGE_INDENT_RIGHT), auto_restock_seperator_continue, potion_restock_textbox_width, control_height), true, true);
		this->auto_potion_restock_hp_to_textbox.get()->set_maximum_length(40);
		this->auto_potion_restock_hp_to_textbox.get()->set_cue_banner("potions");
		
		this->auto_potion_restock_mp_togglebox = std::make_unique<togglebox>(*this);
		this->auto_potion_restock_mp_togglebox.get()->create("Restock Mana Potions", rectangle(TABPAGE_INDENT_LEFT, auto_restock_seperator_continue += (control_height + control_indent), control_width, control_height));
		
		std::shared_ptr<label> auto_potion_restock_mp_from_label = std::make_shared<label>(*this);
		auto_potion_restock_mp_from_label.get()->create("from <", rectangle(rect.width - (label_width + key_indent + control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_restock_seperator_continue, label_width, control_height), true);
		this->store_control(auto_potion_restock_mp_from_label);
		
		this->auto_potion_restock_mp_from_textbox = std::make_unique<textbox>(*this);
		this->auto_potion_restock_mp_from_textbox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_restock_seperator_continue, potion_restock_textbox_width, control_height), true, true);
		this->auto_potion_restock_mp_from_textbox.get()->set_maximum_length(40);
		this->auto_potion_restock_mp_from_textbox.get()->set_cue_banner("potions");
		
		std::shared_ptr<label> auto_potion_restock_mp_to_label = std::make_shared<label>(*this);
		auto_potion_restock_mp_to_label.get()->create("to", rectangle(rect.width - (20 + potion_restock_textbox_width + TABPAGE_INDENT_RIGHT), auto_restock_seperator_continue, 20, control_height), true);
		this->store_control(auto_potion_restock_mp_to_label);
		
		this->auto_potion_restock_mp_to_textbox = std::make_unique<textbox>(*this);
		this->auto_potion_restock_mp_to_textbox.get()->create(rectangle(rect.width - (potion_restock_textbox_width + TABPAGE_INDENT_RIGHT), auto_restock_seperator_continue, potion_restock_textbox_width, control_height), true, true);
		this->auto_potion_restock_mp_to_textbox.get()->set_maximum_length(40);
		this->auto_potion_restock_mp_to_textbox.get()->set_cue_banner("potions");
		
		this->auto_restock_stars_bullets_togglebox = std::make_unique<togglebox>(*this);
		this->auto_restock_stars_bullets_togglebox.get()->create("Recharge Stars/Bullets", rectangle(TABPAGE_INDENT_LEFT, auto_restock_seperator_continue += (control_height + control_indent), control_width, control_height));
		
		/* Auto Sell */
		int auto_sell_seperator_continue = this->create_seperator("Auto Sell", auto_restock_seperator_continue + control_height + control_indent + STANDARD_INDENT);
		
		this->auto_sell_togglebox = std::make_unique<togglebox>(*this);
		this->auto_sell_togglebox.get()->create("Auto Sell (when inventory is full)", rectangle(TABPAGE_INDENT_LEFT, auto_sell_seperator_continue, control_width, control_height));
		
		std::string auto_sell_types[] = { "Equip", "Etc", "Use" };

		for (int i = 0, count = _countof(auto_sell_types); i < count; i++)
		{
			const int togglebox_width = 16;

			int x = rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT);
			int cx = ((rect.width - TABPAGE_INDENT_RIGHT) - x) / count;
			int dx = ((cx / 2) - (togglebox_width / 2));

			std::shared_ptr<togglebox> auto_sell_type_togglebox = std::make_shared<togglebox>(*this);
			auto_sell_type_togglebox.get()->create("", rectangle(x + (cx * i) + dx, auto_sell_seperator_continue, 16, control_height));
			this->auto_sell_type_togglebox_container.push_back(auto_sell_type_togglebox);
			
			std::shared_ptr<label> auto_sell_type_label = std::make_shared<label>(*this);
			auto_sell_type_label.get()->create(auto_sell_types[i], rectangle(x + (cx * i), auto_sell_seperator_continue + control_height, cx, control_height), true);
			this->store_control(auto_sell_type_label);
		}
		
		auto_sell_seperator_continue += control_height;
		
		std::shared_ptr<label> auto_sell_using_label = std::make_shared<label>(*this);
		auto_sell_using_label.get()->create("using", rectangle(rect.width - (label_width + key_indent + control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_sell_seperator_continue += (control_height + control_indent), label_width, control_height), true);
		this->store_control(auto_sell_using_label);
		
		this->auto_sell_delay_textbox = std::make_unique<textbox>(*this);
		this->auto_sell_delay_textbox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_sell_seperator_continue, control_width + key_indent + key_width, control_height), true, true);
		this->auto_sell_delay_textbox.get()->set_maximum_length(40);
		this->auto_sell_delay_textbox.get()->set_cue_banner("milliseconds delay");
		
		std::shared_ptr<label> auto_sell_filtering_label = std::make_shared<label>(*this);
		auto_sell_filtering_label.get()->create("filtering", rectangle(rect.width - (label_width + key_indent + control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_sell_seperator_continue += (control_height + control_indent), label_width, control_height), true);
		this->store_control(auto_sell_filtering_label);
		
		std::string auto_sell_filter_types[] = { "Legendary Potential", "Star Enhanced Equipment", "Custom Filter (see Item Filter)" };

		for (int i = 0, count = _countof(auto_sell_filter_types); i < count; i++)
		{
			const int togglebox_width = 16;

			int x = rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT);
			int cx = ((rect.width - TABPAGE_INDENT_RIGHT) - x) / count;
			int dx = ((cx / 2) - (togglebox_width / 2));

			std::shared_ptr<togglebox> auto_sell_filter_type_togglebox = std::make_shared<togglebox>(*this);
			auto_sell_filter_type_togglebox.get()->create("", rectangle(x + (cx * i) + dx, auto_sell_seperator_continue, 16, control_height));
			this->auto_sell_filter_type_togglebox_container.push_back(auto_sell_filter_type_togglebox);
			
			std::shared_ptr<label> auto_sell_filter_type_label = std::make_shared<label>(*this);
			auto_sell_filter_type_label.get()->create(auto_sell_filter_types[i], rectangle(x + (cx * i), auto_sell_seperator_continue + control_height, cx, 30), true, false);
			this->store_control(auto_sell_filter_type_label);
		}
		
		auto_sell_seperator_continue += 35;
		
		std::shared_ptr<button> auto_sell_test_rush_button = std::make_shared<button>(*this);
		auto_sell_test_rush_button.get()->create("Test Sell Rush", rectangle(TABPAGE_INDENT_LEFT, auto_sell_seperator_continue += (control_height + control_indent), rect.width - (TABPAGE_INDENT_LEFT + TABPAGE_INDENT_RIGHT), 25));
		auto_sell_test_rush_button.get()->set_event(button_event::on_click, [this]() -> bool
		{
			net_buffer packet(trainer_tabpage_toggle);
			packet.write1(tabpage_ids::autoshop);
			packet.write1(autoshop_type_ids::autoshop_test_sell_rush);
		
			for (std::size_t i = 0; i < this->auto_sell_type_togglebox_container.size(); i++)
				packet.write1(this->auto_sell_type_togglebox_container.at(i).get()->get_check());

			packet.write4(this->auto_sell_delay_textbox.get()->get_integer());
		
			for (std::size_t i = 0; i < this->auto_sell_type_togglebox_container.size(); i++)
				packet.write1(this->auto_sell_filter_type_togglebox_container.at(i).get()->get_check());

			return this->send_game_packet(packet);
		});

		this->store_control(auto_sell_test_rush_button);

		/* Auto Transfer */
		int auto_transfer_seperator_continue = this->create_seperator("Auto Transfer", auto_sell_seperator_continue + control_height + control_indent + STANDARD_INDENT);
		int small_textbox_width = ((control_width + key_indent + key_width) / 2) - (control_indent / 2);
		
		this->auto_transfer_mesos_togglebox = std::make_unique<togglebox>(*this);
		this->auto_transfer_mesos_togglebox.get()->create("Auto Transfer", rectangle(TABPAGE_INDENT_LEFT, auto_transfer_seperator_continue, control_width, control_height));
		
		std::shared_ptr<label> auto_transfer_mesos_when_label = std::make_shared<label>(*this);
		auto_transfer_mesos_when_label.get()->create("when >", rectangle(rect.width - (label_width + key_indent + control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_transfer_seperator_continue, label_width, control_height), true);
		this->store_control(auto_transfer_mesos_when_label);
		
		this->auto_transfer_mesos_textbox = std::make_unique<textbox>(*this);
		this->auto_transfer_mesos_textbox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_transfer_seperator_continue, potion_restock_textbox_width, control_height), true, true);
		this->auto_transfer_mesos_textbox.get()->set_maximum_length(40);
		this->auto_transfer_mesos_textbox.get()->set_cue_banner("million mesos");
		
		std::shared_ptr<label> auto_transfer_untill_label = std::make_shared<label>(*this);
		auto_transfer_untill_label.get()->create("to", rectangle(rect.width - (20 + potion_restock_textbox_width + TABPAGE_INDENT_RIGHT), auto_transfer_seperator_continue, 20, control_height), true);
		this->store_control(auto_transfer_untill_label);
		
		this->auto_transfer_mesos_limit_textbox = std::make_unique<textbox>(*this);
		this->auto_transfer_mesos_limit_textbox.get()->create(rectangle(rect.width - (potion_restock_textbox_width + TABPAGE_INDENT_RIGHT), auto_transfer_seperator_continue, potion_restock_textbox_width, control_height), true, true);
		this->auto_transfer_mesos_limit_textbox.get()->set_maximum_length(40);
		this->auto_transfer_mesos_limit_textbox.get()->set_cue_banner("million mesos");
		
		std::shared_ptr<label> auto_transfer_mesos_using_label = std::make_shared<label>(*this);
		auto_transfer_mesos_using_label.get()->create("using", rectangle(rect.width - (label_width + key_indent + control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_transfer_seperator_continue += (control_height + control_indent), label_width, control_height), true);
		this->store_control(auto_transfer_mesos_using_label);
		
		this->auto_transfer_mesos_delay_textbox = std::make_unique<textbox>(*this);
		this->auto_transfer_mesos_delay_textbox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_transfer_seperator_continue, control_width + key_indent + key_width, control_height), true, true);
		this->auto_transfer_mesos_delay_textbox.get()->set_maximum_length(40);
		this->auto_transfer_mesos_delay_textbox.get()->set_cue_banner("milliseconds delay");
		
		std::shared_ptr<label> auto_transfer_channel_label = std::make_shared<label>(*this);
		auto_transfer_channel_label.get()->create("Transfer Target Channel:", rectangle(TABPAGE_INDENT_LEFT, auto_transfer_seperator_continue += (control_height + control_indent), control_width, control_height));
		this->store_control(auto_transfer_channel_label);

		this->auto_transfer_channel_combobox = std::make_unique<combobox>(*this);
		this->auto_transfer_channel_combobox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_transfer_seperator_continue, control_width + key_indent + key_width, control_height), true);
		
		for (int i = 0; i < 20; i++)
			this->auto_transfer_channel_combobox.get()->add_item("Channel " + std::to_string(i + 1));

		this->auto_transfer_channel_combobox.get()->set_selection(0);

		std::shared_ptr<label> auto_transfer_free_market_label = std::make_shared<label>(*this);
		auto_transfer_free_market_label.get()->create("Transfer Target Free Market:", rectangle(TABPAGE_INDENT_LEFT, auto_transfer_seperator_continue += (control_height + control_indent + 2), control_width, control_height));
		this->store_control(auto_transfer_free_market_label);

		this->auto_transfer_free_market_combobox = std::make_unique<combobox>(*this);
		this->auto_transfer_free_market_combobox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_transfer_seperator_continue, control_width + key_indent + key_width, control_height), true);
		
		for (int i = 0; i < 22; i++)
			this->auto_transfer_free_market_combobox.get()->add_item("Free Market " + std::to_string(i + 1));

		this->auto_transfer_free_market_combobox.get()->set_selection(0);

		std::shared_ptr<label> auto_transfer_shop_id_label = std::make_shared<label>(*this);
		auto_transfer_shop_id_label.get()->create("Transfer Target Shop ID:", rectangle(TABPAGE_INDENT_LEFT, auto_transfer_seperator_continue += (control_height + control_indent), control_width, control_height));
		this->store_control(auto_transfer_shop_id_label);

		this->auto_transfer_shop_id_textbox = std::make_unique<textbox>(*this);
		this->auto_transfer_shop_id_textbox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_transfer_seperator_continue += 2, control_width + key_indent + key_width, control_height), true, true);
		this->auto_transfer_shop_id_textbox.get()->set_maximum_length(20);
		this->auto_transfer_shop_id_textbox.get()->set_cue_banner("Shop ID...");

		std::shared_ptr<label> auto_transfer_item_label = std::make_shared<label>(*this);
		auto_transfer_item_label.get()->create("Transfer Target Shop Item:", rectangle(TABPAGE_INDENT_LEFT, auto_transfer_seperator_continue += (control_height + control_indent), control_width, control_height));
		this->store_control(auto_transfer_item_label);

		this->auto_transfer_item_textbox = std::make_unique<textbox>(*this);
		this->auto_transfer_item_textbox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT), auto_transfer_seperator_continue, small_textbox_width, control_height), true, true);
		this->auto_transfer_item_textbox.get()->set_maximum_length(20);
		this->auto_transfer_item_textbox.get()->set_cue_banner("Item ID...");

		this->auto_transfer_item_price_textbox = std::make_unique<textbox>(*this);
		this->auto_transfer_item_price_textbox.get()->create(rectangle(rect.width - (control_width + key_indent + key_width + TABPAGE_INDENT_RIGHT) + (control_indent + small_textbox_width), auto_transfer_seperator_continue, small_textbox_width, control_height), true, true);
		this->auto_transfer_item_price_textbox.get()->set_maximum_length(20);
		this->auto_transfer_item_price_textbox.get()->set_cue_banner("Item Price...");

		std::shared_ptr<button> auto_transfer_test_rush_button = std::make_shared<button>(*this);
		auto_transfer_test_rush_button.get()->create("Test Transfer Rush", rectangle(TABPAGE_INDENT_LEFT, auto_transfer_seperator_continue += (control_height + control_indent), rect.width - (TABPAGE_INDENT_LEFT + TABPAGE_INDENT_RIGHT), 25));
		auto_transfer_test_rush_button.get()->set_event(button_event::on_click, [this]() -> bool
		{
			net_buffer packet(trainer_tabpage_toggle);
			packet.write1(tabpage_ids::autoshop);
			packet.write1(autoshop_type_ids::autoshop_test_transfer_rush);
		
			packet.write4(this->auto_transfer_mesos_textbox.get()->get_integer());
			packet.write4(this->auto_transfer_mesos_limit_textbox.get()->get_integer());
			packet.write4(this->auto_transfer_mesos_delay_textbox.get()->get_integer());
				
			packet.write1(this->auto_transfer_channel_combobox.get()->get_selection());
			packet.write1(this->auto_transfer_free_market_combobox.get()->get_selection());
				
			packet.write4(this->auto_transfer_shop_id_textbox.get()->get_integer());
			packet.write4(this->auto_transfer_item_textbox.get()->get_integer());
			packet.write4(this->auto_transfer_item_price_textbox.get()->get_integer());

			return this->send_game_packet(packet);
		});

		this->store_control(auto_transfer_test_rush_button);
	}
}
