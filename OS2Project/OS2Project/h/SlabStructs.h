/**
* \file SlabStructs.h
* \brief File providing the definitions of the structures used by the slab allocator
*/

#ifndef _slabstructs_h_
#define _slabstructs_h_

#include <mutex> // Mutex
#include "Definitions.h" // MAX_NAME_LENGTH
#include "SlabList.h"
#include "CacheHeaderList.h"
#include "CacheBlockList.h"

namespace os2bn140314d {
	struct cache_header_s;

	/**
	 * \brief Struct representing one slab
	 */
	struct slab_s {
		#pragma region Fields

		slab_s *next_;							/**< Pointer to the next slab */
		slab_s *prev_;							/**< Pointer to the previous slab*/

		size_t *index_array_;					/**< Pointer to the array indexing the slab */
		size_t free_index_;						/**< Index of first free object */
		size_t number_of_allocated_objects_;	/**< Number of allocated objects */

		byte *objects_start_;					/**< Pointer to the start of the object array */

		cache_header_s *header_;				/**< Pointer to the cache header struct */

		#pragma endregion 

		#pragma region Methods

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
		 * \brief Checks if pointer to object is valid for this slab
		 * \param object Pointer to the object
		 * \return True if pointer is valid, false otherwise
		 */
		bool contains(void *object) const noexcept;

		/**
		 * \brief Checks if slab is empty
		 * \return True if slab is empty, false otherwise
		 */
		bool isEmpty() const noexcept;

		/**
		 * \brief Checks if slab is full
		 * \return True if slab is full, false otherwise
		 */
		bool isFull() const noexcept;

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

		#pragma endregion 
	};

	struct cache_block_header_s;

	struct cache_header_s {
		#pragma region Fields

		SlabList full_;							/**< List of full slabs */
		SlabList partial_;						/**< List of partially full slabs */
		SlabList empty_;						/**< List of empty slabs */

		size_t object_size_;					/**< Size of one object in cache */

		size_t number_of_blocks_in_slab_;		/**< Number of memory blocks in one slab */
		size_t num_of_objects_;					/**< Number of objects that can fit in one slab */

		size_t next_color_;						/**< The color of the slab that will be allocated next */
		size_t unused_memory_size_;				/**< Size of unused memory in each slab */

		char name_[MAX_NAME_LENGTH];			/**< Human readable name of the cache */

		std::mutex mutex_;						/**< Mutex used for synhronization in the cache */

		void(*constructor_)(void *);			/**< Constructor of the cache objects */
		void(*destructor_)(void *);				/**< Destructor of the cache objects */
		
		size_t number_of_slabs_;				/**< Number of slabs in cache */
		size_t number_of_allocated_objects_;	/**< Number of allocated objects in cache */

		cache_block_header_s *block_;				/**< Pointer to the block where this header is kept */

		cache_header_s *next_;					/**< Pointer to the next cache header in list */
		cache_header_s *prev_;					/**< Pointer to the previous cache header in list */

		AllocatorError error_;					/**< Error info about the cache */

		#pragma endregion 

		#pragma region Helpers

		/**
		 * \brief Calculate the size of the slab
		 * \param object_size Size of one object
		 * \param index_size Size of one element indexing the slab
		 * \return Size of the slab in bytes
		 * \throw overflow_error Thrown when size is bigger then the maximum allocation size
		 */
		static size_t slabSize(size_t object_size, size_t index_size) throw(std::overflow_error);

		/**
		 * \brief Calculate the number of objects that can fit in one slab
		 * \param slab_size Size of the slab available for the objects and index
		 * \param object_size Size of one object
		 * \param index_size Size of one element indexing the slabd
		 * \return Number of objects
		 * \throw invalid_argument Thrown when one of the parameters is 0
		 */
		static size_t numOfObjects(size_t slab_size, size_t object_size, size_t index_size) throw(std::invalid_argument);

		/**
		* \brief Calculate the size of the unused space in one slab
		* \param slab_size Size of the slab available for the objects and index
		* \param object_size Size of one object
		* \param index_size Size of one element indexing the slabd
		* \return Size of the unused space in bytes
		* \throw invalid_argument Thrown when one of the parameters is 0
		*/
		static size_t unusedSpace(size_t slab_size, size_t object_size, size_t index_size) throw(std::invalid_argument);

		#pragma endregion 

		#pragma region Methods

		/**
		 * \brief Initialize the cache
		 * \param name Human readable name of the cache
		 * \param object_size Size of the object
		 * \param constructor Constructor
		 * \param destructor Destructor
		 * \param block Pointer to the block where the header is located
		 */
		void initilaze(
			const char name[],
			size_t object_size,
			void(*constructor)(void *),
			void(*destructor)(void *),
			cache_block_header_s *block) noexcept;

		/**
		 * \brief Copies at most \c MAX_NAME_LENGTH characters into the name string
		 * \param name Name of the cache
		 */
		void copyName(const char name[]) noexcept;

		/**
		 * \brief Allocate one object from cache
		 * \remarks If the allocation is not successfull, error bit is set
		 */
		void *allocate() noexcept;

		/**
		 * \brief Deallocate one object from cache
		 * \param object Pointer to the object
		 * \remarks If the pointer is not valid, error bit is set
		 */
		void deallocate(void *object) noexcept;

		/**
		 * \brief Deallocate all empty slabs
		 * \return Number of blocks deallocated
		 */
		int shrink() noexcept;

		/**
		 * \brief Print info about the cache
		 * \param os Output stream
		 */
		void printInfo(std::ostream &os) noexcept;

		/**
		 * \brief Print error message for the cache
		 * \param os Output stream
		 * \return Error code
		 */
		int printErrorInfo(std::ostream &os) noexcept;

		#pragma endregion 
	};

	struct cache_block_header_s {
		#pragma region Fields
		
		cache_block_header_s *next_;
		cache_block_header_s *prev_;

		CacheHeaderList used_;
		CacheHeaderList unused_;

		#pragma endregion 

		#pragma region Methods

		/**
		 * \brief Initialize the block header
		 */
		void initialize() noexcept;

		/**
		 * \brief Checks if the block can allocate more cache headers
		 * \return True if additional headers can be allocated, false otherwise
		 */
		bool hasMoreSpace() const noexcept;

		/**
		 * \brief Checks if the block has any allocated cache headers
		 * \return True if there are no allocated cache headers, false otherwise
		 */
		bool isEmpty() const noexcept;

		/**
		 * \brief Allocate one more cache header
		 * \param name Human readable name of the cache
		 * \param object_size Size of the cache object
		 * \param constructor Constructor
    	 * \param destructor Destructor
    	 * \throw overflow_error If there is no more space for the header
		 */
		cache_header_s *create(
			const char name[],
			size_t object_size,
			void(*constructor)(void *),
			void(*destructor)(void *)) throw(std::overflow_error);


		/**
		 * \brief Check if block contains the header
		 * \param header Pointer to the cache header
		 * \return True if the block contains the header, false otherwise
		 */
		bool contains(cache_header_s *header) const noexcept;

		/**
		 * \brief Destroy the cache
		 * \param header Pointer to the cache header
		 * \return True if the destruction was successfull, false otherwise
		 * \remarks If the cache is not empty, it will not be destroyed, and error bit will be set
		 */
		bool destroy(cache_header_s *header) noexcept;

		#pragma endregion 
	};

	union CacheHeaderBlock {
		cache_block_header_s info;
		Block block;
	};

	struct slab_header_s {
		static const size_t BUFFER_SIZES_LOWER_BOUND = 5;
		static const size_t BUFFER_SIZES_UPPER_BOUND = 17;

		#pragma region Fields

		CacheBlockList headers_;

		std::mutex mutex_;

		cache_header_s *buffers_[BUFFER_SIZES_UPPER_BOUND - BUFFER_SIZES_LOWER_BOUND];

		#pragma endregion 

		#pragma region Methods

		/**
		 * \brief Initialize the header
		 */
		void initialize() noexcept;

		/**
		* \brief Allocate cache
		* \param name Name of the cache
		* \param object_size Size of the object in cache
		* \param constructor Constructor
		* \param destructor Destructor
		* \return Cache object
		*
		* If there is no need for a constructor or destructor,
		* the user should pass nullptr in their place
		*/
		cache_header_s *create(
			const char name[],
			size_t object_size,
			void(*constructor)(void *),
			void(*destructor)(void *)) noexcept;

		/**
		 * \brief Deallocate one cache
		 * \param header Pointer to the cache header
		 * \return True if the deallocation was successfull, false otherwise
		 */
		bool destroy(cache_header_s *header) noexcept;

		#pragma endregion 
	};
}

#endif
