#pragma once

#include "native.hpp"

namespace firefly
{
	class ZRefCounted
	{
		ZRefCounted() = delete;

	protected:
		virtual ~ZRefCounted() = 0;
 
	private:
		union
		{
			int _m_nRef;
			ZRefCounted* _m_pNext;
		};
 
		ZRefCounted* _m_pPrev;
		void* unknown;
	};

	static_assert_size(sizeof(ZRefCounted), 0x10);
}