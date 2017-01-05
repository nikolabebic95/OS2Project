/**
 * \file AllocatorUtility.h
 * \brief File providing the helper functions for the allocator
 */

#ifndef _allocatorutility_h_
#define _allocatorutility_h_

#include "Definitions.h" // Block size
#include "Buddy.h" // buddy_header_s

namespace os2bn140314d {

	#pragma region Header union

	struct slab_header_s {
		// TODO: Add fields
	};

	struct header_s {
		size_t size_in_blocks_;
		buddy_header_s buddy_header_;
		slab_header_s slab_header_;

		void initialize(Block *first_pool_block, size_t size_in_blocks) throw (std::invalid_argument);
	};

	union Header {
		header_s header_;
		Block block_;
	};

	#pragma endregion 

	/**
	 * \brief Utility class providing all the helper functions for the allocator
	 */
	class AllocatorUtility final {
	public:

		#pragma region Public interface

		static const size_t MIN_SIZE_IN_BLOCKS = 3;

		// TODO: Provide public interface

		/**
		 * \brief Initialize the allocator
		 * \param memory_start Pointer to the memory which the allocator can use
		 * \param size_in_blocks Size of the memory in blocks
		 */
		static void initialize(void *memory_start, int size_in_blocks) throw (std::invalid_argument);
		
		/**
		 * \brief Get the pointer to memory used by the allocator
		 * \return Pointer to the memory used by the allocator
		 */
		static void *memoryStart() noexcept;

		static header_s &header() noexcept;
		static buddy_header_s &buddyHeader() noexcept;
		static slab_header_s &slabHeader() noexcept;

		#pragma endregion

	private:
		static void *memory_start_;

		#pragma region Delete constructors
		
		AllocatorUtility() = delete;
		AllocatorUtility(const AllocatorUtility &) = delete;
		void operator=(const AllocatorUtility &) = delete;

		#pragma endregion

	};
}

#endif
