#include "Huffman.h"

void huffman_init(Huffman_Tree* tree)
{
	// Reset (set every code to -1)
	u32 max = 1 << (HUFFMAN_MAX_CODE_LEN + 1);
	for(u32 i=0; i<max; ++i)
	{
		tree->map[i] = -1;
	}
}

void huffman_build(Huffman_Tree* tree, u16* code_lengths, u16 code_num)
{
	// Count how codes of each lengths
	u16 len_count[HUFFMAN_MAX_CODE_LEN + 1] = { 0 };

	for(u16 i=0; i<code_num; ++i)
	{
#if DEBUG
		if (code_lengths[i] > HUFFMAN_MAX_CODE_LEN)
		{
			error("Found code length bigger than %d (%d) when building huffman tree", HUFFMAN_MAX_CODE_LEN, code_lengths[i]);
		}
#endif
		len_count[code_lengths[i]]++;
	}

	// Calculate the smallest code for each length
	len_count[0] = 0;
	u16 next_code[HUFFMAN_MAX_CODE_LEN + 1] = { 0 };
	u16 code = 0;

	for(u16 i=1; i<HUFFMAN_MAX_CODE_LEN + 1; ++i)
	{
		code = (code + (len_count[i - 1])) << 1;
		next_code[i] = code;
	}

	// Fill out the map in the huffman tree with symbols where map[code] = symbol
	// However, to make sure codes stay unique (so that 0001 is different from 01, even though
	//	they are the same number numerically), we insert a 1 at the beginning of all codes
	// (So 0001 would become 10001, and 01 would become 101).
	// This way all codes have a unique entry into the map
	for(u16 i=0; i<code_num; ++i)
	{
		u16 len = code_lengths[i];
		if (len == 0)
			continue;

		u16 code = next_code[len];
		next_code[len]++;

		// Prefix with a 1
		code = code | (1 << len);
		tree->map[code] = i;
	}
}

// Builds a fixed length tree for deflate compression
void huffman_build_fixed_length(Huffman_Tree* tree)
{
	// Literal value 0-143 (codes 00110000 - 10111111)
	{
		u16 code = 0b100110000;
		for(u16 i=0; i<=143; ++i)
		{
			tree->map[code] = i;
			code++;
		}
	}

	// Literal value 144-255 (codes 110010000 - 111111111)
	{
		u16 code = 0b1110010000;
		for(u16 i=144; i<=255; ++i)
		{
			tree->map[code] = i;
			code++;
		}
	}

	// Length value 256-279 (codes 0000000 - 0010111)
	{
		u16 code = 0b10000000;
		for(u16 i=256; i<=279; ++i)
		{
			tree->map[code] = i;
			code++;
		}
	}

	// Length value 280-287 (codes 11000000 - 11000111)
	{
		u16 code = 0b111000000;
		for(u16 i=280; i<=287; ++i)
		{
			tree->map[code] = i;
			code++;
		}
	}
}

// Builds a fixed distance tree for deflate compression
void huffman_build_fixed_distance(Huffman_Tree* tree)
{
	{
		u16 code = 0b100000;
		for(u16 i=0; i<32; ++i)
		{
			tree->map[code] = i;
			code++;
		}
	}
}

i16 huffman_find_symbol(Huffman_Tree* tree, u16 code, u16 code_len)
{
	// Prefix the code with a 1
	code = code | (1 << code_len);
	return tree->map[code];
}