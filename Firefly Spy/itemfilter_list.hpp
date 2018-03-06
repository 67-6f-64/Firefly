#pragma once

#include "native.hpp"

#include <set>

namespace firefly
{
	class itemfilter_list
	{
	public:
		itemfilter_list();

		void add(unsigned int id);
		void clear();

		bool exists(unsigned int id);

	private:
		std::set<unsigned int> list_;
	};
}