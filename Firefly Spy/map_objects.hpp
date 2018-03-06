#pragma once

#include "native.hpp"
#include "addresses.hpp"

#include "generic_objects.hpp"
#include "TSingleton.hpp"

namespace firefly
{
	class CUIMiniMap : public CWnd, public TSingleton<CUIMiniMap, TSingleton_CUIMiniMap>
	{
		~CUIMiniMap() = delete;

	public:
		unsigned int get_map_id()
		{
			return this->dwFieldID;
		}

	private:
		padding_sub(TSingleton_CUIMiniMap_dwFieldID_offset, sizeof(CWnd));
		unsigned int dwFieldID;												// 0x147C
	};

	static_assert_size(sizeof(CUIMiniMap), TSingleton_CUIMiniMap_dwFieldID_offset + sizeof(unsigned int));
}