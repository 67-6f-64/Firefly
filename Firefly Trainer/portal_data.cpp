#include "portal_data.hpp"

#include <algorithm>
#include <map>
#include <iostream>
#include <sstream>

namespace firefly
{
	std::ostream& operator <<(std::ostream& stream, portal_data const& portal)
	{
		stream << static_cast<std::string>(portal);
		return stream;
	}

	portal_data::portal_data(int id, int map_id, std::vector<std::string>& lines)
		: id_(id), map_id_(map_id)
	{
		std::map<std::string, std::string> portal_information;

		std::for_each(lines.begin(), lines.end(), [&](std::string& line)
		{
			std::size_t offset = line.find_last_of('=');
			std::string type = std::string(line, 0, offset);
			std::string data = std::string(line, offset + 1);

			std::size_t type_offset = type.find_last_of('_');
			std::string type_token = std::string(type, type_offset + 1);

			portal_information[type_token] = data;
		});

		this->to_map_ = std::stoi(portal_information["TOMAP"], 0, 10);

		this->from_ = portal_information["FROM"];
		this->to_ = portal_information["TO"];

		this->type_ = static_cast<portal_type>(std::stoi(portal_information["TYPE"], 0, 10));
		this->position_(std::stoi(portal_information["X"], 0, 10), std::stoi(portal_information["Y"], 0, 10));
	}

	portal_data::~portal_data()
	{

	}

	bool portal_data::good() const
	{
		if (this->to_map_ != this->map_id_)
		{
			if ((this->type_ >= INVISIBLE && this->type_ <= AUTOMATIC) || 
				(this->type_ >= NORMAL_ALT && this->type_ <= HIDDEN_ALT))
				return true;
		}

		return false;
	}

	int portal_data::id() const
	{
		return this->id_;
	}

	int portal_data::map_id() const
	{
		return this->map_id_;
	}

	int portal_data::to_map() const
	{
		return this->to_map_;
	}

	std::string const& portal_data::from() const
	{
		return this->from_;
	}

	std::string const& portal_data::to() const
	{
		return this->to_;
	}

	portal_data::portal_type portal_data::type() const
	{
		return this->type_;
	}

	point const& portal_data::position() const
	{
		return this->position_;
	}

	portal_data::operator std::string const() const
	{
		std::stringstream ss;
		ss << "#" << this->id_ << " @ " << this->position_ << " -> " << this->to_map_;
		return ss.str();
	}
}