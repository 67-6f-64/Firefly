#pragma once

#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#ifdef _UNICODE
#undef _UNICODE
#endif

#ifdef UNICODE
#undef UNICODE
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef _STL_SECURE_NO_WARNINGS
#define _STL_SECURE_NO_WARNINGS
#endif

#define CUSTOM_NOTIFY		(WM_USER + 1)
#define CUSTOM_COMMAND		(WM_USER + 2)
#define CUSTOM_CTLCOLOR		(WM_USER + 3)
#define	OWNER_MEASUREITEM	(WM_USER + 4)
#define OWNER_DRAWITEM		(WM_USER + 5)

#define WM_SOCKET			(WM_USER + 10)

#define WM_GAME_SOCKET		(WM_USER + 10)
#define WM_MANAGER_SOCKET	(WM_USER + 11)

#include <Windows.h>
#include <Windowsx.h>

#include <CommCtrl.h>

#ifndef padding
#define padding(x) struct { unsigned char __padding##x[(x)]; };
#endif

#ifndef padding_add
#define padding_add(x, y) struct { unsigned char __padding##x[(x) + (y)]; };
#endif

#ifndef padding_sub
#define padding_sub(x, y) struct { unsigned char __padding##x[(x) - (y)]; };
#endif

#ifndef static_assert_size
#define static_assert_size(actual, expected) \
	static_assert((actual) == (expected), "Size assertion failed: " #actual " != " #expected ".");
#endif

#ifndef static_assert_offset
#define static_assert_offset(type, member, expected) \
	static_assert(offsetof((type), (member)) == (expected), "Offset assertion failed: " offsetof((type), (member)) " != " #expected ".");
#endif