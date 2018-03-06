#include "map_data.hpp"

#include <algorithm>
#include <map>

namespace firefly
{
	std::ostream& operator <<(std::ostream& stream, map_data const& map)
	{
		stream << static_cast<std::string>(map);
		return stream;
	}

	map_data::map_data(std::vector<std::string>& lines)
	{
		std::string id(*lines.begin());
		id.erase(id.begin());
		id.erase(id.end() - 1);

		this->id_ = std::stoi(id, 0, 10);

		lines.erase(lines.begin());

		std::map<std::string, std::string> map_information;

		std::for_each(lines.begin(), lines.end(), [&](std::string& line)
		{
			std::size_t offset = line.find_last_of('=');
			std::string type = std::string(line, 0, offset);
			std::string data = std::string(line, offset + 1);

			map_information[type] = data;
		});

		this->region_ = map_information["Region"];
		this->streetname_ = map_information["StreetName"];
		this->name_ = map_information["Name"];

		for (int i = 0, portals = std::stoi(map_information["Portal_Count"], 0, 10); i < portals; i++)
		{
			const std::string prefix("Portal_" + std::to_string(i) + "_");

			std::vector<std::string> portal_lines;

			std::remove_if(lines.begin(), lines.end(), [&](std::string& line) -> bool
			{
				if (!line.compare(0, prefix.size(), prefix))
				{
					portal_lines.push_back(line);
					return true;
				}

				return false;
			});

			this->portals_.push_back(std::make_shared<portal_data>(i, this->id_, portal_lines));
		}
	}

	map_data::~map_data()
	{

	}

	int map_data::id() const
	{
		return this->id_;
	}

	std::string const& map_data::region() const
	{
		return this->region_;
	}

	std::string const& map_data::streetname() const
	{
		return this->streetname_;
	}

	std::string const& map_data::name() const
	{
		return this->name_;
	}

	std::vector<std::shared_ptr<portal_data> const> const& map_data::portals() const
	{
		return this->portals_;
	}

	map_data::operator std::string const() const
	{
		std::stringstream ss;
		ss << this->id_ << ": " << this->name_.c_str();
		return ss.str();
	}
}