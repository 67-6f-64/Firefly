#pragma once

#include "native.hpp"

namespace firefly
{
	template <typename T>
	class ZArray
	{
	public:
		ZArray()
		{
			/* TODO: code constructor */	
		}
		
		unsigned int GetCount()
		{
			if (!this->a)
				return 0;

			return *(reinterpret_cast<unsigned int*>(this->a) - 1);
		}

		bool IsEmpty()
		{
			return (this->GetCount() == 0);
		}
		
		T& operator[](int index)
		{
			return this->a[index];
		}

		template <typename U>
		U at(unsigned int index)
		{
			return *reinterpret_cast<U*>(reinterpret_cast<unsigned char*>(this->a) + index);
		}

		T* buffer_at(unsigned int index)
		{
			return reinterpret_cast<T*>(reinterpret_cast<unsigned char*>(this->a) + index);
		}

		template <typename U>
		U fetch(unsigned int* index, bool increment = true)
		{
			U result = this->at<U>(*index);

			if (increment)
				*index += sizeof(U);

			return result;
		}

	private:
		T* a;
	};
	
	static_assert_size(sizeof(ZArray<unsigned int>), 0x04);
}