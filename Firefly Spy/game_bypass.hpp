#pragma once

#include "native.hpp"

namespace firefly
{
	unsigned char* get_game_image_copy(unsigned int* image_size);

	void initialize_game_image();
	void initialize_game_bypass();
}