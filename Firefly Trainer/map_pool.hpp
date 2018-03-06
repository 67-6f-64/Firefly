#pragma once

#include "map_data.hpp"
#include "portal_data.hpp"

#include <list>
#include <map>
#include <memory>

namespace firefly
{
	class map_pool
	{
		friend std::ostream& operator <<(std::ostream& stream, map_pool const& maps);

		static const int DEFAULT_MAX_RECURSIONS = 250;
		static int MAX_RECURSIONS;

	public:
		map_pool();
		~map_pool();

		void load_items(std::string const& file_path);

		std::vector<std::shared_ptr<portal_data> const> const generate_path(int start, int finish);
		std::vector<std::shared_ptr<map_data> const> const navigate(int start, int finish);
		std::vector<std::shared_ptr<map_data> const> const find_map(std::string query) const;
		
		std::string find_map_name(int map_id);
		unsigned int find_map_id(std::string const& map_name);

		operator std::string const() const;

	private:
		void parse_lines(std::list<std::string>& lines);

		bool recursive_navigate(std::vector<std::shared_ptr<map_data> const>& path,
			std::shared_ptr<map_data> const& node, std::shared_ptr<map_data> const& goal, int recursions = 0);

	private:
		std::map<int, std::shared_ptr<map_data>> maps_;
	};
}