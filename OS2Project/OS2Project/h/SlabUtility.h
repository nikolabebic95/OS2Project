/**
* \file SlabUtility.h
* \brief File providing the interface for the slab allocator
*/

#ifndef _slabutility_h_
#define _slabutility_h_

#include "SlabStructs.h"

namespace os2bn140314d {

	/**
	 * \brief Class providing interface for the slab allocator
	 */
	class Slab final {
	public:
		#pragma region Public interface

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
		static cache_header_s *create(
			const char name[],
			size_t object_size,
			void(*constructor)(void *),
			void(*destructor)(void *)) noexcept;

		/**
		* \brief Shrink cache
		* \param cache Pointer to the cache
		* \return Number of deallocated blocks
		*/
		static int shrink(cache_header_s *cache) noexcept;

		/**
		* \brief Allocate one object from cache
		* \param cache Pointer to the cache
		* \return Allocated object
		*/
		static void *allocate(cache_header_s *cache) noexcept;

		/**
		* \brief Deallocate one object from cache
		* \param cache Pointer to the cache
		* \param object Pointer to the object
		*/
		static void deallocate(cache_header_s *cache, void *object) noexcept;

		/**
		* \brief Allocate one small memory buffer
		* \param size Size of the buffer
		* \return Pointer to the allocated buffer
		*/
		static void *bufferAllocate(size_t size) noexcept;

		/**
		* \brief Deallocate one small memory buffer
		* \param buffer Pointer to a buffer obtained by \c bufferAllocate
		*/
		static void bufferDeallocate(const void *buffer) noexcept;

		/**
		* \brief Deallocate cache
		* \param cache Pointer to the cache
		*/
		static void destroy(cache_header_s *cache) noexcept;

		/**
		* \brief Print cache info
		* \param cache Pointer to the cache
		* \param os Output stream
		*/
		static void printInfo(cache_header_s *cache, std::ostream &os) noexcept;

		/**
		* \brief Print error message
		* \param cache Pointer to the cache
		* \param os Output stream
		*/
		static int printErrors(cache_header_s *cache, std::ostream &os) noexcept;

		#pragma endregion 

	private:

		#pragma region Delete constructors

		Slab() = delete;
		Slab(const Slab &) = delete;
		void operator=(const Slab &) = delete;

		#pragma endregion 

	};

}

#endif
