#pragma once

#include "native.hpp"
#include "addresses.hpp"

#include "generic_objects.hpp"

#include "TSingleton.hpp"

namespace firefly
{
	class CWvsPhysicalSpace2D : public TSingleton<CWvsPhysicalSpace2D, TSingleton_CWvsPhysicalSpace2D>
	{
		~CWvsPhysicalSpace2D() = delete;

	public:
		bool IsInRect(POINT pt)
		{
			return (PtInRect(&this->m_rcMBR, pt) != FALSE);
		}

		void GetTopLeft(POINT* pt)
		{
			pt->x = m_rcMBR.left;
			pt->y = m_rcMBR.top;
		}

		void GetBoundary(RECT* rc)
		{
			rc->left = this->m_rcMBR.left;
			rc->top = this->m_rcMBR.top;
			rc->right = this->m_rcMBR.right;
			rc->bottom = this->m_rcMBR.bottom;
		}

	private:
		padding(0x0C);
		RECT m_rcMBR;
	};

	static_assert_size(sizeof(CWvsPhysicalSpace2D), 0x1C);
}