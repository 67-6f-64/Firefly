#pragma once

namespace firefly
{
	namespace crc32
	{
		unsigned int crc32(unsigned char* data, unsigned int size, unsigned int crc = 0);
	}
}