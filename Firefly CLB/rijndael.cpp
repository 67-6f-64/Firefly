#include "rijndael.hpp"
#include "rijndael_tables.hpp"

namespace firefly
{
	rijndael::rijndael()
		: validation(false)
	{

	}

	rijndael::~rijndael()
	{

	}

	bool rijndael::initialize(const unsigned char* key)
	{
		for (int i = 0; i < _MAX_IV_SIZE; i++)
		{
			this->iv[i] = 0;
		}

		this->rounds = 14; /* Calculated as such: this->rounds = ((key_length * 8) / 32) + 6; */

		if (!key)
		{
			return false;
		}

		unsigned char key_matrix[_MAX_KEY_COLUMNS][4];

		for (unsigned int i = 0; i < 32; i++)
		{
			key_matrix[i >> 2][i & 3] = key[i];
		}

		this->key_schedule(key_matrix);

		this->validation = true;
		return true;
	}

	bool rijndael::transform_block(const unsigned char* input, unsigned int length, unsigned char* output)
	{
		if (!this->validation || input == 0 || length <= 0)
		{
			return false;
		}

		for (unsigned int i = (length / 16); i > 0; i--, input += 16, output += 16)
		{
			this->transform(input, output);
		}

		return true;
	}
	
	void rijndael::transform(const unsigned char a[16], unsigned char b[16])
	{
		unsigned char temp[4][4];

		*((unsigned int*)temp[0]) = *((unsigned int*)(a)) ^ *((unsigned int*)this->expanded_key[0][0]);
		*((unsigned int*)temp[1]) = *((unsigned int*)(a + 4)) ^ *((unsigned int*)this->expanded_key[0][1]);
		*((unsigned int*)temp[2]) = *((unsigned int*)(a + 8)) ^ *((unsigned int*)this->expanded_key[0][2]);
		*((unsigned int*)temp[3]) = *((unsigned int*)(a + 12)) ^ *((unsigned int*)this->expanded_key[0][3]);

		*((unsigned int*)(b)) = *((unsigned int*)T1[temp[0][0]])
			^ *((unsigned int*)T2[temp[1][1]])
			^ *((unsigned int*)T3[temp[2][2]])
			^ *((unsigned int*)T4[temp[3][3]]);

		*((unsigned int*)(b + 4)) = *((unsigned int*)T1[temp[1][0]])
			^ *((unsigned int*)T2[temp[2][1]])
			^ *((unsigned int*)T3[temp[3][2]])
			^ *((unsigned int*)T4[temp[0][3]]);

		*((unsigned int*)(b + 8)) = *((unsigned int*)T1[temp[2][0]])
			^ *((unsigned int*)T2[temp[3][1]])
			^ *((unsigned int*)T3[temp[0][2]])
			^ *((unsigned int*)T4[temp[1][3]]);

		*((unsigned int*)(b + 12)) = *((unsigned int*)T1[temp[3][0]])
			^ *((unsigned int*)T2[temp[0][1]])
			^ *((unsigned int*)T3[temp[1][2]])
			^ *((unsigned int*)T4[temp[2][3]]);

		for (unsigned int i = 1; i < this->rounds - 1; i++)
		{
			*((unsigned int*)temp[0]) = *((unsigned int*)(b)) ^ *((unsigned int*)this->expanded_key[i][0]);
			*((unsigned int*)temp[1]) = *((unsigned int*)(b + 4)) ^ *((unsigned int*)this->expanded_key[i][1]);
			*((unsigned int*)temp[2]) = *((unsigned int*)(b + 8)) ^ *((unsigned int*)this->expanded_key[i][2]);
			*((unsigned int*)temp[3]) = *((unsigned int*)(b + 12)) ^ *((unsigned int*)this->expanded_key[i][3]);

			*((unsigned int*)(b)) = *((unsigned int*)T1[temp[0][0]])
				^ *((unsigned int*)T2[temp[1][1]])
				^ *((unsigned int*)T3[temp[2][2]])
				^ *((unsigned int*)T4[temp[3][3]]);

			*((unsigned int*)(b + 4)) = *((unsigned int*)T1[temp[1][0]])
				^ *((unsigned int*)T2[temp[2][1]])
				^ *((unsigned int*)T3[temp[3][2]])
				^ *((unsigned int*)T4[temp[0][3]]);

			*((unsigned int*)(b + 8)) = *((unsigned int*)T1[temp[2][0]])
				^ *((unsigned int*)T2[temp[3][1]])
				^ *((unsigned int*)T3[temp[0][2]])
				^ *((unsigned int*)T4[temp[1][3]]);

			*((unsigned int*)(b + 12)) = *((unsigned int*)T1[temp[3][0]])
				^ *((unsigned int*)T2[temp[0][1]])
				^ *((unsigned int*)T3[temp[1][2]])
				^ *((unsigned int*)T4[temp[2][3]]);
		}

		*((unsigned int*)temp[0]) = *((unsigned int*)(b)) ^ *((unsigned int*)this->expanded_key[this->rounds - 1][0]);
		*((unsigned int*)temp[1]) = *((unsigned int*)(b + 4)) ^ *((unsigned int*)this->expanded_key[this->rounds - 1][1]);
		*((unsigned int*)temp[2]) = *((unsigned int*)(b + 8)) ^ *((unsigned int*)this->expanded_key[this->rounds - 1][2]);
		*((unsigned int*)temp[3]) = *((unsigned int*)(b + 12)) ^ *((unsigned int*)this->expanded_key[this->rounds - 1][3]);

		b[0] = T1[temp[0][0]][1];
		b[1] = T1[temp[1][1]][1];
		b[2] = T1[temp[2][2]][1];
		b[3] = T1[temp[3][3]][1];
		b[4] = T1[temp[1][0]][1];
		b[5] = T1[temp[2][1]][1];
		b[6] = T1[temp[3][2]][1];
		b[7] = T1[temp[0][3]][1];
		b[8] = T1[temp[2][0]][1];
		b[9] = T1[temp[3][1]][1];
		b[10] = T1[temp[0][2]][1];
		b[11] = T1[temp[1][3]][1];
		b[12] = T1[temp[3][0]][1];
		b[13] = T1[temp[0][1]][1];
		b[14] = T1[temp[1][2]][1];
		b[15] = T1[temp[2][3]][1];

		*((unsigned int*)(b)) ^= *((unsigned int*)this->expanded_key[this->rounds][0]);
		*((unsigned int*)(b + 4)) ^= *((unsigned int*)this->expanded_key[this->rounds][1]);
		*((unsigned int*)(b + 8)) ^= *((unsigned int*)this->expanded_key[this->rounds][2]);
		*((unsigned int*)(b + 12)) ^= *((unsigned int*)this->expanded_key[this->rounds][3]);
	}

	void rijndael::key_schedule(unsigned char key_matrix[_MAX_KEY_COLUMNS][4])
	{
		int round_keys = this->rounds - 6;
		unsigned char temp_key_matrix[_MAX_KEY_COLUMNS][4];

		round_key(false, 0, round_keys, key_matrix, temp_key_matrix);

		unsigned int current_round = 0;
		unsigned int tab_index = 0;

		round_expanded_key(round_keys, current_round, tab_index, temp_key_matrix);

		for (int rcon_pointer = 0; current_round <= this->rounds;)
		{
			temp_key_matrix[0][0] ^= S[temp_key_matrix[round_keys - 1][1]];
			temp_key_matrix[0][1] ^= S[temp_key_matrix[round_keys - 1][2]];
			temp_key_matrix[0][2] ^= S[temp_key_matrix[round_keys - 1][3]];
			temp_key_matrix[0][3] ^= S[temp_key_matrix[round_keys - 1][0]];
			temp_key_matrix[0][0] ^= rcon[rcon_pointer++];

			if (round_keys != 8)
			{
				round_key(true, 1, round_keys, temp_key_matrix, temp_key_matrix);
			}
			else
			{
				round_key(true, 1, round_keys / 2, temp_key_matrix, temp_key_matrix);

				temp_key_matrix[round_keys / 2][0] ^= S[temp_key_matrix[round_keys / 2 - 1][0]];
				temp_key_matrix[round_keys / 2][1] ^= S[temp_key_matrix[round_keys / 2 - 1][1]];
				temp_key_matrix[round_keys / 2][2] ^= S[temp_key_matrix[round_keys / 2 - 1][2]];
				temp_key_matrix[round_keys / 2][3] ^= S[temp_key_matrix[round_keys / 2 - 1][3]];

				round_key(true, round_keys / 2 + 1, round_keys, temp_key_matrix, temp_key_matrix);
			}

			round_expanded_key(round_keys, current_round, tab_index, temp_key_matrix);
		}
	}

	void rijndael::round_key(bool xor, unsigned int start_value, unsigned int less_than,
		unsigned char key_matrix[_MAX_KEY_COLUMNS][4], unsigned char temp_key_matrix[_MAX_KEY_COLUMNS][4])
	{
		for (unsigned int i = start_value; i < less_than; i++)
		{
			if (xor)
			{
				unsigned int temp_key = *reinterpret_cast<unsigned int*>(key_matrix[i - 1]);
				*reinterpret_cast<unsigned int*>(temp_key_matrix[i]) ^= temp_key;
			}
			else
			{
				unsigned int temp_key = *reinterpret_cast<unsigned int*>(key_matrix[i]);
				*reinterpret_cast<unsigned int*>(temp_key_matrix[i]) = temp_key;
			}
		}
	}

	void rijndael::round_expanded_key(unsigned int round_keys, unsigned int& current_round,
		unsigned int& tab_index, unsigned char temp_key_matrix[_MAX_KEY_COLUMNS][4])
	{
		for (unsigned int i = 0; (i < round_keys) && (current_round <= this->rounds);)
		{
			for (; (i < round_keys) && (tab_index < 4); i++, tab_index++)
			{
				unsigned int temp_key = *reinterpret_cast<unsigned int*>(temp_key_matrix[i]);
				*reinterpret_cast<unsigned int*>(this->expanded_key[current_round][tab_index]) = temp_key;
			}

			if (tab_index == 4)
			{
				current_round++;
				tab_index = 0;
			}
		}
	}
}