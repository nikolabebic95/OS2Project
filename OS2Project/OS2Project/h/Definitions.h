#ifndef _definitions_h_
#define _definitions_h_

typedef struct kmem_cache_s kmem_cache_t;

const size_t BLOCK_SIZE = 4096;
const size_t CACHE_L1_LINE_SIZE = 64;

const size_t POWERS_OF_TWO = 64;
const size_t BITS_IN_BYTE = 8;

const size_t ENTRIES_IN_BITMAP = BLOCK_SIZE * BITS_IN_BYTE;

union Block;

struct BlockInfo {
	Block *next;
	Block *prev;
	size_t index;
};

union Block {
	BlockInfo info;
	unsigned char bytes[BLOCK_SIZE];
};

#endif
