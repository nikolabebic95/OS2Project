#include "SlabStructs.h"

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


}
