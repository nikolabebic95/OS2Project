/**
* \file CacheHeaderList.h
* \brief File providing the functions for the manipulation of the lists of cache header blocks
*/

#ifndef _cacheblocklist_h_
#define _cacheblocklist_h_

#include <stdexcept> // underflow_error

namespace os2bn140314d {
	struct cache_block_header_s;

	/**
	 * \brief Class providing the functions for the manipulations of the lists of cache header blocks
	 */
	class CacheBlockList {
	public:
		/**
		 * \brief Insert one block into the list
		 * \param element Pointer to the header block
		 */
		void insert(cache_block_header_s *element) noexcept;

		/**
		 * \brief Remove one specific block from the list
		 * \param element Pointer to the header block
		 */
		void remove(cache_block_header_s *element) noexcept;

		/**
		 * \brief Check if list is empty
		 * \return True if the list is empty, false otherwise
		 */
		bool isEmpty() const noexcept;

		/**
		 * \brief Get the first block in the list
		 * \return Pointer to the first block
		 * \throw underflow_error Thrown if the list is empty
		 */
		cache_block_header_s *first() const throw(std::underflow_error);

	private:
		cache_block_header_s *first_ = nullptr;
	};
}

#endif
