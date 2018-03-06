#pragma once

#include "native.hpp"
#include "addresses.hpp"

#include "generic_objects.hpp"
#include "TSingleton.hpp"

namespace firefly
{
	#define VK_A 0x41
	#define VK_S 0x53
	#define VK_D 0x44
	#define VK_F 0x46
	
	class CWndMan : public CWnd, public TSingleton<CWndMan, TSingleton_CWndMan>
	{
		~CWndMan() = delete;

	public:
		void send_key(unsigned int vkey, bool press_up = true)
		{
			unsigned int repeatcount = 1;
			unsigned int scancode = (MapVirtualKey(vkey, MAPVK_VK_TO_VSC) << 16);
			unsigned int extendedkey = ((is_extend_key(vkey) ? 1 : 0) << 24);

			this->OnKey(vkey, repeatcount | scancode | extendedkey);

			if (press_up)
			{
				unsigned int transition = (1 << 31);
				this->OnKey(vkey, repeatcount | scancode | extendedkey | transition);
			}
		}
		
	private:
		bool is_extend_key(unsigned int vkey)
		{
			return (vkey == VK_INSERT ||	// Insert
				vkey == VK_DELETE ||		// Delete
				vkey == VK_HOME ||			// Home
				vkey == VK_END ||			// End
				vkey == VK_PRIOR ||			// Page Up
				vkey == VK_NEXT ||			// Page Down
				vkey == VK_UP ||			// Arrow Up
				vkey == VK_DOWN);			// Arrow Down
		}
	};

	class CFuncKeyMappedMan : public TSingleton<CFuncKeyMappedMan, TSingleton_CFuncKeyMappedMan>
	{
		~CFuncKeyMappedMan() = delete;

	public:
		unsigned int func_key_mapped(unsigned int vkey)
		{
			FUNCKEY_MAPPED* func_key = CFuncKeyMappedMan__FuncKeyMapped(this, nullptr, MapVirtualKey(vkey, MAPVK_VK_TO_VSC));
			return (func_key ? func_key->id() : 0);
		}
	};

	static_assert_size(sizeof(CWndMan), sizeof(CWnd));
	static_assert_size(sizeof(CFuncKeyMappedMan), 0x01);
}