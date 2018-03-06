#pragma once

#include "native.hpp"

#include <string>
#include <vector>

namespace firefly
{
	class item_data
	{
	public:
		explicit item_data(std::string const&& Id, std::string const&& Name);

		std::string id;
		std::string name;
	};

	class item_pool
	{
	public:
		item_pool();
		~item_pool();

		std::string get_item_name(std::string const& item_id);

		void load_items(std::string const& file_path);
		bool query_items(std::string const& query, std::vector<const item_data>& results);

	private:
		std::vector<const item_data> items;

		bool parse_item(std::string const& item_string);
	};
}