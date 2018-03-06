#include "windows_hooks.hpp"
#include "detours.hpp"

#include "ngs_bypass.hpp"
#include "game_bypass.hpp"
#include "nexon_api.hpp"

#include "network_client.hpp"

#include "autobotting_hooks.hpp"
#include "autologin_hooks.hpp"
#include "exclusive_hooks.hpp"
#include "generic_hooks.hpp"
#include "hack_hooks.hpp"
#include "itemfilter_hooks.hpp"
#include "kami_hooks.hpp"
#include "spawncontrol_hooks.hpp"

#include "autologin_data.hpp"

#include "output.hpp"

#include <Psapi.h>
#pragma comment(lib, "psapi")

#include <Iphlpapi.h>
#pragma comment(lib, "iphlpapi")
#pragma comment(lib, "netapi32")

#ifdef _PRODUCTION
#include <VMProtectSDK.h>
#endif

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

namespace firefly
{
	bool is_address_from_maplestory(void* address)
	{
		MODULEINFO module_info; 
		memset(&module_info, 0, sizeof(module_info));

		if (GetModuleInformation(GetCurrentProcess(), GetModuleHandle(NULL), &module_info, sizeof(module_info))) 
		{
			if (address > module_info.lpBaseOfDll && reinterpret_cast<unsigned char*>(address) < reinterpret_cast<unsigned char*>(module_info.lpBaseOfDll) + module_info.SizeOfImage)
				return true;
		}

		return false;
	}

	HWND get_maplestory_window_handle()
	{
		for (HWND hwnd = GetTopWindow(NULL); hwnd != NULL; hwnd = GetNextWindow(hwnd, GW_HWNDNEXT))
		{
			unsigned long window_process_id = 0;
			GetWindowThreadProcessId(hwnd, &window_process_id);

			if (window_process_id != GetCurrentProcessId())
				continue;

			char class_name[256];
			memset(class_name, 0, sizeof(class_name));

			if (GetClassName(hwnd, class_name, sizeof(class_name)))
			{
				if (!strcmp(class_name, "MapleStoryClass"))
					return hwnd;
			}
		}

		return NULL;
	}
	
#ifdef _PRODUCTION
#pragma optimize("g", off)
#endif
	bool Hook_CreateThread()
	{
		static decltype(&CreateThread) _CreateThread = CreateThread;

		decltype(&CreateThread) CreateThread_hook = [](LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId) -> HANDLE
		{
#ifdef _PRODUCTION
			VMProtectBeginVirtualization("CreateThread_hook");
#endif
			/* Pre optimization-switch */
			typedef struct _tiddata
			{
				unsigned long _tid;	/* thread ID */
				uintptr_t _thandle;	/* thread handle */

				padding_sub(0x54, sizeof(unsigned long) + sizeof(uintptr_t));

				/* following fields are needed by _beginthread code */
				void* _initaddr;	/* initial user thread address */
				void* _initarg;		/* initial user thread argument */
			} _tiddata, *_ptiddata;

			/* Post optimization-switch */
			typedef struct _zthreaddata
			{
				void* _initaddr;
				void** _initarg;
			} _zthreaddata;

			if (is_address_from_maplestory(_ReturnAddress()))
			{
				/* CLoadClientDataThread inherits from ZThread, which uses _beginthreadex to execute ZThread::_ZTE in a new thread (which calls ZThread::Run from vtable) */
				static LPTHREAD_START_ROUTINE ZThread__ZTE = reinterpret_cast<LPTHREAD_START_ROUTINE>(reinterpret_cast<_zthreaddata*>(lpParameter)->_initaddr);

				LPTHREAD_START_ROUTINE ZThread__ZTE_hook = [](void* pVoid) -> DWORD
				{
					DWORD ret = ZThread__ZTE(pVoid);

					/* ZThread::_ZTE has finished executing ZThread::Run (which means all game data has been loaded) */
					PostMessage(get_maplestory_window_handle(), WM_USER + 0xBEEF, 0, 0);

					return ret;
				};
				
				detours::redirect(true, reinterpret_cast<void**>(&ZThread__ZTE), ZThread__ZTE_hook);
			}
		
			return _CreateThread(lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, lpThreadId);	
#ifdef _PRODUCTION
			VMProtectEnd();
#endif
		};

		return detours::redirect(true, reinterpret_cast<void**>(&_CreateThread), CreateThread_hook);
	}

	bool Hook_RegCreateKeyEx()
	{
		static decltype(&RegCreateKeyEx) _RegCreateKeyEx = RegCreateKeyEx;

		decltype(&RegCreateKeyEx) RegCreateKeyEx_hook = [](HKEY hKey, LPCSTR lpSubKey, DWORD Reserved, LPSTR lpClass, DWORD dwOptions, REGSAM samDesired, LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition) -> LSTATUS
		{
			static bool initialized = false;

			if (!initialized)
			{
				HMODULE hModule = NULL;
				
				if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, reinterpret_cast<LPCSTR>(_ReturnAddress()), &hModule) && hModule == GetModuleHandle(NULL))
				{
					set_force_splash_screen(true);
					initialize_nexon_api_hooks();
					initialized = true;
				}
			}

			return _RegCreateKeyEx(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);	
		};

		return detours::redirect(true, reinterpret_cast<void**>(&_RegCreateKeyEx), RegCreateKeyEx_hook);
	}

	void Hook_StartUpDlgClass_WndProc(const WNDCLASSEX* lpwcx)
	{
		static WNDPROC StartUpDlgClass_WndProc = nullptr;

		static WNDPROC StartUpDlgClass_WndProc_hook = [](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
#ifdef _PRODUCTION
			VMProtectBeginVirtualization("StartUpDlgClass_WndProc_hook");
#endif
			switch (uMsg)
			{
			case WM_CREATE:
				{
					set_force_splash_screen(false);

					initialize_game_image();
					//initialize_game_bypass();

					initialize_ngs_bypass();

					network_client::get_instance().initialize(reinterpret_cast<HMODULE>(&__ImageBase));
					
					//set_generic_hooks(true);
					//set_autobotting_hooks(true);
					//set_autologin_hooks(true);
					////set_kami_hooks(true);
					//set_hack_hooks(true);
					//set_itemfilter_hooks(true);
					//set_spawncontrol_hooks(true);
					//set_exclusive_hooks(true);
				}
				break;

			case WM_COPYDATA:
				{
					COPYDATASTRUCT* copy_data = reinterpret_cast<COPYDATASTRUCT*>(lParam);
					network_client::get_instance().connect_client(static_cast<unsigned short>(copy_data->dwData));
					set_blackcipher_spy(reinterpret_cast<unsigned char*>(copy_data->lpData), copy_data->cbData);
					DestroyWindow(hWnd);
				}
				break;

			default:
				break;
			}

			return StartUpDlgClass_WndProc(hWnd, uMsg, wParam, lParam);
#ifdef _PRODUCTION
			VMProtectEnd();
#endif
		};

		StartUpDlgClass_WndProc = lpwcx->lpfnWndProc;
		const_cast<WNDCLASSEX*>(lpwcx)->lpfnWndProc = StartUpDlgClass_WndProc_hook;
	}
	
	void Hook_MapleStoryClass_WndProc(const WNDCLASSEX* lpwcx)
	{
		static WNDPROC MapleStoryClass_WndProc = nullptr;

		static WNDPROC MapleStoryClass_WndProc_hook = [](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
#ifdef _PRODUCTION
			VMProtectBeginVirtualization("MapleStoryClass_WndProc_hook");
#endif
			switch (uMsg)
			{
			case WM_CREATE:
				{
					SetWindowText(hWnd, std::string("MapleStory | PID: " + std::to_string(GetCurrentProcessId())).c_str());
					SetWindowPos(hWnd, NULL, 0 + GetSystemMetrics(SM_CXSIZEFRAME), 0 + GetSystemMetrics(SM_CYSIZEFRAME), 0, 0, SWP_NOSIZE | SWP_NOZORDER);
					Hook_CreateThread();
				}
				break;
				
			case WM_USER + 0xDEAD:
				//try_select_region(reinterpret_cast<void*>(lParam));
				break;

			case WM_USER + 0xBEEF:
				//try_select_world_channel();
				break;

			case WM_DESTROY:
				uninitialize_ngs_bypass();
				break;

			default:
				break;
			}

			return MapleStoryClass_WndProc(hWnd, uMsg, wParam, lParam);
#ifdef _PRODUCTION
			VMProtectEnd();
#endif
		};

		MessageBox(0,0,0,0);

		MapleStoryClass_WndProc = lpwcx->lpfnWndProc;
		const_cast<WNDCLASSEX*>(lpwcx)->lpfnWndProc = MapleStoryClass_WndProc_hook;
	}
#ifdef _PRODUCTION
#pragma optimize("g", on)
#endif
	
	bool Hook_RegisterClassExA()
	{
		static decltype(&RegisterClassExA) _RegisterClassExA = RegisterClassExA;

		decltype(&RegisterClassExA) RegisterClassExA_hook = [](const WNDCLASSEX* lpwcx) -> ATOM
		{
			if (lpwcx->lpszClassName)
			{
				if (!strcmp(lpwcx->lpszClassName, "StartUpDlgClass"))
					Hook_StartUpDlgClass_WndProc(lpwcx);
				else if (!strcmp(lpwcx->lpszClassName, "MapleStoryClass"))
					Hook_MapleStoryClass_WndProc(lpwcx);
				else if (!strcmp(lpwcx->lpszClassName, "NexonADBallon"))
					return NULL;
			}

			return _RegisterClassExA(lpwcx);
		};

		return detours::redirect(true, reinterpret_cast<void**>(&_RegisterClassExA), RegisterClassExA_hook);
	}
	
	/* Randomize Hardware ID */
	bool Hook_GetAdaptersInfo()
	{
		static decltype(&GetAdaptersInfo) _GetAdaptersInfo = GetAdaptersInfo;
		
		decltype(&GetAdaptersInfo) GetAdaptersInfo_hook = [](PIP_ADAPTER_INFO AdapterInfo, PULONG SizePointer) -> ULONG
		{
			ULONG error_code = _GetAdaptersInfo(AdapterInfo, SizePointer);
			
			if (autologin_data::get_instance().enabled() && autologin_data::get_instance().randomize_hwid())
			{
				if (error_code == ERROR_SUCCESS && is_address_from_maplestory(_ReturnAddress()))
				{
					for (IP_ADAPTER_INFO* adapter = AdapterInfo; adapter != NULL; adapter = adapter->Next)
					{
						if (adapter->Type == MIB_IF_TYPE_ETHERNET && strncmp(reinterpret_cast<char*>(adapter->Address), "DEST", 6))
						{
							time_t t = time(0);   // get time now
							struct tm * now = localtime(&t);

							printf("[%d:%d:%d - %08X] GetAdaptersInfo adapter:", now->tm_hour, now->tm_min, now->tm_sec, _ReturnAddress());

							for (int i = 0; i < sizeof(adapter->Address); i++)
								printf(" %02X", adapter->Address[i]);

							printf("\n");
						}
					}
				}
			}

			return error_code;
		};

		return detours::redirect(true, reinterpret_cast<void**>(&_GetAdaptersInfo), GetAdaptersInfo_hook);
	}
	
	bool Hook_Netbios()
	{
		static decltype(&Netbios) _Netbios = Netbios;
		
		decltype(&Netbios) Netbios_hook = [](PNCB pncb) -> UCHAR
		{
			UCHAR error_code = _Netbios(pncb);
			
			if (autologin_data::get_instance().enabled() && autologin_data::get_instance().randomize_hwid())
			{
				if (error_code == NRC_GOODRET && pncb->ncb_command == NCBASTAT && is_address_from_maplestory(_ReturnAddress()))
				{
					typedef struct _ASTAT_
					{
						ADAPTER_STATUS adapter_status;
						NAME_BUFFER name_buffer[30];
					} ASTAT;

					ASTAT* adapter = reinterpret_cast<ASTAT*>(pncb->ncb_buffer);

					/* 0xFE == Ethernet adapter, 0xFF == Token Ring adapter */
					if (adapter->adapter_status.adapter_type == 0xFE || adapter->adapter_status.adapter_type == 0xFF)
					{
						time_t t = time(0);   // get time now
						struct tm * now = localtime(&t);

						printf("[%d:%d:%d - %08X] Netbios Adapter:", now->tm_hour, now->tm_min, now->tm_sec, _ReturnAddress());

						for (int i = 0; i < sizeof(adapter->adapter_status.adapter_address); i++)
							printf(" %02X", adapter->adapter_status.adapter_address[i]);

						printf("\n");
					}
				}
			}

			return error_code;
		};

		return detours::redirect(true, reinterpret_cast<void**>(&_Netbios), Netbios_hook);
	}
	
	bool Hook_getsockname()
	{
		static decltype(&getsockname) _getsockname = getsockname;
		
		decltype(&getsockname) getsockname_hook = [](SOCKET s, sockaddr* name, int* namelen) -> int
		{
			int ret = _getsockname(s, name, namelen);

			if (autologin_data::get_instance().enabled() && autologin_data::get_instance().randomize_hwid())
			{
				if (ret == 0 && *namelen == sizeof(sockaddr_in) && is_address_from_maplestory(_ReturnAddress()))
				{
					time_t t = time(0);   // get time now
					struct tm * now = localtime(&t);

					unsigned char* p = reinterpret_cast<unsigned char*>(&reinterpret_cast<sockaddr_in*>(name)->sin_addr.S_un.S_addr);
					printf("[%d:%d:%d - %08X] IP: %d.%d.%d.%d\n", now->tm_hour, now->tm_min, now->tm_sec, _ReturnAddress(), p[0], p[1], p[2], p[3]);
				}
			}

			return ret;
		};

		return detours::redirect(true, reinterpret_cast<void**>(&_getsockname), getsockname_hook);
	}
	
	bool Hook_GetVolumeInformation()
	{
		static decltype(&GetVolumeInformation) _GetVolumeInformation = GetVolumeInformation;
		
		decltype(&GetVolumeInformation) GetVolumeInformation_hook = [](LPCSTR lpRootPathName, LPSTR lpVolumeNameBuffer, DWORD nVolumeNameSize, LPDWORD lpVolumeSerialNumber,
			LPDWORD lpMaximumComponentLength, LPDWORD lpFileSystemFlags, LPSTR lpFileSystemNameBuffer, DWORD nFileSystemNameSize) -> BOOL
		{
			BOOL ret = _GetVolumeInformation(lpRootPathName, lpVolumeNameBuffer, nVolumeNameSize, lpVolumeSerialNumber,	lpMaximumComponentLength, lpFileSystemFlags, lpFileSystemNameBuffer, nFileSystemNameSize);
			
			if (autologin_data::get_instance().enabled() && autologin_data::get_instance().randomize_hwid())
			{
				if (ret == TRUE && is_address_from_maplestory(_ReturnAddress()))
				{
					char root_folder[MAX_PATH];
					memset(root_folder, 0, sizeof(root_folder));

					if (GetWindowsDirectory(root_folder, MAX_PATH))
					{
						*(strchr(root_folder, '\\') + 1) = 0;

						time_t t = time(0);   // get time now
						struct tm * now = localtime(&t);

						if (!strcmp(lpRootPathName, root_folder))
							printf("[%d:%d:%d - %08X] HDD Serial: %08X\n", now->tm_hour, now->tm_min, now->tm_sec, _ReturnAddress(), *lpVolumeSerialNumber);
					}
				}
			}

			return ret;
		};

		return detours::redirect(true, reinterpret_cast<void**>(&_GetVolumeInformation), GetVolumeInformation_hook);
	}

	void initialize_windows_hooks(HMODULE module)
	{
		Hook_RegCreateKeyEx();
		Hook_RegisterClassExA();

		Hook_GetAdaptersInfo();
		Hook_Netbios();
		Hook_getsockname();
		Hook_GetVolumeInformation();
	}
}