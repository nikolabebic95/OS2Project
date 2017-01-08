/**
* \file CacheHeaderList.h
* \brief File providing the functions for the manipulation of the lists of cache headers
*/

#ifndef _cacheheaderlist_h_
#define _cacheheaderlist_h_

#include <stdexcept> // underflow_error

namespace os2bn140314d {

	struct cache_header_s;

	/**
	 * \brief Class implementing the list of cache headers
	 */
	class CacheHeaderList {
	public:
		/**
		 * \brief Insert one element into the list
		 * \param element Pointer to cache header
		 */
		void insert(cache_header_s *element) noexcept;

		/**
		 * \brief Remove one specific element from the list
		 * \param element Pointer to the cache header
		 */
		void remove(cache_header_s *element) noexcept;

		/**
		 * \brief Remove first element from the list
		 * \return Pointer to the first element
		 */
		cache_header_s *remove() throw (std::underflow_error);

		/**
		 * \brief Checks if list is empty
		 * \return True if the list is empty, false otherwise
		 */
		bool isEmpty() const noexcept;

	private:
		cache_header_s *first_ = nullptr;
	};
}

#endif
