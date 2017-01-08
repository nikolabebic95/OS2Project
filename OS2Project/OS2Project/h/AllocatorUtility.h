/**
 * \file AllocatorUtility.h
 * \brief File providing the helper functions for the allocator
 */

#ifndef _allocatorutility_h_
#define _allocatorutility_h_

#include "Definitions.h" // Block size
#include "Buddy.h" // buddy_header_s
#include "SlabStructs.h" // slab_header_s

namespace os2bn140314d {

	#pragma region Header union

	/**
	 * \brief All the data needed for the allocator. Kept in the first block of allocated memory
	 */
	struct header_s {
		buddy_header_s buddy_header_;	/**< Header used by the buddy allocator */
		slab_header_s slab_header_;		/**< Header used by the slab allocator */
		std::mutex write_mutex_;		/**< Mutex used for console output mutual exclusion */

		/**
		 * \brief Initialize the allocator header
		 * \param first_pool_block Pointer to the first block available
		 * \size_in_blocks Size of the pool in blocks
		 * \throw invalid_argument Thrown when size is too small
		 */
		void initialize(Block *first_pool_block, size_t size_in_blocks) throw (std::invalid_argument);
	};

	/**
	 * \brief First block of allocated memory
	 */
	union Header {
		header_s header_;
		Block block_;
		~Header() = delete;
	};

	#pragma endregion 

	/**
	 * \brief Utility class providing all the helper functions for the allocator
	 */
	class AllocatorUtility final {
	public:

		#pragma region Public interface

		static const size_t MIN_SIZE_IN_BLOCKS = 3; /**< Minimal size of the allocator in blocks */

		/**
		 * \brief Initialize the allocator
		 * \param memory_start Pointer to the memory which the allocator can use
		 * \param size_in_blocks Size of the memory in blocks
		 * \throw invalid_argument Thrown when size is not valid
		 */
		static void initialize(void *memory_start, int size_in_blocks) throw (std::invalid_argument);
		
		/**
		 * \brief Get the pointer to memory used by the allocator
		 * \return Pointer to the memory used by the allocator
		 */
		static void *memoryStart() noexcept;

		/**
		 * \brief Get the pointer to the block where the passed memory is located
		 */
		static const Block *blockStart(const void *memory) noexcept;

		/**
		 * \brief Lock the console for output
		 */
		static void writeLock() noexcept;

		/**
		 * \brief Unlock the console for output
		 */
		static void writeUnlock() noexcept;

		/**
		 * \brief Get the reference to the header block
		 * \return Reference to the header block
		 */
		static header_s &header() noexcept;

		/**
		 * \brief Get the reference to the buddy header
		 * \return Reference to the buddy header
		 */
		static buddy_header_s &buddyHeader() noexcept;

		/**
		 * \brief Get the reference to the slab header
		 * \return Reference to the slab header
		 */
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
