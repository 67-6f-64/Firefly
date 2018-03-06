#pragma once

#include "native.hpp"

namespace firefly
{
	namespace pointer
	{
		namespace page
		{
			inline bool is_readable(void* address)
			{
				if (!address)
					return false;

				MEMORY_BASIC_INFORMATION mbi;
				memset(&mbi, 0, sizeof(MEMORY_BASIC_INFORMATION));

				if (VirtualQuery(address, &mbi, sizeof(MEMORY_BASIC_INFORMATION)) == sizeof(MEMORY_BASIC_INFORMATION))
				{
					if (!mbi.Protect || (mbi.Protect & PAGE_GUARD))
						return false;

					if ((mbi.Protect & PAGE_EXECUTE_READ) || (mbi.Protect & PAGE_EXECUTE_READWRITE) || (mbi.Protect & PAGE_READONLY) || (mbi.Protect & PAGE_READWRITE))
						return true;
				}

				return false;
			}
		
			inline bool is_writeable(void* address)
			{
				if (!address)
					return false;

				MEMORY_BASIC_INFORMATION mbi;
				memset(&mbi, 0, sizeof(MEMORY_BASIC_INFORMATION));

				if (VirtualQuery(address, &mbi, sizeof(MEMORY_BASIC_INFORMATION)) == sizeof(MEMORY_BASIC_INFORMATION))
				{
					if (!mbi.Protect || (mbi.Protect & PAGE_GUARD))
						return false;
				
					if ((mbi.Protect & PAGE_EXECUTE_WRITECOPY) || (mbi.Protect & PAGE_EXECUTE_READWRITE) ||	(mbi.Protect & PAGE_WRITECOPY) || (mbi.Protect & PAGE_READWRITE))
						return true;
				}

				return false;
			}
		}

		template <typename T>
		bool read(T* data, void* base, std::size_t offset)
		{
			if (!page::is_readable(base))
				return false;

			unsigned char* pointer = *reinterpret_cast<unsigned char**>(base) + offset;
			
			if (page::is_readable(pointer))
			{
				memcpy(data, pointer, sizeof(T));
				return true;
			}

			return false;
		}

		template <typename T>
		bool vread(T* data, void* base, std::size_t offsets, ...)
		{
			if (!page::is_readable(base))
				return false;

			va_list va;
			va_start(va, offsets);

			unsigned char* pointer = reinterpret_cast<unsigned char*>(base);

			for (size_t i = 0; i < offsets; i++)
			{
				if (!page::is_readable(pointer))
				{
					va_end(va);
					return false;
				}

				pointer = *reinterpret_cast<unsigned char**>(pointer) + va_arg(va, std::size_t);
			}

			va_end(va);

			if (page::is_readable(pointer))
			{
				memcpy(data, pointer, sizeof(T));
				return true;
			}

			return false;
		}

		template <typename T>
		bool write(T data, void* base, std::size_t offset)
		{
			if (!page::is_readable(base))
				return false;

			unsigned char* pointer = *reinterpret_cast<unsigned char**>(base) + offset;
			
			if (page::is_writeable(pointer))
			{
				memcpy(pointer, &data, sizeof(T));
				return true;
			}

			return false;
		}

		template <typename T>
		bool vwrite(T data, void* base, std::size_t offsets, ...)
		{
			if (!page::is_readable(base))
			{
				return false;
			}

			va_list va;
			va_start(va, offsets);
		
			unsigned char* pointer = reinterpret_cast<unsigned char*>(base);

			for (size_t i = 0; i < offsets; i++)
			{
				if (!page::is_readable(pointer))
				{
					va_end(va);
					return false;
				}

				pointer = *reinterpret_cast<unsigned char**>(pointer) + va_arg(va, std::size_t);
			}

			va_end(va);

			if (page::is_writeable(pointer))
			{
				memcpy(pointer, &data, sizeof(T));
				return true;
			}

			return false;
		}
	}
}