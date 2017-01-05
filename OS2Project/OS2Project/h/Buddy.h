/**
* \file Buddy.h
* \brief File providing the interface for the buddy allocator
*/

#ifndef _buddy_h_
#define _buddy_h_

#include <stdexcept> // bad_alloc, invalid_argument, overflow_error
#include "Definitions.h"

namespace os2bn140314d {
	
	class Buddy final {
	public:

		friend struct buddy_header_s;

		#pragma region Public interface

		/**
		 * \brief Allocate memory of at least the given size
		 * \param size Size in blocks
		 * \throw bad_alloc Thrown when there is not enough memory
		 */
		static void *allocate(size_t size) throw (std::bad_alloc, std::invalid_argument);

		/**
		 * \brief Allocate the memory of the size 2^size
		 * \param power 2^power is size in blocks
		 * \throw bad_alloc Thrown when there is not enough memory
		 */
		static void *allocatePowerOfTwo(size_t power) throw (std::bad_alloc);

		/**
		 * \brief Deallocate memory
		 * \param memory Pointer to the memory obtained by allocation
		 * \param size Size of the memory passed to \c allocate
		 * \throw invalid_argument Thrown when pointer is out of range
		 */
		static void deallocate(void *memory, size_t size) throw (std::invalid_argument);

		/**
		 * \brief Deallocate memory
		 * \param memory Pointer to the memory obtained by allocation
		 * \param power Power of the memory passed to \c allocatePowerOfTwo
		 * \throw invalid_argument Thrown when pointer is out of range
		 */
		static void deallocatePowerOfTwo(void *memory, size_t power) throw (std::invalid_argument);

		#pragma endregion 

	private:

		#pragma region Delete constructors

		Buddy() = delete;
		Buddy(const Buddy &) = delete;
		void operator=(const Buddy &) = delete;

		#pragma endregion

		#pragma region Helpers

		/**
		 * \brief Check whether the number is a power of two
		 */
		static bool isPowerOfTwo(size_t number) noexcept;

		/**
		 * \brief Check whether the first bit in a number is 1
		 */
		static bool isSignedNegative(size_t number) noexcept;

		/**
		 * \brief Return the smallest power of two greater or equal to the number
		 */
		static size_t greaterOrEqualPowerOfTwo(size_t number) throw (std::overflow_error);

		/**
		 * \brief Return the greatest power of two smaller or equal to the number
		 */
		static size_t smallerOrEqualPowerOfTwo(size_t number) noexcept;

		/**
		 * \brief Conversion from y to x in 2^x = y
		 */
		static size_t powerToIndex(size_t number) throw (std::invalid_argument);

		static size_t indexToPower(size_t index) throw (std::out_of_range);

		#pragma endregion 
	};

	struct BitMapBlock {
	public:
		unsigned char bytes[BLOCK_SIZE];

		void initialize() noexcept;

		void allocate(size_t index, size_t size_in_blocks) throw (std::out_of_range);		
		void deallocate(size_t index, size_t size_in_blocks) throw (std::out_of_range);

		bool isFree(size_t index) const throw (std::out_of_range);

	private:
		static size_t indexOfByte(size_t index) throw (std::out_of_range);
		static unsigned char mask(size_t index) throw (std::out_of_range);

		void insertValue(size_t index, bool value) throw (std::out_of_range);
		void insertValues(size_t index, size_t size_in_blocks, bool value) throw (std::out_of_range);
	};

	struct buddy_header_s {
		friend class Buddy;

	public:
		// TODO: Add more fields
		Block *pointers_[POWERS_OF_TWO];
		size_t number_of_bitmaps_;
		BitMapBlock *bitmaps_;
		Block *memory_;
		size_t number_of_blocks_;

		void initialize(Block *first_block, size_t size_in_blocks) throw (std::invalid_argument);

	private:
		void initializePointers() noexcept;
		void initializeBitmaps(Block *first_block, size_t size_in_blocks) noexcept;

		static size_t numOfBitmaps(size_t size_in_blocks) noexcept;
		size_t indexOfBitmap(Block *block) const throw (std::invalid_argument);
		size_t indexInBitmap(Block *block) const throw (std::invalid_argument);
	};
}

#endif
