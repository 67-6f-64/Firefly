#pragma once

#include "native.hpp"

#include <sstream>

namespace firefly
{
	class point
	{
		friend std::ostream& operator <<(std::ostream& stream, point const& pt);

	public:
		point();
		point(int x, int y);

		void operator ()(int x, int y);
		operator std::string const() const;

		int x;
		int y;
	};

	struct size
	{
		size();
		size(int width, int height);

		int width;
		int height;
	};

	struct rectangle : public point, public size
	{
		rectangle();
		rectangle(int x, int y, int width, int height);
	};
}