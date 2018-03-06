#pragma once

#define _MAX_KEY_COLUMNS		(256/32)
#define _MAX_ROUNDS				14
#define _MAX_IV_SIZE			16

namespace firefly
{
	class rijndael
	{
	public:
		rijndael();
		~rijndael();

		bool initialize(const unsigned char* key);
		bool transform_block(const unsigned char* input, unsigned int length, unsigned char* output);
	
	protected:
		void transform(const unsigned char a[16], unsigned char b[16]);
		void key_schedule(unsigned char key_matrix[_MAX_KEY_COLUMNS][4]);

		void round_key(bool xor, unsigned int start_value, unsigned int less_than, 
			unsigned char key_matrix[_MAX_KEY_COLUMNS][4], unsigned char temp_key_matrix[_MAX_KEY_COLUMNS][4]);

		void round_expanded_key(unsigned int round_keys, unsigned int& current_round,
			unsigned int& tab_index, unsigned char temp_key_matrix[_MAX_KEY_COLUMNS][4]);

	private:
		bool validation;
		unsigned int rounds;
		unsigned char iv[_MAX_IV_SIZE];
		unsigned char expanded_key[_MAX_ROUNDS + 1][4][4];
	};
}