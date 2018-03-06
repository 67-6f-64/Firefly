#pragma once

#include "native.hpp"

namespace firefly
{
	void set_blackcipher_spy(unsigned char* data, unsigned int size);
	
	bool initialize_ngs_bypass();
	void uninitialize_ngs_bypass();
}