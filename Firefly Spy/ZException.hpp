#pragma once

#include "native.hpp"

namespace firefly
{
	class ZException
	{
	public:
		ZException(HRESULT hr)
			: m_hr(hr)
		{

		}

		HRESULT Error()
		{
			return this->m_hr;
		}

	private:
		const HRESULT m_hr;
	};

	static_assert_size(sizeof(ZException), 0x04);
}