/**
 * \file Slab.cpp
 * \brief Implementation of the slab allocator interface provided by the professors
 */

#include "Slab.h"
#include "AllocatorUtility.h"
#include "SlabUtility.h"
#include <iostream>

using namespace os2bn140314d;

void kmem_init(void *space, int block_num) {
	AllocatorUtility::initialize(space, block_num);
}

kmem_cache_t *kmem_cache_create(const char *name, size_t size, void(*ctor)(void *), void(*dtor)(void *)) {
	auto ret = Slab::create(name, size, ctor, dtor);
	return reinterpret_cast<kmem_cache_t *>(ret);
}

int kmem_cache_shrink(kmem_cache_t *cachep) {
	return Slab::shrink(reinterpret_cast<cache_header_s *>(cachep));
}

void *kmem_cache_alloc(kmem_cache_t *cachep) {
	return Slab::allocate(reinterpret_cast<cache_header_s *>(cachep));
}

void kmem_cache_free(kmem_cache_t *cachep, void *objp) {
	Slab::deallocate(reinterpret_cast<cache_header_s *>(cachep), objp);
}

void *kmalloc(size_t size) {
	return Slab::bufferAllocate(Buddy::sizeToPower(Buddy::greaterOrEqualPowerOfTwo(size)));
}

void kfree(const void *objp) {
	Slab::bufferDeallocate(objp);
}

void kmem_cache_destroy(kmem_cache_t *cachep) {
	Slab::destroy(reinterpret_cast<cache_header_s *>(cachep));
}

void kmem_cache_info(kmem_cache_t *cachep) {
	Slab::printInfo(reinterpret_cast<cache_header_s *>(cachep), std::cout);
}

int kmem_cache_error(kmem_cache_t *cachep) {
	return Slab::printErrors(reinterpret_cast<cache_header_s *>(cachep), std::cerr);
}
