#include "basic_types.hpp"

namespace firefly
{
	std::ostream& operator <<(std::ostream& stream, point const& pt)
	{
		stream << static_cast<std::string>(pt);
		return stream;
	}

	/* point */
	point::point()
		: x(0), y(0)
	{

	}

	point::point(int x, int y)
		: x(x), y(y)
	{

	}

	void point::operator ()(int x, int y)
	{
		this->x = x;
		this->y = y;
	}

	point::operator std::string const() const
	{
		std::stringstream ss;
		ss << "(" << this->x << ", " << this->y << ")";
		return ss.str();
	}

	/* size */
	size::size()
		: width(0), height(0)
	{

	}

	size::size(int width, int height)
		: width(width), height(height)
	{

	}

	/* rectangle */
	rectangle::rectangle()
		: point(0, 0), size(0, 0)
	{

	}

	rectangle::rectangle(int x, int y, int width, int height)
		: point(x, y), size(width, height)
	{

	}
}