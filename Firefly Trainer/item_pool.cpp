#include "item_pool.hpp"
#include "utility.hpp"

#include <cassert>
#include <fstream>
#include <functional>
#include <regex>

namespace firefly
{
	item_data::item_data(std::string const&& id, std::string const&& name)
		: id(id), name(name)
	{

	}

	item_pool::item_pool()
	{

	}

	item_pool::~item_pool()
	{

	}

	void item_pool::load_items(std::string const& file_path)
	{
		std::ifstream file(file_path);
		std::string item_string;

		while (file.good())
		{
			std::getline(file, item_string);
			this->parse_item(item_string);
		}
	}
	
	std::string item_pool::get_item_name(std::string const& item_id)
	{
		if (!this->items.empty())
		{
			for (std::vector<const item_data>::const_iterator iterate = this->items.begin(), end = this->items.end(); iterate != end; iterate++)
			{
				try
				{
					if (utility::upper(iterate->id).compare(utility::upper(item_id)) != std::string::npos)
						return iterate->name;
				}
				catch (std::exception& e)
				{
					UNREFERENCED_PARAMETER(e);
				}
			}
		}
		
		return std::string("");
	}

	bool item_pool::query_items(std::string const& query, std::vector<const item_data>& results)
	{
		results.clear();

		if (this->items.empty())
			return false;

		for (std::vector<const item_data>::const_iterator iterate = this->items.begin(), end = this->items.end(); iterate != end; iterate++)
		{
			if (utility::upper(iterate->id).find(utility::upper(query)) != std::string::npos || utility::upper(iterate->name).find(utility::upper(query)) != std::string::npos)
			{
				results.push_back(*iterate);
			}
		}

		return (!results.empty());
	}

	bool item_pool::parse_item(std::string const& item_string)
	{
		if (item_string.empty())
			return false;

		std::tr1::regex rgx("(\\d+) (.+)");
		std::tr1::smatch matches;

		if (std::tr1::regex_search(item_string, matches, rgx))
		{
			try
			{
				this->items.push_back(item_data(matches[1].str(), matches[2].str()));
			}
			catch (...)
			{
				return false;
			}
		}

		return true;
	}
}