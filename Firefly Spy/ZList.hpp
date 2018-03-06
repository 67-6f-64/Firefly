#pragma once

#include "native.hpp"

#include <functional>

namespace firefly
{
	template <typename T>
	struct ZListData
	{
		padding(0x04);
		ZListData<T>* pNext;
		ZListData<T>* pPrev;
		padding(0x08);
		T data;					// <- ZList<T>::GetHead() points to this.
	};

	template <typename T>
	class ZList
	{
		virtual ~ZList() = default;

	public:
		unsigned int GetCount()
		{
			return this->_m_uCount;
		}

		T* GetHead()
		{
			return this->_m_pHead;
		}

		T* GetTail()
		{
			return this->_m_pTail;
		}

		T* GetNext(T** pos)
		{
			T* current = reinterpret_cast<T*>(*pos);
		
			std::size_t struct_size = (sizeof(ZListData<T>) - sizeof(T));
			ZListData<T>* list_data = reinterpret_cast<ZListData<T>*>(reinterpret_cast<unsigned char*>(*pos) - struct_size);
			*pos = reinterpret_cast<T*>(reinterpret_cast<unsigned char*>(list_data->pNext) + struct_size);

			return current;
		}

		T* GetPrev(T** pos)
		{
			T* current = reinterpret_cast<T*>(*pos);
		
			std::size_t struct_size = (sizeof(ZListData<T>) - sizeof(T));
			ZListData<T>* list_data = reinterpret_cast<ZListData<T>*>(reinterpret_cast<unsigned char*>(*pos) - struct_size);
			*pos = reinterpret_cast<T*>(reinterpret_cast<unsigned char*>(list_data->pPrev) + struct_size);

			return current;
		}

		void EnumerateList(std::function<bool(T*)> enum_func = [](T*) -> bool { return true; })
		{
			if (this->GetCount() > 0)
			{
				T* pos = this->GetHead();

				for (unsigned int i = 0, count = this->GetCount(); i < count; i++)
				{
					T* current = this->GetNext(&pos);

					if (current)
					{
						if (enum_func(current))
							break;
					}
				}
			}
		}

	private:
		void* baseclases;
		unsigned int _m_uCount;
		T* _m_pHead;
		T* _m_pTail;
	};
	
	static_assert_size(sizeof(ZListData<void*>), 0x18);
	static_assert_size(sizeof(ZList<void*>), 0x14);
}