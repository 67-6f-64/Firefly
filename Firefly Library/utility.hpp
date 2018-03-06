#pragma once

#include "native.hpp"

#include <string>

namespace firefly
{
	namespace utility
	{
		__forceinline void __declspec(naked) invoke_crash()
		{
			__asm
			{
				xor ebx,ebx
				xor edx,edx
				xor esi,esi
				xor edi,edi
				xor ebp,ebp

				mov eax,fs:[0x18]		// TEB
				mov ecx,[eax+0x08]		// Stack Limit (Bottom)
				mov eax,[eax+0x04]		// Stack Base (Ceiling) 

				null_stack:
				cmp eax,ecx
				jbe complete_crash
				sub eax,0x04
				mov [eax],ebx
				jmp null_stack
					
				complete_crash:
				xor eax,eax
				xor ecx,ecx
				ret
			}
		}

		std::string upper(std::string const& string);
		std::string lower(std::string const& string);

		template<class T>
		std::string delimited_numeric(T value)
		{
			std::stringstream ss;
			ss.imbue(std::locale(""));
			ss << std::fixed << value;
			return ss.str();
		}
	}
}