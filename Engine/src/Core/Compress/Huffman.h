#pragma once

#define HUFFMAN_MAX_CODE_LEN 15

struct Huffman_Tree
{
	// To make sure codes stay unique (so that 0001 is different from 01, even though
	//	they are the same number numerically), we insert a 1 at the beginning of all codes
	// (So 0001 would become 10001, and 01 would become 101) during lookup.
	// This way all codes have a unique entry into the map
	// This is why the map has to fit HUFF_MAX + 1 bits instead of just HUFF_MAX

	// Lookup map, where the index is the code (prefixed with one), and the value is the symbol
	//	represented by that code. A value of -1 means no symbol is mapped to this code.
	i16 map[1 << (HUFFMAN_MAX_CODE_LEN + 1)];
};

void huffman_init(Huffman_Tree* tree);
void huffman_build(Huffman_Tree* tree, u16* code_lengths, u16 code_num);

void huffman_build_fixed_length(Huffman_Tree* tree);
void huffman_build_fixed_distance(Huffman_Tree* tree);

i16 huffman_find_symbol(Huffman_Tree* tree, u16 code, u16 code_len);