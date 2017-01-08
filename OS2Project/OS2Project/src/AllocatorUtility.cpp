/**
* \file AllocatorUtility.cpp
* \brief Implementation of the allocator helper functions
*/

#include "AllocatorUtility.h"

namespace os2bn140314d {

	#pragma region header_s implementation

	void header_s::initialize(Block *first_pool_block, size_t size_in_blocks) throw (std::invalid_argument) {
		if (size_in_blocks == 0) {
			throw std::invalid_argument("Size of the memory must be greater than 0");
		}

		buddy_header_.initialize(first_pool_block, size_in_blocks);
		slab_header_.initialize();

		new (&write_mutex_) std::mutex;
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

	const Block *AllocatorUtility::blockStart(const void * memory) noexcept {
		auto start = reinterpret_cast<const byte *>(memory_start_);
		auto pointer = reinterpret_cast<const byte *>(memory);

		auto diff = pointer - start;

		return reinterpret_cast<const Block *>(pointer - diff % BLOCK_SIZE);
	}

	void AllocatorUtility::writeLock() noexcept {
		auto &header = AllocatorUtility::header();
		header.write_mutex_.lock();
	}

	void AllocatorUtility::writeUnlock() noexcept {
		auto &header = AllocatorUtility::header();
		header.write_mutex_.unlock();
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
