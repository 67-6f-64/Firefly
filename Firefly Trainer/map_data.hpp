#pragma once

#include "portal_data.hpp"

#include <memory>
#include <string>
#include <vector>

namespace firefly
{
	class map_data
	{
		friend std::ostream& operator <<(std::ostream& stream, map_data const& map);

	public:
		static const int MINIMUM_VALID_SIZE = 5; // minimum number of lines for an entry to be valid

		map_data(std::vector<std::string>& lines);
		~map_data();

		int id() const;

		std::string const& region() const;
		std::string const& streetname() const;
		std::string const& name() const;

		std::vector<std::shared_ptr<portal_data> const> const& portals() const;

		operator std::string const() const;

	private:
		int id_;

		std::string region_;
		std::string streetname_;
		std::string name_;

		std::vector<std::shared_ptr<portal_data> const> portals_;
	};
}