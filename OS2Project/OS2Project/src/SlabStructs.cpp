#include "SlabStructs.h"
#include "Buddy.h"
#include <ostream>
#include <iostream>
#include "AllocatorUtility.h"

namespace os2bn140314d {

	#pragma region slab_s implementation

	void slab_s::initialize(size_t color_offset, cache_header_s *header) noexcept {
		header_ = header;

		auto start = reinterpret_cast<byte *>(this);

		// Index array is starting after the slab_s structure, inside the block
		auto index_array_start = start + sizeof(slab_s);

		// Object array is starting after the index array
		// Array start should be offset by color
		auto object_array_start = index_array_start + header_->num_of_objects_ * sizeof(size_t) + color_offset;

		index_array_ = reinterpret_cast<size_t *>(index_array_start);
		objects_start_ = object_array_start;

		initializeIndexArray();
		initializeObjectArray();
	}

	void slab_s::initializeIndexArray() noexcept {
		free_index_ = 0;
		
		for (size_t i = 1; i < header_->num_of_objects_; i++) {
			index_array_[i - 1] = i;
		}

		index_array_[header_->num_of_objects_ - 1] = NULL_INDEX;
	}

	void slab_s::initializeObjectArray() noexcept {
		number_of_allocated_objects_ = 0;

		if (header_->constructor_ == nullptr) {
			return;
		}

		for (size_t i = 0; i < header_->num_of_objects_; i++) {
			auto pointer = objectAt(i);
			header_->constructor_(pointer);
		}
	}

	byte *slab_s::objectAt(size_t index) const throw(std::out_of_range) {
		if (index >= header_->num_of_objects_) {
			throw std::out_of_range("Object index in slab out of range");
		}

		return objects_start_ + index * header_->object_size_;
	}

	size_t slab_s::indexOf(void *object) const throw(std::invalid_argument) {
		if (!objectInRange(object)) {
			throw std::invalid_argument("Object not in slab");
		}

		auto pointer = reinterpret_cast<byte *>(object);

		auto diff = pointer - objects_start_;

		if (diff % header_->object_size_ != 0) {
			throw std::invalid_argument("Invalid pointer to object");
		}

		return diff / header_->object_size_;
	}


	bool slab_s::objectInRange(void *object) const noexcept {
		return object >= objects_start_ && object <= objectAt(header_->num_of_objects_ - 1);
	}

	bool slab_s::contains(void * object) const noexcept {
		if (!objectInRange(object)) {
			return false;
		}

		auto pointer = reinterpret_cast<byte *>(object);

		auto diff = pointer - objects_start_;

		return diff % header_->object_size_ == 0;
	}

	bool slab_s::isEmpty() const noexcept {
		return number_of_allocated_objects_ == 0;
	}

	bool slab_s::isFull() const noexcept {
		return number_of_allocated_objects_ == header_->num_of_objects_;
	}

	void *slab_s::allocate() throw(std::bad_alloc) {
		if (free_index_ == NULL_INDEX) {
			throw std::bad_alloc();
		}

		// There is no special need to mark the object allocated
		// Just say that the first free object is the next one in the list
		auto ret = objectAt(free_index_);
		free_index_ = index_array_[free_index_];

		number_of_allocated_objects_++;

		return ret;
	}

	void slab_s::deallocate(void *object) throw(std::invalid_argument) {
		auto index = indexOf(object);

		if (header_->destructor_ != nullptr) {
			header_->destructor_(object);			
		}

		if (header_->constructor_ != nullptr) {
			header_->constructor_(object);
		}

		// Insert free object to the beginning of the list
		index_array_[index] = free_index_;
		free_index_ = index;

		number_of_allocated_objects_--;
	}

	#pragma endregion 

	#pragma region cache_header_s implementation

	size_t cache_header_s::slabSize(size_t object_size, size_t index_size) throw(std::overflow_error) {
		auto ret = Buddy::greaterOrEqualPowerOfTwo(object_size + index_size + sizeof(cache_header_s));
		if (ret < BLOCK_SIZE) {
			ret = BLOCK_SIZE;
		}

		return ret;
	}

	size_t cache_header_s::numOfObjects(size_t slab_size, size_t object_size, size_t index_size) throw(std::invalid_argument) {
		if (slab_size == 0 || object_size == 0 || index_size == 0) {
			throw std::invalid_argument("Size can not be 0");
		}

		return slab_size / (object_size + index_size);
	}

	size_t cache_header_s::unusedSpace(size_t slab_size, size_t object_size, size_t index_size) throw(std::invalid_argument) {
		if (slab_size == 0 || object_size == 0 || index_size == 0) {
			throw std::invalid_argument("Size can not be 0");
		}

		return slab_size % (object_size + index_size);
	}

	void cache_header_s::initilaze(
		const char name[], 
		size_t object_size,
		void(*constructor)(void *), 
		void(*destructor)(void *), 
		cache_block_header_s * block) noexcept 
	{
		copyName(name);
		object_size_ = object_size;
		constructor_ = constructor;
		destructor_ = destructor;
		block_ = block;

		next_color_ = 0;
		number_of_slabs_ = 0;
		number_of_allocated_objects_ = 0;

		auto slab_size = slabSize(object_size, sizeof(size_t));

		number_of_blocks_in_slab_ = slab_size / BLOCK_SIZE;

		num_of_objects_ = numOfObjects(slab_size - sizeof(cache_header_s), object_size, sizeof(size_t));
		unused_memory_size_ = unusedSpace(slab_size - sizeof(cache_header_s), object_size, sizeof(size_t));

		new (&mutex_) std::mutex;

		new (&full_) SlabList;
		new (&partial_) SlabList;
		new (&empty_) SlabList;

		error_ = OK;
	}

	void cache_header_s::copyName(const char name[]) noexcept {
		for (auto i = 0; i < MAX_NAME_LENGTH; i++) {
			name_[i] = name[i];
			if (name[i] == '\0') {
				break;
			}
		}
	}

	void *cache_header_s::allocate() noexcept {
		mutex_.lock();

		// Check if list with partially full slabs has slabs
		// If it does, allocate from there
		// If the slab becomes full, move it to the list with the full slabs
		if (!partial_.isEmpty()) {
			auto slab = partial_.first();
			auto ret = slab->allocate();

			if (slab->isFull()) {
				partial_.remove(slab);
				full_.insert(slab);
			}

			number_of_allocated_objects_++;

			mutex_.unlock();

			return ret;
		}

		// Check if list with empty slabs has slabs
		// If it does, allocate from there
		// Move the slab to the list with partially allocated slabs
		// Corner case - only one object in each slab
		// In that case move the slab to the list with the full slabs
		if (!empty_.isEmpty()) {
			auto slab = empty_.first();
			auto ret = slab->allocate();
			
			empty_.remove(slab);

			if (num_of_objects_ == 1) {
				full_.insert(slab);
			}
			else {
				partial_.insert(slab);
			}

			number_of_allocated_objects_++;

			mutex_.unlock();

			return ret;
		}

		// Allocate one more slab
		// Allocate object from there
		// Put the slab into the list with partially allocated slabs
		// Corner case - only one object in each slab
		// In that case move the slab to the list with the full slabs
		try {
			auto new_slab = reinterpret_cast<slab_s *>(Buddy::allocate(number_of_blocks_in_slab_));
			new_slab->initialize(next_color_, this);

			next_color_ += CACHE_L1_LINE_SIZE;
			next_color_ %= unused_memory_size_;

			auto ret = new_slab->allocate();

			if (num_of_objects_ == 1) {
				full_.insert(new_slab);
			}
			else {
				partial_.insert(new_slab);
			}

			number_of_slabs_++;
			number_of_allocated_objects_++;

			mutex_.unlock();

			return ret;
		}
		catch(std::bad_alloc &) {
			error_ |= NO_MORE_SPACE;

			mutex_.unlock();

			return nullptr;
		}
	}

	void cache_header_s::deallocate(void *object) noexcept {
		mutex_.lock();

		// Try finding the slab in the list with the full slabs
		// If it is there, deallocate the object
		// Put the slab into the list with the partially full slabs
		// Corner case - only one object in each slab
		// In that case move the slab to the list with the empty slabs
		auto slab = full_.get(object);
		if (slab != nullptr) {
			slab->deallocate(object);
			full_.remove(slab);

			if (num_of_objects_ == 1) {
				empty_.insert(slab);
			}
			else {
				partial_.insert(slab);
			}

			number_of_allocated_objects_--;

			mutex_.unlock();

			return;
		}

		// Try finding the slab in the list with the partially full slabs
		// If it is there, deallocate the object
		// Check if slab became emtpy
		// If it did, move the slab to the list with the empty slabs
		slab = partial_.get(object);
		if (slab != nullptr) {
			slab->deallocate(object);

			if (slab->number_of_allocated_objects_ == 0) {
				partial_.remove(slab);
				empty_.insert(slab);
			}

			number_of_allocated_objects_--;

			mutex_.unlock();

			return;
		}

		// The object does not belong to any slab
		// Update the error info and return
		error_ |= DEALLOCATING_WRONG_OBJECT;

		mutex_.unlock();
	}

	int cache_header_s::shrink() noexcept {
		mutex_.lock();

		auto ret = 0;

		while (!empty_.isEmpty()) {
			auto slab = empty_.first();
			empty_.remove(slab);
			Buddy::deallocate(slab, number_of_blocks_in_slab_);
			ret += number_of_blocks_in_slab_;
		}

		mutex_.unlock();

		return ret;
	}

	void cache_header_s::printInfo(std::ostream & os) noexcept {
		
		mutex_.lock();
		AllocatorUtility::writeLock();

		auto fill_ratio = static_cast<double>(number_of_allocated_objects_) / (number_of_slabs_ * num_of_objects_);

		os << "Name                          -- " << name_ << std::endl;
		os << "Object size                   -- " << object_size_ << "B" << std::endl;
		os << "Cache size                    -- " << number_of_slabs_ * number_of_blocks_in_slab_ << " Blocks" << std::endl;
		os << "Number of slabs               -- " << number_of_slabs_ << std::endl;
		os << "Number of objects in one slab -- " << num_of_objects_ << std::endl;
		os << "Fill ratio                    -- " << fill_ratio << std::endl;

		AllocatorUtility::writeUnlock();
		mutex_.unlock();
	}

	int cache_header_s::printErrorInfo(std::ostream & os) noexcept {
		mutex_.lock();
		AllocatorUtility::writeLock();

		if (error_ == OK) {
			os << "No errors" << std::endl;
			auto ret = error_;
			AllocatorUtility::writeUnlock();
			return ret;
		}
		if (error_ & NO_MORE_SPACE) {
			os << "No more space for the allocation" << std::endl;
		}

		if (error_ & DESTROYING_NON_EMPTY_CACHE) {
			os << "Destroying a non empty cache is illegal" << std::endl;
		}

		if (error_ & DEALLOCATING_WRONG_OBJECT) {
			os << "Deallocating an object from the wrong slab" << std::endl;
		}

		auto ret = error_;
		error_ = OK;

		AllocatorUtility::writeUnlock();
		mutex_.unlock();
		
		return ret;
	}

	#pragma endregion 

	#pragma region cache_block_header_s implementation

	void cache_block_header_s::initialize() noexcept {
		new (&used_) CacheHeaderList;
		new (&unused_) CacheHeaderList;

		auto pointer_to_headers = reinterpret_cast<cache_header_s *>(this + 1);
		auto size = BLOCK_SIZE - sizeof(cache_block_header_s);
		auto num_of_headers = size / sizeof(cache_header_s);

		for (unsigned int i = 0; i < num_of_headers; i++) {
			unused_.insert(pointer_to_headers + i);
		}
	}

	bool cache_block_header_s::hasMoreSpace() const noexcept {
		return !unused_.isEmpty();
	}

	bool cache_block_header_s::isEmpty() const noexcept {
		return used_.isEmpty();
	}

	cache_header_s *cache_block_header_s::create(
		const char name[], 
		size_t object_size, 
		void(*constructor)(void *), 
		void(*destructor)(void *)) throw(std::overflow_error)
	{
		if (unused_.isEmpty()) {
			throw std::overflow_error("No more space for the cache header");
		}

		auto header = unused_.remove();
		header->initilaze(name, object_size, constructor, destructor, this);

		used_.insert(header);

		return header;
	}

	bool cache_block_header_s::contains(cache_header_s *header) const noexcept {
		auto pointer_to_headers = reinterpret_cast<const cache_header_s *>(this + 1);
		auto size = BLOCK_SIZE - sizeof(cache_block_header_s);
		auto num_of_headers = size / sizeof(cache_header_s);

		return header >= pointer_to_headers && header < pointer_to_headers + num_of_headers;
	}

	bool cache_block_header_s::destroy(cache_header_s *header) noexcept {
		if (header->number_of_allocated_objects_ != 0) {
			header->error_ |= DESTROYING_NON_EMPTY_CACHE;
			return false;
		}

		header->shrink();

		used_.remove(header);
		unused_.insert(header);

		return true;
	}

	#pragma endregion 

	#pragma region slab_header_s implementation

	void slab_header_s::initialize() noexcept {
		new (&headers_) CacheBlockList;

		new (&mutex_) std::mutex;

		for (auto i = BUFFER_SIZES_LOWER_BOUND; i < BUFFER_SIZES_UPPER_BOUND; i++) {
			buffers_[i - BUFFER_SIZES_LOWER_BOUND] = create("Buffer", Buddy::powerToSize(i), nullptr, nullptr);
		}
	}

	cache_header_s *slab_header_s::create(
		const char name[], 
		size_t object_size,
		void(*constructor)(void *), 
		void(*destructor)(void *)) noexcept
	{
		mutex_.lock();

		// If there are header blocks in the list
		// Find the first block that has more space
		// Allocate from there
		if (!headers_.isEmpty()) {
			auto header_block = headers_.first();

			while (header_block != nullptr && !header_block->hasMoreSpace()) {
				header_block = header_block->next_;
			}

			if (header_block != nullptr) {
				auto ret = header_block->create(name, object_size, constructor, destructor);
				mutex_.unlock();
				return ret;
			}
		}

		// If there are no blocks, or all are full, allocate one more
		// Allocate cache from there
		try {
			auto header_block = reinterpret_cast<cache_block_header_s *>(Buddy::allocate(1));
			header_block->initialize();
			auto ret = header_block->create(name, object_size, constructor, destructor);

			headers_.insert(header_block);

			mutex_.unlock();

			return ret;
		}
		catch(std::bad_alloc &) {
			// If there is no more space, just return nullptr
			mutex_.unlock();
			return nullptr;
		}
	}

	bool slab_header_s::destroy(cache_header_s *header) noexcept {
		mutex_.lock();

		auto header_block = headers_.first();
		
		// Pass through the list, find the block with the header
		// Destroy the cache when block is found
		// If the block is empty remove and deallocate it
		while (header_block != nullptr) {
			if (header_block->contains(header)) {
				auto ret = header_block->destroy(header);

				if (header_block->isEmpty()) {
					headers_.remove(header_block);
					Buddy::deallocate(header_block, 1);
				}

				mutex_.unlock();
				return ret;
			}

			header_block = header_block->next_;
		}

		mutex_.unlock();

		return false;
	}

	#pragma endregion 
}
