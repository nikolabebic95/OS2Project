/**
* \file Buddy.h
* \brief File providing the interface for the buddy allocator
*/

#ifndef _buddy_h_
#define _buddy_h_

#include <stdexcept> // Exceptions
#include <mutex> // mutex
#include "Definitions.h" // Constants

namespace os2bn140314d {
	
	/**
	 * \brief Utility class providing interface for the buddy allocator
	 */
	class Buddy final {
	public:
		#pragma region Public interface

		/**
		 * \brief Allocate memory of at least the given size
		 * \param size Size in blocks
		 * \throw bad_alloc Thrown when there is not enough memory
		 * \throw invalid_argument Thrown when passed size is 0
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

		#pragma region Helpers

		/**
		* \brief Check whether the number is a power of two
		* \param number Number to check
		* \return True if power of two, false otherwise
		*/
		static bool isPowerOfTwo(size_t number) noexcept;

		/**
		* \brief Check whether the first bit in a number is 1
		* \param number Number to check
		* \return True if first bit is 1, false otherwise
		*/
		static bool isSignedNegative(size_t number) noexcept;

		/**
		* \brief Return the smallest power of two greater or equal to the number
		* \param number Number to compare
		* \return Power of two
		* \throw overflow_error Thrown if no greater power of two in \c size_t range
		*/
		static size_t greaterOrEqualPowerOfTwo(size_t number) throw (std::overflow_error);

		/**
		* \brief Return the greatest power of two smaller or equal to the number
		* \param number Number to compare
		* \return Power of two
		*/
		static size_t smallerOrEqualPowerOfTwo(size_t number) noexcept;

		/**
		* \brief Conversion from y to x in 2^x = y
		* \param number y in equation
		* \return x in equation
		* \throw invalid_argument Thrown when number is not a power of two
		*/
		static size_t sizeToPower(size_t number) throw (std::invalid_argument);

		/**
		* \brief Conversion from x to y in 2^x = y
		* \param power x in equation
		* \return y in equation
		* \throw out_of_range Thrown when y is not in \c size_t range
		*/
		static size_t powerToSize(size_t power) throw (std::out_of_range);

		#pragma endregion 

	private:

		#pragma region Delete constructors

		Buddy() = delete;
		Buddy(const Buddy &) = delete;
		void operator=(const Buddy &) = delete;

		#pragma endregion
	};

	#pragma region Structs

	/**
	 * \brief Struct representing one block of memory that acts as a bitmap indexing other blocks
	 */
	struct BitMapBlock {
		byte bytes[BLOCK_SIZE];

		/**
		 * \brief Initialize bitmap
		 */
		void initialize() noexcept;

		/**
		 * \brief Allocate a number of blocks
		 * \param index Starting index of the blocks that should be allocated
		 * \param size_in_blocks Number of blocks that should be allocated
		 * \throw out_of_range Thrown when allocated blocks are not in range
		 * \remarks Does not check if memory was already allocated
		 */
		void allocate(size_t index, size_t size_in_blocks) throw (std::out_of_range);

		/**
		 * \brief Deallocate a number of blocks
		 * \param index Starting index of the blocks that should be allocated
		 * \param size_in_blocks Number of blocks that should be allocated
		 * \throw out_of_range Thrown when allocated blocks are not in range
		 * \remarks Does not check if memory was already free
		 */
		void deallocate(size_t index, size_t size_in_blocks) throw (std::out_of_range);

		/**
		 * \brief Check whether the block is free
		 * \param index Index of a block
		 * \return True if block is free, false otherwise
		 * \throw out_of_range Thrown when block index is out of range
		 */
		bool isFree(size_t index) const throw (std::out_of_range);

		/**
		 * \brief Get the index of a byte where the block's bit is located
		 * \param index Index of the block
		 * \return Index of a byte
		 * \throw out_of_range Thrown when block index is out of range
		 */
		static size_t indexOfByte(size_t index) throw (std::out_of_range);

		/**
		 * \brief Get the mask (form -> 0*10*) of the blocks bit
		 * \param index Index of the block
		 * \return Mask with one bit set
		 * \throw out_of_range Thrown when block index is out of range
		 */
		static byte mask(size_t index) throw (std::out_of_range);

		/**
		 * \brief Set or reset a specific bit
		 * \param index Index where the value should be inserted
		 * \value Value which should be inserted
		 * \throw out_of_range Thrown when index is out of range
		 */
		void insertValue(size_t index, bool value) throw (std::out_of_range);
		
		/**
		 * \brief Set or reset a part of the bitmap
		 * \param index Starting index
		 * \param size_in_blocks Number of elements
		 * \param value Value which is inserted (1 - set, 0 - reset)
		 * \throw out_of_range Thrown when index is not in range
		 */
		void insertValues(size_t index, size_t size_in_blocks, bool value) throw (std::out_of_range);
	};

	/**
	 * \brief Header needed by buddy allocator
	 */
	struct buddy_header_s {
		Block *pointers_[POWERS_OF_TWO];
		size_t number_of_bitmaps_;
		BitMapBlock *bitmaps_;
		Block *memory_;
		size_t number_of_blocks_;
		std::mutex mutex_;

		/**
		 * \brief Initialize the struct
		 * \param first_block Pointer to the first block available to the buddy allocator
		 * \param size_in_blocks Number of blocks available to the buddy allocator
		 * \throw invalid_argument Thrown when size is too small
		 */
		void initialize(Block *first_block, size_t size_in_blocks) throw (std::invalid_argument);

		/**
		 * \brief Initialize pointers to the lists of buddies
		 */
		void initializePointers() noexcept;

		/**
		 * \brief Initialize the bitmaps
		 * \param first_block Pointer to the first block available to the buddy allocator
		 * \param size_in_blocks Number of blocks available to the buddy allocator
		 */
		void initializeBitmaps(Block *first_block, size_t size_in_blocks) noexcept;

		/**
		 * \brief Calculate the number of bitmaps needed for the given number of blocks
		 * \size_in_blocks Number of blocks
		 * \return Number of bitmap blocks
		 */
		static size_t numOfBitmaps(size_t size_in_blocks) noexcept;

		/**
		 * \brief Calculate the index of bitmap where the information about the block is stored
		 * \param block Pointer to the block
		 * \throw invalid_argument Thrown when buddy is not responsible for the given block
		 */
		size_t indexOfBitmap(Block *block) const throw (std::invalid_argument);

		/**
		 * \brief Calculate the index of block inside the bitmap
		 * \param block Pointer to the block
		 * \throw invalid_argument Thrown when buddy is not responsible for the given block
		 */
		size_t indexInBitmap(Block *block) const throw (std::invalid_argument);

		/**
		 * \brief Get the left buddy of the block.
		 * \param block Pointer to the block
		 * \param power Block size
		 * \return If the block itself is the left buddy, the block, left buddy otherwise
		 * \throw invalid_argument Thrown when buddy is not responsible for the given block
		 */
		Block *leftBuddy(Block *block, size_t power) const throw (std::invalid_argument);

		/**
		* \brief Get the right buddy of the block.
		* \param block Pointer to the block
		* \param power Block size
		* \return If the block itself is the right buddy, the block, right buddy otherwise
		* \throw invalid_argument Thrown when buddy is not responsible for the given block
		*/
		Block *rightBuddy(Block *block, size_t power) const throw (std::invalid_argument);

		/**
		 * \brief Check if the block is free
		 * \param block Pointer to the block
		 * \return True if block is free, false otherwise
		 * \throw invalid_argument Thrown when buddy is not responsible for the given block
		 */
		bool isFree(Block *block) const throw (std::invalid_argument);

		/**
		 * \brief Check if buddy is responsible for the block
		 * \param block Pointer to the block
		 * \return True if the block is in range, false otherwise
		 */
		bool isInRange(Block *block) const noexcept;
	};

	#pragma endregion 
}

#endif
