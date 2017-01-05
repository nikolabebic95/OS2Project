/**
* \file AllocatorUtility.cpp
* \brief Implementation of the allocator helper functions
*/

#include "AllocatorUtility.h"

namespace os2bn140314d {

	#pragma region header_s implementation

	void header_s::initialize(Block *first_pool_block, size_t size_in_blocks) throw (std::invalid_argument) {
		if (size_in_blocks_ == 0) {
			throw std::invalid_argument("Size of the memory must be greater than 0");
		}

		size_in_blocks_ = size_in_blocks;
		buddy_header_.initialize(first_pool_block, size_in_blocks_);
		// TODO: Initialize slab header
	}

	#pragma endregion

	#pragma region AllocatorUtility implementation

	void *AllocatorUtility::memory_start_ = nullptr;

	void AllocatorUtility::initialize(void * memory_start, int size_in_blocks) throw (std::invalid_argument) {
		if (size_in_blocks < MIN_SIZE_IN_BLOCKS) {
			throw std::invalid_argument("Size of the allocated space must be at least " + MIN_SIZE_IN_BLOCKS);
		}

		memory_start_ = memory_start;
		
		auto &header = *static_cast<Header *>(memory_start);

		auto first_pool_block = static_cast<Block *>(memory_start) + 1;

		header.header_.initialize(first_pool_block, size_in_blocks - 1);
	}

	void *AllocatorUtility::memoryStart() noexcept {
		return memory_start_;
	}

	header_s &AllocatorUtility::header() noexcept {
		auto &header = *static_cast<Header *>(memory_start_);
		return header.header_;
	}

	buddy_header_s & AllocatorUtility::buddyHeader() noexcept {
		auto &header = AllocatorUtility::header();
		return header.buddy_header_;
	}

	slab_header_s & AllocatorUtility::slabHeader() noexcept {
		auto &header = AllocatorUtility::header();
		return header.slab_header_;
	}

	#pragma endregion 
}
