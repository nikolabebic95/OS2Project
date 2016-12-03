/**
 * \file AllocatorUtility.h
 * \brief File providing the helper functions for the allocator
 */

#ifndef _allocatorutility_h_
#define _allocatorutility_h_

namespace general {

	/**
	 * \brief Utility class providing all the helper functions for the allocator
	 */
	class AllocatorUtility final {
	public:
		// TODO: Provide public interface

		/**
		 * \brief Initialize the allocator
		 * \param memory_start Pointer to the memory which the allocator can use
		 * \param size_in_blocks Size of the memory in blocks
		 */
		static void initialize(void *memory_start, int size_in_blocks);
		
		/**
		 * \brief Get the pointer to memory used by the allocator
		 * \return Pointer to the memory used by the allocator
		 */
		static void *memoryStart();

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
