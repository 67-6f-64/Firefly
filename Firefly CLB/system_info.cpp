#include "native.hpp"
#include "system_info.hpp"

#include <Iphlpapi.h>

#pragma comment(lib, "iphlpapi")
#pragma comment(lib, "netapi32")

namespace firefly
{
	namespace system_info
	{
		#pragma pack(push, 1)
		typedef struct _machine_info
		{
			unsigned char mac_address[6];	// 0x00
			unsigned int hdd_serial;		// 0x06
			unsigned int zero;				// 0x0A
			unsigned short checksum;		// 0x0E
		} machine_info;
		#pragma pack(pop)

		void get_adapters_info(unsigned char* buffer)
		{
			ULONG size_pointer = 0;
			IP_ADAPTER_INFO* adapter_info = NULL;

			if (GetAdaptersInfo(NULL, &size_pointer) == ERROR_BUFFER_OVERFLOW && size_pointer > 0)
			{
				adapter_info = reinterpret_cast<IP_ADAPTER_INFO*>(malloc(size_pointer));

				if (GetAdaptersInfo(adapter_info, &size_pointer) != ERROR_SUCCESS)
				{
					free(adapter_info);
					adapter_info = NULL;
				}
			}

			if (adapter_info)
			{
				for (IP_ADAPTER_INFO* adapter = adapter_info; adapter != 0; adapter = adapter->Next)
				{
					if (adapter->Type == MIB_IF_TYPE_ETHERNET)
					{
						if (strncmp(reinterpret_cast<char*>(adapter->Address), "DEST", 4) != 0)
						{
							sprintf(reinterpret_cast<char*>(buffer), "%02X%02X%02X%02X%02X%02X", adapter->Address[0], adapter->Address[1], 
								adapter->Address[2], adapter->Address[3], adapter->Address[4], adapter->Address[5]);

							break;
						}
					}
				}

				free(adapter_info);
			}
		}

		void get_net_bios(unsigned char* buffer, bool collective)
		{
			typedef struct _ASTAT_
			{
				ADAPTER_STATUS adapter_status;
				NAME_BUFFER name_buffer[30];
			} ASTAT;

			ASTAT adapter;
			memset(&adapter, 0, sizeof(ASTAT));

			LANA_ENUM lenum;
			memset(&lenum, 0, sizeof(LANA_ENUM));

			NCB ncb;
			memset(&ncb, 0, sizeof(NCB));

			ncb.ncb_command = NCBENUM;
			ncb.ncb_buffer = reinterpret_cast<unsigned char*>(&lenum);
			ncb.ncb_length = sizeof(LANA_ENUM);

			if (Netbios(&ncb) == NRC_GOODRET)
			{
				for (unsigned char i = 0; i < lenum.length; i++)
				{
					/* reset the LAN adapter */
					memset(&ncb, 0, sizeof(NCB));

					ncb.ncb_command = NCBRESET;
					ncb.ncb_lana_num = (collective ? lenum.lana[i] : i);

					if (Netbios(&ncb) == NRC_GOODRET)
					{
						/* retrieve the adapter information */
						memset(&ncb, 0, sizeof(NCB));
						ncb.ncb_command = NCBASTAT;
						ncb.ncb_lana_num = (collective ? lenum.lana[i] : i);

						strcpy(reinterpret_cast<char*>(ncb.ncb_callname), "*               ");
						memset(&adapter, 0, sizeof(ASTAT));

						ncb.ncb_buffer = reinterpret_cast<unsigned char*>(&adapter);
						ncb.ncb_length = sizeof(ASTAT);

						if (Netbios(&ncb) == NRC_GOODRET)
						{
							if (collective)
							{
								char temp_string[256];
								memset(temp_string, 0, sizeof(temp_string));

								sprintf_s(temp_string, "%02X-%02X-%02X-%02X-%02X-%02X",
									adapter.adapter_status.adapter_address[0], adapter.adapter_status.adapter_address[1],
									adapter.adapter_status.adapter_address[2], adapter.adapter_status.adapter_address[3],
									adapter.adapter_status.adapter_address[4], adapter.adapter_status.adapter_address[5]);

								if (strlen(reinterpret_cast<char*>(buffer)) > 0)
									strcat(reinterpret_cast<char*>(buffer), ", ");

								strcat(reinterpret_cast<char*>(buffer), temp_string);
							}
							else
							{
								/* 0xFE == Ethernet adapter | 0xFF == Token Ring adapter */
								if ((adapter.adapter_status.adapter_type != 0xFE) && (adapter.adapter_status.adapter_type != 0xFF))
								{
									if (adapter.adapter_status.adapter_type)
										*buffer = adapter.adapter_status.adapter_type;
								}
								else
								{
									memcpy(buffer, adapter.adapter_status.adapter_address, 6);
								}

								break;
							}
						}
					}
				}
			}
		}

		unsigned int get_hdd_serial_number()
		{
			char root_path[MAX_PATH];

			if (!GetWindowsDirectory(root_path, MAX_PATH))
				return 0;

			root_path[3] = 0;

			char volume_name_buffer[MAX_PATH + 1];
			memset(volume_name_buffer, 0, sizeof(volume_name_buffer));

			char file_system_name_buffer[MAX_PATH + 1];
			memset(file_system_name_buffer, 0, sizeof(file_system_name_buffer));

			unsigned long volume_serial_number = 0;
			unsigned long maximum_component_length = 0;
			unsigned long file_system_flags = 0;

			if (!GetVolumeInformation(root_path, volume_name_buffer, sizeof(volume_name_buffer), &volume_serial_number,
				&maximum_component_length, &file_system_flags, file_system_name_buffer, sizeof(file_system_name_buffer)))
			{
				return 0;
			}

			return volume_serial_number;
		}
		
		std::string get_hdd_serial()
		{
			char hdd_serial[256];
			memset(hdd_serial, 0, sizeof(hdd_serial));

			unsigned int hdd_serial_number = system_info::get_hdd_serial_number();
			unsigned char* hdd_serial_pointer = reinterpret_cast<unsigned char*>(&hdd_serial_number);

			sprintf_s(hdd_serial, "%02X%02X%02X%02X", hdd_serial_pointer[0], hdd_serial_pointer[1], hdd_serial_pointer[2], hdd_serial_pointer[3]);

			return std::string(hdd_serial);
		}

		unsigned int get_local_ip_address(SOCKET sock)
		{
			if (sock != INVALID_SOCKET)
			{
				sockaddr_in addr;
				memset(&addr, 0, sizeof(sockaddr_in));

				int namelen = sizeof(sockaddr_in);

				if (!getsockname(sock, reinterpret_cast<sockaddr*>(&addr), &namelen))
					return addr.sin_addr.S_un.S_addr;
			}
			
			return 0;
		}

		std::string get_local_mac_address()
		{
			char mac_address[256];
			memset(mac_address, 0, sizeof(mac_address));

			get_net_bios(reinterpret_cast<unsigned char*>(mac_address), true);
			return std::string(mac_address);
		}

		std::string get_local_mac_serial()
		{
			char mac_address[256];
			memset(mac_address, 0, sizeof(mac_address));

			get_adapters_info(reinterpret_cast<unsigned char*>(mac_address));
			return std::string(mac_address);
		}
		
		std::string get_local_serials()
		{
			return std::string(get_local_mac_serial() + "_" + get_hdd_serial());
		};

		void get_machine_id(unsigned char* buffer)
		{
			memset(buffer, 0, 16);
			machine_info* machine = reinterpret_cast<machine_info*>(buffer);

			get_net_bios(machine->mac_address, false);
			machine->hdd_serial = get_hdd_serial_number();

			unsigned short* pointer = reinterpret_cast<unsigned short*>(buffer);

			for (int i = 0; i < 6; i++, pointer++)
				machine->checksum += ((*pointer) * (i + 3));
		}
	}
}