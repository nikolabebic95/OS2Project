/**
* \file Definitions.h
* \brief File providing the constants and type definitions used by the allocator
*/

#ifndef _definitions_h_
#define _definitions_h_

#include "Slab.h" // Definitions

namespace os2bn140314d {

	typedef unsigned char byte;

	const size_t POWERS_OF_TWO = 64;
	const size_t BITS_IN_BYTE = 8;

	const size_t ENTRIES_IN_BITMAP = BLOCK_SIZE * BITS_IN_BYTE;

	const size_t MAX_NAME_LENGTH = 256;

	const size_t NULL_INDEX = ~static_cast<size_t>(0);

	union Block;

	struct BlockInfo {
		Block *next;
		Block *prev;
		size_t index;
	};

	union Block {
		BlockInfo info;
		byte bytes[BLOCK_SIZE];
	};

	enum AllocatorError : int {
		OK = 0,
		NO_MORE_SPACE = 1,
		DESTROYING_NON_EMPTY_CACHE = 2,
		DEALLOCATING_WRONG_OBJECT = 4
	};
	
	inline AllocatorError operator|(const AllocatorError &lhs, const AllocatorError &rhs) {
		return static_cast<AllocatorError>(static_cast<int>(lhs) | static_cast<int>(rhs));
	}

	inline AllocatorError &operator|=(AllocatorError &lhs, const AllocatorError &rhs) {
		lhs = lhs | rhs;
		return lhs;
	}

	inline AllocatorError operator&(const AllocatorError &lhs, const AllocatorError &rhs) {
		return static_cast<AllocatorError>(static_cast<int>(lhs) & static_cast<int>(rhs));
	}

	inline AllocatorError &operator&=(AllocatorError &lhs, const AllocatorError &rhs) {
		lhs = lhs & rhs;
		return lhs;
	}
}

#endif
