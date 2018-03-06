#include "crc32.hpp"

namespace firefly
{
	namespace crc32
	{
		void make_crc32_table(unsigned int* crc32_table)
		{
			static const unsigned char polynomial_terms[] = { 0, 1, 2, 4, 5, 7, 8, 10, 11, 12, 16, 22, 23, 26 };
		
			unsigned int polynomial = 0;

			for (unsigned int i = 0; i < sizeof(polynomial_terms); i++)
			{
				polynomial |= (1 << (31 - polynomial_terms[i]));
			}

			unsigned int mirror_polynomial = polynomial; 

			for (unsigned int i = 0; i < ((sizeof(polynomial) * 8) - 1); i++)
			{
				mirror_polynomial <<= 1;
				polynomial >>= 1;
				mirror_polynomial |= polynomial & 1;
			}

			for (unsigned int i = 0, crc = (i << 24); i < 256; i++, crc = (i << 24))
			{
				for (unsigned int j = 0; j < 8; j++)
				{
					crc = (crc & 0x80000000 ? (mirror_polynomial ^ (crc << 1)) : (crc << 1));
				}

				crc32_table[i] = crc;
			}
		}

		unsigned int* get_crc32_table()
		{
			static bool crc_table_empty = true;
			static unsigned int crc32_table[256] = { 0 };

			if (crc_table_empty)
			{
				crc_table_empty = false;
				make_crc32_table(crc32_table);
			}

			return crc32_table;
		}

		unsigned int crc32(unsigned char* data, unsigned int size, unsigned int crc)
		{
			unsigned int* crc32_table = get_crc32_table();
			unsigned char* data_buffer = data;

			if (size >= 16)
			{
				for (unsigned int i = (size >> 4); i > 0; i--, size -= 16, data_buffer += 16)
				{
					unsigned int crc_0	= crc32_table[(crc	  >> 24) ^ data_buffer[0]]  ^ (crc    << 8);
					unsigned int crc_1	= crc32_table[(crc_0  >> 24) ^ data_buffer[1]]  ^ (crc_0  << 8);
					unsigned int crc_2	= crc32_table[(crc_1  >> 24) ^ data_buffer[2]]  ^ (crc_1  << 8);
					unsigned int crc_3	= crc32_table[(crc_2  >> 24) ^ data_buffer[3]]  ^ (crc_2  << 8);
					unsigned int crc_4	= crc32_table[(crc_3  >> 24) ^ data_buffer[4]]  ^ (crc_3  << 8);
					unsigned int crc_5	= crc32_table[(crc_4  >> 24) ^ data_buffer[5]]  ^ (crc_4  << 8);
					unsigned int crc_6	= crc32_table[(crc_5  >> 24) ^ data_buffer[6]]  ^ (crc_5  << 8);
					unsigned int crc_7	= crc32_table[(crc_6  >> 24) ^ data_buffer[7]]  ^ (crc_6  << 8);
					unsigned int crc_8	= crc32_table[(crc_7  >> 24) ^ data_buffer[8]]  ^ (crc_7  << 8);
					unsigned int crc_9	= crc32_table[(crc_8  >> 24) ^ data_buffer[9]]  ^ (crc_8  << 8);
					unsigned int crc_10 = crc32_table[(crc_9  >> 24) ^ data_buffer[10]] ^ (crc_9  << 8);
					unsigned int crc_11 = crc32_table[(crc_10 >> 24) ^ data_buffer[11]] ^ (crc_10 << 8);
					unsigned int crc_12 = crc32_table[(crc_11 >> 24) ^ data_buffer[12]] ^ (crc_11 << 8);
					unsigned int crc_13 = crc32_table[(crc_12 >> 24) ^ data_buffer[13]] ^ (crc_12 << 8);
					unsigned int crc_14 = crc32_table[(crc_13 >> 24) ^ data_buffer[14]] ^ (crc_13 << 8);
					unsigned int crc_15 = crc32_table[(crc_14 >> 24) ^ data_buffer[15]] ^ (crc_14 << 8);
					crc = crc_15;
				}
			}

			if (size >= 1)
			{
				for (unsigned int i = size; i > 0; i--, size--, data_buffer++)
				{
					crc = crc32_table[(crc >> 24) ^ data_buffer[0]] ^ (crc << 8);
				}
			}

			return crc;
		}
	}
}