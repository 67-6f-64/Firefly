#pragma once

#include "native.hpp"
#include "ZArray.hpp"

namespace firefly
{
	struct COutPacket
	{
		int m_bLoopback;						// 0x00
	
		union									// 0x04
		{	
			unsigned short* m_aSendHeaders;
			unsigned char* m_aSendBuff;
		};

		unsigned int m_uOffset;					// 0x08
		int m_bTypeHeader1Byte;					// 0x0C
		int m_bIsEncryptedByShanda;				// 0x10
	};
	
#pragma warning(disable: 4200)
	class CInPacket
	{
	public:
		template <typename T> 
		T fetch_at(unsigned int index)
		{
			return *reinterpret_cast<T*>(&this->m_aRecvBuff->a[index]);
		}

	private:
		int m_bLoopback;						// 0x00
		int m_nState;							// 0x04

		union									// 0x08
		{
			struct 
			{
				unsigned int dw;
				unsigned short usHeader;
			}* pHeader;
			struct 
			{
				unsigned int dw;
				unsigned char a[0];
			}* m_aRecvBuff;
		};

		unsigned int m_uLength;					// 0x0C
		unsigned int m_uRawSeq;					// 0x10
		unsigned int m_uDataLen;				// 0x14
		unsigned int m_uOffset;					// 0x18
	};
#pragma warning(default: 4200)

	static_assert_size(sizeof(COutPacket), 0x14);
	static_assert_size(sizeof(CInPacket), 0x1C);
}