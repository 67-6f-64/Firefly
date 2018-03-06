#pragma once

#include "native.hpp"

namespace firefly
{
	struct __POSITION
	{
		char dummy[1];
	};

	template <class key_type, class value_type, class pair_type = key_type>
	class ZMap
	{
		virtual ~ZMap() = default;

	public:
		value_type* GetAt(key_type const& key, value_type* value)
		{
			if (!this->_m_apTable)
				return nullptr;

			_PAIR* pair = this->_m_apTable[this->CalculateDigest(key) % this->_m_uTableSize];

			if (!pair)
				return nullptr;

			while (pair->key != key)
			{
				pair = pair->pNext;

				if (!pair) 
					return nullptr;
			}

			if (value)
				*value = pair->value;

			return &pair->value;
		}
			
		value_type* get_at(const key_type& key, value_type& value)
		{
			if (ap_table_ != nullptr)
			{
				PAIR* pair = ap_table_[get_hash(key) % table_size_];
				if (pair != nullptr)
				{
					while (pair->key_ != key)
					{
						pair = pair->next_;
						if (pair == nullptr) return nullptr;
					}

					value = pair->value_;
				}
			}
			return &value;
		}

		unsigned int GetCount()
		{
			return this->_m_uCount;
		}
		
	private:
		template <typename T>
		unsigned int CalculateDigest(T const& key) const
		{
			return _rotr(key, 5);
		}

	private:
		struct _PAIR // ZMap<A, B, C>::_PAIR
		{
			unsigned char gap0[4];
			_PAIR* pNext;
			key_type key;
			value_type value;
		} ** _m_apTable;

		unsigned int _m_uTableSize;
		unsigned int _m_uCount;
		unsigned int _m_uAutoGrowEvery128;
		unsigned int _m_uAutoGrowLimit;
	};
	
	static_assert_size(sizeof(ZMap<void*, void*, void*>), 0x18);
	//static_assert_size(sizeof(ZMap<void*, void*, void*>::_PAIR), 0x10);
}