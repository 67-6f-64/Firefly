#include "output.hpp"

namespace output
{
	void hexdump(void* input, int length)
	{
		unsigned char* buffer = reinterpret_cast<unsigned char*>(input);

		for (int i = 0; i < length; i += 16)
		{
			printf("%06X: ", i);

			for (int j = 0; j < 16; j++)
			{
				if (i + j < length)
				{
					printf("%02X ", buffer[i + j]);
				}
				else
				{
					printf("   ");
				}
			}

			printf(" ");

			for (int j = 0; j < 16; j++)
			{
				if (i + j < length)
				{
					printf("%c", isprint(buffer[i + j]) ? buffer[i + j] : '.');
				}
			}

			printf("\n");
		}
	}

	void fhexdump(FILE* file, void* input, int length)
	{
		unsigned char* buffer = reinterpret_cast<unsigned char*>(input);

		for (int i = 0; i < length; i += 16)
		{
			fprintf(file, "%06X: ", i);

			for (int j = 0; j < 16; j++)
			{
				if (i + j < length)
				{
					fprintf(file, "%02X ", buffer[i + j]);
				}
				else
				{
					fprintf(file, "   ");
				}
			}

			fprintf(file, " ");

			for (int j = 0; j < 16; j++)
			{
				if (i + j < length)
				{
					fprintf(file, "%c", isprint(buffer[i + j]) ? buffer[i + j] : '.');
				}
			}

			fprintf(file, "\n");
		}
	}
}