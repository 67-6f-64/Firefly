#pragma once

#include "native.hpp"

namespace firefly
{
	template <typename T>
	struct ZRef
	{
	public:
		operator const T*() const
		{
			return this->p;
		}

	public:
		padding(0x04);
		T* p;
	};

	static_assert_size(sizeof(ZRef<void*>), 0x08);
}