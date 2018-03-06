#pragma once

#include "native.hpp"
#include "generic_hook_data.hpp"

#include <string>

namespace firefly
{
	template <typename T>
	class ZXString
	{
		struct _ZXStringData
		{
			int nRef;
			int nCap;
			int nByteLen;
		};

	public:
		ZXString()
		{
			this->_m_pStr = static_cast<T>(0);
		}
	
		bool IsEmpty()
		{
			return (!this->_m_pStr || !*this->_m_pStr);
		}

		std::size_t GetLength()
		{
			if (this->_m_pStr)
				return reinterpret_cast<_ZXStringData*>(reinterpret_cast<unsigned char*>(this->_m_pStr) - sizeof(_ZXStringData))->nByteLen;

			return 0;
		}

		void SetText(std::string const& text)
		{
			return ZXString__Assign(this, nullptr, text.c_str(), -1);
		}

		operator const T*() const
		{
			return this->_m_pStr;
		}

	private:	
		T* _m_pStr;
	};

	static_assert_size(sizeof(ZXString<char>), 0x04);
	//static_assert_size(sizeof(ZXString<char>::_ZXStringData), 0x0C);
}