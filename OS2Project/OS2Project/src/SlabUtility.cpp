/**
* \file SlabUtility.h
* \brief File implementing the interface for the slab allocator
*/

#include "SlabUtility.h"
#include "AllocatorUtility.h"

namespace os2bn140314d {

	cache_header_s * Slab::create(
		const char name[], 
		size_t object_size, 
		void(*constructor)(void *), 
		void(*destructor)(void *)) noexcept 
	{
		auto &header = AllocatorUtility::slabHeader();
		return header.create(name, object_size, constructor, destructor);
	}

	int Slab::shrink(cache_header_s * cache) noexcept {
		return cache->shrink();
	}

	void *Slab::allocate(cache_header_s * cache) noexcept {
		return cache->allocate();
	}

	void Slab::deallocate(cache_header_s * cache, void * object) noexcept {
		return cache->deallocate(object);
	}

	void *Slab::bufferAllocate(size_t size) noexcept {
		auto &header = AllocatorUtility::slabHeader();
		return header.buffers_[size - slab_header_s::BUFFER_SIZES_LOWER_BOUND]->allocate();
	}

	void Slab::bufferDeallocate(const void *buffer) noexcept {
		auto slab = const_cast<slab_s *>(reinterpret_cast<const slab_s *>(AllocatorUtility::blockStart(buffer)));
		auto cache = slab->header_;
		cache->deallocate(const_cast<void *>(buffer));
	}

	void Slab::destroy(cache_header_s * cache) noexcept {
		auto &header = AllocatorUtility::slabHeader();
		header.destroy(cache);
	}

	void Slab::printInfo(cache_header_s * cache, std::ostream & os) noexcept {
		cache->printInfo(os);
	}

	int Slab::printErrors(cache_header_s * cache, std::ostream & os) noexcept {
		return cache->printErrorInfo(os);
	}
}
