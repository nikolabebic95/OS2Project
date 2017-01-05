/**
 * \file Slab.h
 * \brief Interface for the cache system provided by the professors
 */

#ifndef _slab_h_
#define _slab_h_

#include "Definitions.h" // kmem_cache_t

/**
 * \brief Initialize the allocator
 * \param space Pointer to the memory which the allocator can use
 * \param block_num Size of the memory in blocks
 */
void kmem_init(void *space, int block_num);

/**
 * \brief Allocate cache
 * \param name Name of the cache
 * \param size Size of the cache
 * \param ctor Constructor
 * \param dtor Destructor
 * \return Cache object
 * 
 * If there is no need for a constructor or destructor,
 * the user should pass nullptr in their place
 */
kmem_cache_t *kmem_cache_create(const char *name, size_t size, void(*ctor)(void *), void(*dtor)(void *));

/**
 * \brief Shrink cache
 * \param cachep Pointer to the cache
 * \return Number of deallocated blocks
 */
int kmem_cache_shrink(kmem_cache_t *cachep);

/**
 * \brief Allocate one object from cache
 * \param cachep Pointer to the cache
 */
void *kmem_cache_alloc(kmem_cache_t *cachep);

/**
 * \brief Deallocate one object from cache
 * \param cachep Pointer to the cache
 * \param objp Pointer to the object
 */
void kmem_cache_free(kmem_cache_t *cachep, void *objp);

/**
 * \brief Allocate one small memory buffer
 * \size Size of the buffer
 * \return Pointer to the allocated buffer
 */
void *kmalloc(size_t size);

/**
 * \brief Deallocate one small memory buffer
 * \objp Pointer to a buffer obtained by \c kmalloc
 */
void kfree(const void *objp);

/**
 * \brief Deallocate cache
 * \param cachep Pointer to the cache
 */
void kmem_cache_destroy(kmem_cache_t *cachep);

/**
 * \brief Print cache info
 * \param cachep Pointer to the cache
 */
void kmem_cache_info(kmem_cache_t *cachep);

/**
 * \brief Print error message
 * \param cachep Pointer to the cache
 */
int kmem_cache_error(kmem_cache_t *cachep);

#endif
