#pragma once

#include "native.hpp"

namespace firefly
{
	template <typename T, int TSingleton_pointer>
	class TSingleton
	{
		TSingleton() = delete;
		~TSingleton() = delete;

	public:
		static T* GetInstance()
		{
			return *reinterpret_cast<T**>(TSingleton_pointer);
		}

		static bool IsInitialized()
		{
			return (GetInstance() != nullptr);
		}
	};

	static_assert_size(sizeof(TSingleton<long, 0x00000000>), 0x01);
}