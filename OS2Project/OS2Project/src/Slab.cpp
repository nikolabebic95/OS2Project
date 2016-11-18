/**
 * \file Slab.cpp
 * \brief Implementation of the slab allocator interface provided by the professors
 */

#include "Slab.h"

void kmem_init(void *space, int block_num) {
	// TODO: Implementation
}

kmem_cache_t * kmem_cache_create(const char *name, size_t size, void(*ctor)(void *), void(*dtor)(void *)) {
	// TODO: Implementation
	return nullptr;
}

int kmem_cache_shrink(kmem_cache_t *cachep) {
	// TODO: Implementation
	return 0;
}

void * kmem_cache_alloc(kmem_cache_t *cachep) {
	// TODO: Implementation
	return nullptr;
}

void kmem_cache_free(kmem_cache_t *cachep, void *objp) {
	// TODO: Implementation
}

void * kmalloc(size_t size) {
	// TODO: Implementation
	return nullptr;
}

void kfree(const void *objp) {
	// TODO: Implementation
}

void kmem_cache_destroy(kmem_cache_t *cachep) {
	// TODO: Implementation
}

void kmem_cache_info(kmem_cache_t *cachep) {
	// TODO: Implementation
}

int kmem_cache_error(kmem_cache_t *cachep) {
	// TODO: Implementation
	return 0;
}
