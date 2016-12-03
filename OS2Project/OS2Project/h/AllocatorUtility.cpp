#include "AllocatorUtility.h"

namespace general {

	void *AllocatorUtility::memory_start_ = nullptr;

	void AllocatorUtility::initialize(void * memory_start, int size_in_blocks) {
		// TODO: Implementation
		// TODO: Decide how to store information about everything
		memory_start_ = memory_start;
	}

	void *AllocatorUtility::memoryStart() {
		return memory_start_;
	}
	
}
