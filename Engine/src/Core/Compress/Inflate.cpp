#include "Inflate.h"
#include <stdio.h>
#include <stdlib.h>
#include "Huffman.h"

struct Inflate_Stream
{
	u8* data;
	u32 length;

	union
	{
		struct
		{
			u32 bit		: 3;
			u32 byte	: 29;
		} s;

		u32 position;
	};
};

void write_bits(Inflate_Stream* stream, const u32 value, const u32 num_bits)
{
	u8 bit_value;
	for(u32 i=0; i<num_bits; ++i)
	{
		bit_value = (value >> i) & 0x1;
		u8& value = stream->data[stream->s.byte];

		if (bit_value)
			value |= (1 << stream->s.bit);
		else
			value &= ~(1 << stream->s.bit);

		stream->position++;
	}
}

u32 read_bits(Inflate_Stream* stream, const u32 num_bits)
{
	u32 result = 0;
	u8 bit_value;
	for(u32 i=0; i<num_bits; ++i)
	{
		bit_value = (stream->data[stream->s.byte] >> stream->s.bit) & 0x1;
		result |= bit_value << i;

		stream->position++;
	}

	return result;
}

void read_write_bytes(Inflate_Stream* stream, const u16 length, const u32 distance)
{
	for(u32 i=0; i<length; ++i)
	{
		stream->data[stream->s.byte] = stream->data[stream->s.byte - distance];
		stream->s.byte++;
	}
}

void print_bits(u8 value, u8 len)
{
	for(u8 i=0; i<len; ++i)
	{
		u8 bit = value & (0x1 << (len - i - 1));
		printf("%d", bit ? 1 : 0);
	}
}

// Reads a code and looks up its symbol in given huffman tree
u32 read_symbol_from_stream(Inflate_Stream* str, Huffman_Tree* tree)
{
	u32 code_len = 1;
	u32 code = 0;
	u32 symbol = -1;

	for(code_len=1; code_len<=HUFFMAN_MAX_CODE_LEN; ++code_len)
	{
		code = (code << 1) | read_bits(str, 1);
		symbol = huffman_find_symbol(tree, code, code_len);

		if (symbol != -1)
			break;
	}

	return symbol;
}

// Function to decode huffman tree code lengths for a dynamic length tree (done as the second and third step in decoding a dynamic huffman tree)
void decode_dynamic_huffman_codes(Inflate_Stream* stream, u16* code_length_arr, u32 code_num, Huffman_Tree* length_tree)
{
	for(u32 i=0; i<code_num;)
	{
		u32 symbol = read_symbol_from_stream(stream, length_tree);
		if (symbol < 16)
		{
			// 0 - 15 represents code lengths of 0-15
			code_length_arr[i] = symbol;
			i++;
		}
		else if (symbol == 16)
		{
			// 16 means to copy the previous code length 3-6 times (2 extra bits)
			u16 last_length = code_length_arr[i - 1];

			u16 copy_num = 3 + read_bits(stream, 2);
			for(u32 copy=0; copy<copy_num; ++copy)
			{
				code_length_arr[i + copy] = last_length;
			}
			i += copy_num;
		}
		else if (symbol == 17)
		{
			// 17 means set length to (0) 3-10 times (3 extra bits)
			u32 num_zeroes = 3 + read_bits(stream, 3);
			for(u32 j=0; j<num_zeroes; ++j)
			{
				code_length_arr[i + j] = 0;
			}

			i += num_zeroes;
		}
		else if (symbol == 18)
		{
			// 18 means set length to (0) 11 - 138 times (7 extra bits)
			u32 num_zeroes = 11 + read_bits(stream, 7);
			for(u32 j=0; j<num_zeroes; ++j)
			{
				code_length_arr[i + j] = 0;
			}

			i += num_zeroes;
		}
		else
		{
			error("Invalid length symbol");
		}
	}
}

// Decodes a literal/length tree and distance tree from the inflate stream
void decode_dynamic_huffman_trees_from_stream(Inflate_Stream* stream, Huffman_Tree* lit_length_tree, Huffman_Tree* distance_tree)
{
	// Dynamic huffman trees are defined themselves with a huffman tree (for greater compression)
	u16 num_lit_length_codes = read_bits(stream, 5)		+ 257;
	u16 num_distance_codes = read_bits(stream, 5)		+ 1;
	u16 num_code_length_codes = read_bits(stream, 4)	+ 4;

	Huffman_Tree code_length_tree;
	huffman_init(&code_length_tree);

	{
		// Step 1: Read the lengths for the huffman tree used to decode the _actual_ huffman trees
		// Where each symbol is a rule for how to decode the lengths of each tree
		u16 code_lengths[19] = { 0 };
		u16 code_length_symbol_order[] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };

		for(u32 i=0; i<num_code_length_codes; ++i)
		{
			code_lengths[code_length_symbol_order[i]] = read_bits(stream, 3);
		}

		huffman_build(&code_length_tree, code_lengths, 19);

		// Step 2: Decode the lit/length tree code lengths by using the above tree
		u16 length_buffer[286];
		u16* lit_length_code_lengths = length_buffer;
		decode_dynamic_huffman_codes(stream, lit_length_code_lengths, num_lit_length_codes, &code_length_tree);

		huffman_build(lit_length_tree, lit_length_code_lengths, num_lit_length_codes);

		// Step 3: Decode the distance tree code lengths by using the above tree
		u16* distance_code_lengths = length_buffer;
		decode_dynamic_huffman_codes(stream, distance_code_lengths, num_distance_codes, &code_length_tree);

		huffman_build(distance_tree, distance_code_lengths, num_distance_codes);
	}
}

u16 decode_length(Inflate_Stream* str, u32 symbol)
{
	if (symbol == 285)
		return 258;

	symbol = symbol - 257;
	if (symbol < 8)
		return symbol + 3;
	else
	{
		u8 extra_bits = (symbol - 4) / 4;
		return (((symbol - 8) % 4 + 4) << extra_bits) + 3 + read_bits(str, extra_bits);
	}
}

u16 decode_distance(Inflate_Stream* str, u32 symbol)
{
	if (symbol < 4)
		return symbol + 1;
	else
	{
		u8 extra_bits = (symbol - 2) / 2;
		return (((symbol - 4) % 2 + 2) << extra_bits) + 1 + read_bits(str, extra_bits);
	}
}

void decode_huffman_block(Inflate_Stream* in, Inflate_Stream* out, Huffman_Tree* length_tree, Huffman_Tree* distance_tree)
{
	// Read symbol
	while(true)
	{
		u32 symbol = read_symbol_from_stream(in, length_tree);

		// End of block
		if (symbol == 256)
			break;

		// Literal byte
		if (symbol < 256)
		{
			write_bits(out, symbol, 8);
		}

		// <Length, Distance> pair
		else
		{
			u32 length = decode_length(in, symbol);
			u32 dist_symbol = read_symbol_from_stream(in, distance_tree);
			u32 dist = decode_distance(in, dist_symbol);

			read_write_bytes(out, length, dist);
		}
	}
}

void inflate(void* dest, u32 dest_size, void* src, u32 src_size)
{
	Inflate_Stream in_str;
	in_str.data = (u8*)src;
	in_str.length = src_size;
	in_str.position = 0;

	Inflate_Stream out_str;
	out_str.data = (u8*)dest;
	out_str.length = dest_size;
	out_str.position = 0;

	u8 block_last = read_bits(&in_str, 1);
	u8 block_header = read_bits(&in_str, 2);

	// Num literal/length codes
	if (block_header == 0)
	{
		error("Raw data not implemented");
	}
	if (block_header == 1)
	{
		Huffman_Tree fixed_length;
		Huffman_Tree fixed_distance;

		huffman_init(&fixed_length);
		huffman_build_fixed_length(&fixed_length);
		huffman_init(&fixed_distance);
		huffman_build_fixed_distance(&fixed_distance);

		decode_huffman_block(&in_str, &out_str, &fixed_length, &fixed_distance);
	}
	if (block_header == 2)
	{
		Huffman_Tree lit_length_tree;
		huffman_init(&lit_length_tree);

		Huffman_Tree distance_tree;
		huffman_init(&distance_tree);

		decode_dynamic_huffman_trees_from_stream(&in_str, &lit_length_tree, &distance_tree);
		decode_huffman_block(&in_str, &out_str, &lit_length_tree, &distance_tree);
	}
}
