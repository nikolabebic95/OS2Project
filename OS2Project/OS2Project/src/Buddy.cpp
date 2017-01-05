#include "Buddy.h"
#include "AllocatorUtility.h"
#include "BlockList.h"

namespace os2bn140314d {
	
	#pragma region Buddy implementation

	void *Buddy::allocate(size_t size) throw (std::bad_alloc, std::invalid_argument) {
		if (size == 0) {
			throw std::invalid_argument("Size must be greater than 0");
		}

		try {
			auto power = greaterOrEqualPowerOfTwo(size);
			auto index = sizeToPower(power);

			return allocatePowerOfTwo(index);
		}
		catch (std::exception &) {
			throw std::bad_alloc();
		}
	}

	void *Buddy::allocatePowerOfTwo(size_t power) throw (std::bad_alloc) {
		auto &header = AllocatorUtility::buddyHeader();

		header.mutex_.lock();

		auto pointer = header.pointers_[power];

		Block *ret;

		if (pointer == nullptr) {
			auto bigger_power = power;
			while (bigger_power < POWERS_OF_TWO && header.pointers_[bigger_power] == nullptr) {
				bigger_power++;
			}

			if (bigger_power == POWERS_OF_TWO) {
				// No more available memory
				header.mutex_.unlock();
				throw std::bad_alloc();
			}

			pointer = header.pointers_[bigger_power];
			ret = BlockList::remove(pointer);

			while (bigger_power != power) {
				bigger_power--;
				
				auto second_pointer = ret + bigger_power;
				second_pointer->info.index = bigger_power;
				BlockList::insert(header.pointers_[bigger_power], second_pointer);
			}
		}
		else {
			ret = BlockList::remove(pointer);
		}

		auto index_of_bitmap = header.indexOfBitmap(ret);
		auto index_in_bitmap = header.indexInBitmap(ret);

		auto num_of_entries = powerToSize(power);

		if (index_in_bitmap + num_of_entries > ENTRIES_IN_BITMAP) {
			header.bitmaps_[index_of_bitmap].allocate(index_in_bitmap, ENTRIES_IN_BITMAP - index_in_bitmap);
			header.bitmaps_[index_of_bitmap + 1].allocate(0, num_of_entries - ENTRIES_IN_BITMAP + index_in_bitmap);
		}
		else {
			header.bitmaps_[index_of_bitmap].allocate(index_in_bitmap, num_of_entries);
		}

		header.mutex_.unlock();

		return ret;
	}

	void Buddy::deallocate(void *memory, size_t size) throw (std::invalid_argument) {
		if (size == 0) {
			// This is not an exception - nothing is deallocated
			return;
		}

		auto power = greaterOrEqualPowerOfTwo(size);
		auto index = sizeToPower(power);

		deallocatePowerOfTwo(memory, index);
	}

	void Buddy::deallocatePowerOfTwo(void * memory, size_t power) throw (std::invalid_argument) {
		auto &header = AllocatorUtility::buddyHeader();

		auto block = reinterpret_cast<Block *>(memory);

		if (block < header.memory_ || block >= header.memory_ + header.number_of_blocks_) {
			throw std::invalid_argument("Memory address not a part of the buddy allocator");
		}

		header.mutex_.lock();

		auto index_of_bitmap = header.indexOfBitmap(block);
		auto index_in_bitmap = header.indexInBitmap(block);
		
		auto num_of_entries = powerToSize(power);

		if (index_in_bitmap + num_of_entries > ENTRIES_IN_BITMAP) {
			header.bitmaps_[index_of_bitmap].deallocate(index_in_bitmap, ENTRIES_IN_BITMAP - index_in_bitmap);
			header.bitmaps_[index_of_bitmap + 1].deallocate(0, num_of_entries - ENTRIES_IN_BITMAP + index_in_bitmap);
		}
		else {
			header.bitmaps_[index_of_bitmap].deallocate(index_in_bitmap, num_of_entries);
		}

		auto current_power = power;
		auto current_block = block;

		while (true) {
			current_block->info.index = current_power;

			BlockList::insert(header.pointers_[current_power], current_block);

			auto left = header.leftBuddy(current_block, current_power);
			auto right = header.rightBuddy(current_block, current_power);

			if (header.isInRange(left) &&
				header.isInRange(right) &&
				header.isFree(left) && 
				header.isFree(right) && 
				left->info.index == current_power && 
				right->info.index == current_power)
			{
				BlockList::remove(header.pointers_[current_power], left);
				BlockList::remove(header.pointers_[current_power], right);

				left->info.index = current_power + 1;
				BlockList::insert(header.pointers_[current_power + 1], left);
			}
			else {
				break;
			}
		}

		header.mutex_.unlock();
	}

	bool Buddy::isPowerOfTwo(size_t number) noexcept {
		return (number & number - 1) == 0;
	}

	bool Buddy::isSignedNegative(size_t number) noexcept {
		// 1) Flip the bits in a zero, get 1..1
		// 2) Shift that to the right, get 01..1
		// 3) Flip the bits, get 10..0
		// 4) AND with number
		// 5) If the result is not 0, the first bit in the number is 1
		auto mask = ~(~static_cast<size_t>(0) >> 1);
		return (number & mask) != 0;
	}

	size_t Buddy::greaterOrEqualPowerOfTwo(size_t number) throw (std::overflow_error) {
		if (isSignedNegative(number)) {
			throw std::overflow_error("Greater power of two is out of size_t range");
		}

		size_t ret = 1;
		while (ret < number) {
			ret <<= 1;
		}

		return ret;
	}

	size_t Buddy::smallerOrEqualPowerOfTwo(size_t number) noexcept {
		auto ret = ~(~static_cast<size_t>(0) >> 1);
		while (ret > number) {
			ret >>= 1;
		}

		return ret;
	}

	size_t Buddy::sizeToPower(size_t number) throw (std::invalid_argument) {
		if (!isPowerOfTwo(number)) {
			throw std::invalid_argument("Number is not a power of two");
		}

		size_t ret = 0;
		size_t mask = 1;
		while ((mask & number) == 0) {
			mask <<= 1;
			ret++;
		}

		return ret;
	}

	size_t Buddy::powerToSize(size_t power) throw (std::out_of_range) {
		if (power > sizeof(size_t) * BITS_IN_BYTE) {
			throw std::out_of_range("Power of two index out of range");
		}

		return 1 << power;
	}

	#pragma endregion 

	#pragma region BitMapBlock implementation

	void BitMapBlock::initialize() noexcept {
		for (auto &byte : bytes) {
			byte = 0;
		}
	}

	void BitMapBlock::allocate(size_t index, size_t size_in_blocks) throw(std::out_of_range) {
		insertValues(index, size_in_blocks, true);
	}

	void BitMapBlock::deallocate(size_t index, size_t size_in_blocks) throw(std::out_of_range) {
		insertValues(index, size_in_blocks, false);
	}

	bool BitMapBlock::isFree(size_t index) const throw(std::out_of_range) {
		auto i = indexOfByte(index);
		auto m = mask(index);
		return (bytes[i] & m) != 0;
	}

	size_t BitMapBlock::indexOfByte(size_t index) throw(std::out_of_range) {
		if (index > ENTRIES_IN_BITMAP) {
			throw std::out_of_range("Bitmap entry index out of range");
		}

		return index / BITS_IN_BYTE;
	}

	unsigned char BitMapBlock::mask(size_t index) throw(std::out_of_range) {
		if (index > ENTRIES_IN_BITMAP) {
			throw std::out_of_range("Bitmap entry index out of range");
		}

		return 1 << index % BITS_IN_BYTE;
	}

	void BitMapBlock::insertValue(size_t index, bool value) throw(std::out_of_range) {
		auto i = indexOfByte(index);
		auto m = mask(index);

		if (value) {
			bytes[i] |= m;
		}
		else {
			bytes[i] &= ~m;
		}
	}

	void BitMapBlock::insertValues(size_t index, size_t size_in_blocks, bool value) throw(std::out_of_range) {
		for (auto i = index; i < index + size_in_blocks; i++) {
			insertValue(i, value);
		}
	}

	#pragma endregion

	#pragma region buddy_header_s implementation

	void buddy_header_s::initialize(Block *first_block, size_t size_in_blocks) throw (std::invalid_argument) {
		if (size_in_blocks < 2) {
			throw std::invalid_argument("Too few blocks for the buddy allocator");
		}

		new (&mutex_) std::mutex();

		initializePointers();
		initializeBitmaps(first_block, size_in_blocks);

		auto remaining_size = size_in_blocks - number_of_bitmaps_;
		auto *remaining_blocks = first_block + number_of_bitmaps_;

		memory_ = remaining_blocks;
		number_of_blocks_ = remaining_size;

		while (remaining_size > 0) {
			auto power = Buddy::smallerOrEqualPowerOfTwo(remaining_size);
			auto index = Buddy::sizeToPower(power);

			remaining_blocks->info.index = index;
			BlockList::insert(pointers_[index], remaining_blocks);

			remaining_blocks += power;
			remaining_size -= power;
		}
	}

	void buddy_header_s::initializePointers() noexcept {
		for (auto &pointer : pointers_) {
			pointer = nullptr;
		}
	}

	void buddy_header_s::initializeBitmaps(Block * first_block, size_t size_in_blocks) noexcept {
		number_of_bitmaps_ = numOfBitmaps(size_in_blocks);

		bitmaps_ = reinterpret_cast<BitMapBlock *>(first_block);

		for (size_t i = 0; i < number_of_bitmaps_; i++) {
			bitmaps_[i].initialize();
		}
	}

	size_t buddy_header_s::numOfBitmaps(size_t size_in_blocks) noexcept {
		auto ret = size_in_blocks / ENTRIES_IN_BITMAP;

		if (size_in_blocks % ENTRIES_IN_BITMAP == 0) {
			return ret;
		}
		else {
			return ret + 1;
		}
	}

	size_t buddy_header_s::indexOfBitmap(Block *block) const throw(std::invalid_argument) {
		if (block == nullptr || block < memory_ || block > memory_ + number_of_blocks_) {
			throw std::invalid_argument("Block not inside allocated space");
		}

		auto dist = block - memory_;
		return dist / ENTRIES_IN_BITMAP;
	}

	size_t buddy_header_s::indexInBitmap(Block *block) const throw(std::invalid_argument) {
		if (block == nullptr || block < memory_ || block > memory_ + number_of_blocks_) {
			throw std::invalid_argument("Block not inside allocated space");
		}

		auto dist = block - memory_;
		return dist % ENTRIES_IN_BITMAP;
	}

	Block *buddy_header_s::leftBuddy(Block *block, size_t power) const throw(std::invalid_argument) {
		auto size = Buddy::powerToSize(power + 1);

		if (block < memory_ || block > memory_ + number_of_blocks_) {
			throw std::invalid_argument("Block not in buddy address space");
		}

		auto diff = block - memory_;

		if (diff % size == 0) {
			return block;
		}
		else {
			return block - size;
		}
	}

	Block * buddy_header_s::rightBuddy(Block * block, size_t power) const throw(std::invalid_argument) {
		auto size = Buddy::powerToSize(power + 1);

		if (block < memory_ || block > memory_ + number_of_blocks_) {
			throw std::invalid_argument("Block not in buddy address space");
		}

		auto diff = block - memory_;

		if (diff % size == 0) {
			return block + size;
		}
		else {
			return block;
		}
	}

	bool buddy_header_s::isFree(Block *block) const throw(std::invalid_argument) {
		if (block < memory_ || block >= memory_ + number_of_blocks_) {
			throw std::invalid_argument("Block not in buddy range");
		}

		auto index_of_bitmap = indexOfBitmap(block);
		auto index_in_bitmap = indexInBitmap(block);

		return bitmaps_[index_of_bitmap].isFree(index_in_bitmap);
	}

	bool buddy_header_s::isInRange(Block *block) const noexcept {
		return block >= memory_ && block < memory_ + number_of_blocks_;
	}

	#pragma endregion  
}
