#include "utility.hpp"

#include <algorithm>

namespace firefly
{
	namespace utility
	{
		std::string upper(std::string const& string)
		{
			std::string upper_string;
			std::transform(string.begin(), string.end(), std::back_inserter(upper_string), toupper);
			return upper_string;
		}

		std::string lower(std::string const& string)
		{
			std::string lower_string;
			std::transform(string.begin(), string.end(), std::back_inserter(lower_string), tolower);
			return lower_string;
		}
	}
}