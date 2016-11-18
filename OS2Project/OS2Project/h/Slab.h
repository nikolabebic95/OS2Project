/**
 * \file Slab.h
 * \brief Interface for the cache system provided by the professors
 * TODO: Documentation
 */

#ifndef _slab_h_
#define _slab_h_

typedef struct kmem_cache_s kmem_cache_t;

#define BLOCK_SIZE (4096)
#define CACHE_L1_LINE_SIZE (64)

/**
 * \brief Initialize
 * TODO: Document parameters
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
 * TODO: Document parameter and return value
 */
int kmem_cache_shrink(kmem_cache_t *cachep);

/**
 * \brief Allocate one object from cache
 * TODO: Document parameter
 */
void *kmem_cache_alloc(kmem_cache_t *cachep);

/**
 * \brief Deallocate one object from cache
 * TODO: Document parameter
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
 * TODO: Document parameter
 */
void kmem_cache_destroy(kmem_cache_t *cachep);

/**
 * \brief Print cache info
 * TODO: Document parameter
 */
void kmem_cache_info(kmem_cache_t *cachep);

/**
 * \brief Print error message
 * TODO: DOcument parameter
 */
int kmem_cache_error(kmem_cache_t *cachep);

#endif
