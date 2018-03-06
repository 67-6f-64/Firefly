#include "map_pool.hpp"
#include "utility.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>

namespace firefly
{
	std::ostream& operator <<(std::ostream& stream, map_pool const& maps)
	{
		stream << static_cast<std::string>(maps);
		return stream;
	}

	int map_pool::MAX_RECURSIONS = map_pool::DEFAULT_MAX_RECURSIONS;

	map_pool::map_pool()
	{

	}

	map_pool::~map_pool()
	{

	}

	void map_pool::load_items(std::string const& file_path)
	{
		std::ifstream file(file_path);
		std::list<std::string> lines;

		while (file.good())
		{
			std::string line;
			std::getline(file, line);
			lines.push_back(line);
		}

		this->parse_lines(lines);
	}
	
	std::vector<std::shared_ptr<portal_data> const> const map_pool::generate_path(int start, int finish)
	{
		std::vector<std::shared_ptr<portal_data> const> result;
		std::vector<std::shared_ptr<map_data> const> const maps = this->navigate(start, finish);

		if (maps.empty())
			return result;

		for (std::size_t i = 0; i < maps.size() - 1; i++)
		{
			int next_map_id = maps[i + 1]->id();
			std::vector<std::shared_ptr<portal_data> const> const& portals = maps[i]->portals();

			std::for_each(portals.begin(), portals.end(), [&](std::shared_ptr<portal_data> const& portal)
			{
				if (portal->to_map() == next_map_id)
					result.push_back(portal);
			});
		}

		return result;
	}

	std::vector<std::shared_ptr<map_data> const> const map_pool::navigate(int start, int finish)
	{
		std::vector<std::shared_ptr<map_data> const> result;

		if (start == finish)
			return result;

		std::shared_ptr<map_data> const begin = this->maps_[start];
		std::shared_ptr<map_data> const end = this->maps_[finish];

		if (!begin || !end)
			return result;

		if (begin->region() != end->region())
			return result;

		this->recursive_navigate(result, begin, end);
		return result;
	}

	std::vector<std::shared_ptr<map_data> const> const map_pool::find_map(std::string query) const
	{
		std::vector<std::shared_ptr<map_data> const> result;

		std::for_each(this->maps_.begin(), this->maps_.end(), [&](std::pair<int, std::shared_ptr<map_data> const> pairing)
		{
			std::shared_ptr<map_data> const& map = pairing.second;

			if (!map)
				return;

			if (utility::upper(static_cast<std::string>(*map.get())).find(utility::upper(query)) != std::string::npos)
				result.push_back(map);
		});

		return result;
	}

	std::string map_pool::find_map_name(int map_id)
	{
		std::shared_ptr<map_data> map = this->maps_[map_id];
		return (map ? map->name() : std::string("N/A"));
	}

	unsigned int map_pool::find_map_id(std::string const& map_name)
	{
		for (std::pair<int, std::shared_ptr<map_data> const> pairing : this->maps_)
		{
			std::shared_ptr<map_data> const& map = pairing.second;

			if (!map)
				return 0;

			if (!utility::upper(static_cast<std::string>(*map.get())).compare(utility::upper(map_name)))
				return map->id();
		}
		
		return 0;
	}

	map_pool::operator std::string const() const
	{
		std::stringstream ss;
		ss << "Maplist [" << this->maps_.size() << "] {...} @ " << this;
		return ss.str();
	}

	void map_pool::parse_lines(std::list<std::string>& lines)
	{
		while (lines.size() >= map_data::MINIMUM_VALID_SIZE)
		{
			static const std::string prefix("[");

			std::vector<std::string> map_lines;

			std::list<std::string>::const_iterator iterator = lines.begin();
			std::list<std::string>::const_iterator end = lines.end();
			
			do
			{
				map_lines.push_back(*iterator++);
			} 
			while (iterator != end && (*iterator).compare(0, prefix.size(), prefix));

			lines.erase(lines.begin(), iterator);

			if (map_lines.size() < map_data::MINIMUM_VALID_SIZE)
				continue;

			try
			{
				std::shared_ptr<map_data> const map(new map_data(map_lines));
				this->maps_[map->id()] = map;
			}
			catch (...) 
			{
				/* catch exception */
			}
		}
	}

	bool map_pool::recursive_navigate(std::vector<std::shared_ptr<map_data> const>& path,
		std::shared_ptr<map_data> const& node, std::shared_ptr<map_data> const& goal, int recursions)
	{
		if (recursions >= MAX_RECURSIONS)
			return false;

		if (node == goal)
		{
			path.push_back(goal); // save endpoint
			return true;
		}

		std::vector<std::shared_ptr<portal_data> const> portals = node->portals();

		if (portals.empty())
			return false; // no portals -> no possible paths

		for (std::vector<std::shared_ptr<portal_data> const>::const_iterator iterator = portals.begin(), end = portals.end(); iterator != end; iterator++)
		{
			std::shared_ptr<portal_data> const& portal = *iterator;

			if (!portal->good())
				continue;

			std::shared_ptr<map_data> const& adjoined = this->maps_[portal->to_map()];

			if (!adjoined) // map not found
				continue;

			if (std::find(path.begin(), path.end(), adjoined) != path.end()) // avoid going in infinite loops
				continue;

			path.push_back(node); // pre-order traversal: push current map onto stack

			if (this->recursive_navigate(path, adjoined, goal, recursions + 1))
				return true; // a path was found
			else
				path.pop_back(); // the candidate map was incorrect, so pop it off the stack
		}

		return false;
	}
}