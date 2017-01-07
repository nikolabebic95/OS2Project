/**
* \file SlabStructs.h
* \brief File providing the definitions of the structures used by the slab allocator
*/

#ifndef _slabstructs_h_
#define _slabstructs_h_

#include <mutex> // Mutex
#include "Definitions.h" // MAX_NAME_LENGTH
#include "SlabList.h"

namespace os2bn140314d {
	struct cache_header_s;

	/**
	 * \brief Struct representing one slab
	 */
	struct slab_s {
		slab_s *next_;							/**< Pointer to the next slab */
		slab_s *prev_;							/**< Pointer to the previous slab*/

		size_t *index_array_;					/**< Pointer to the array indexing the slab */
		size_t free_index_;						/**< Index of first free object */
		size_t number_of_allocated_objects_;	/**< Number of allocated objects */

		byte *objects_start_;					/**< Pointer to the start of the object array */

		cache_header_s *header_;				/**< Pointer to the cache header struct */

		/**
		 * \brief Initialize the slab
		 * \param color_offset Number of bytes that the objects should be offset
		 * \param header Pointer to the parent \c cache_header_s struct
		 */
		void initialize(size_t color_offset, cache_header_s *header) noexcept;

		/**
		 * \brief Initialize the array indexing the slab
		 */
		void initializeIndexArray() noexcept;

		/**
		 * \brief Initialize the objects in the slab
		 */
		void initializeObjectArray() noexcept;

		/**
		 * \brief Get the object at the specific index
		 * \param index Index of the object
		 * \return Pointer to the object
		 * \throw out_of_range Thrown when index is out of range
		 */
		byte *objectAt(size_t index) const throw (std::out_of_range);

		/**
		 * \brief Get the index of the specific object
		 * \param object Pointer to the object
		 * \return Index of the object
		 * \throw invalid_argument Thrown when pointer is out of range, or it does not point to an object
		 */
		size_t indexOf(void *object) const throw(std::invalid_argument);

		/**
		 * \brief Checks if object is in slab range
		 * \param object Pointer to the object
		 * \return True if object is in range, false otherwise
		 */
		bool objectInRange(void *object) const noexcept;

		/**
		 * \brief Allocate one object from slab
		 * \throw bad_alloc Thrown if there are no free objects in slab
		 */
		void *allocate() throw(std::bad_alloc);

		/**
		 * \brief Deallocate one object from slab
		 * \param object Pointer to the object
		 * \throw invalid_argument Thrown when pointer does not point to an object in slab
		 */
		void deallocate(void *object) throw(std::invalid_argument);
	};

	union CacheHeaderBlock;

	struct cache_header_s {
		SlabList full_;
		SlabList partial_;
		SlabList empty_;

		size_t object_size_;

		size_t num_of_objects_;

		size_t next_color_;
		size_t unused_memory_size_;

		char name_[MAX_NAME_LENGTH];

		std::mutex mutex_;

		void(*constructor_)(void *);
		void(*destructor_)(void *);

		size_t number_of_slabs_;
		size_t number_of_allocated_objects_;

		CacheHeaderBlock *block_;

		size_t next_free_header_index_;
	};

	struct cache_block_header_s {
		CacheHeaderBlock *next_;
		CacheHeaderBlock *prev_;

		size_t number_of_headers_;

		cache_header_s *headers_start_;
		size_t first_free_header_index_;
	};

	union CacheHeaderBlock {
		cache_block_header_s info;
		Block block;
	};
}

#endif
