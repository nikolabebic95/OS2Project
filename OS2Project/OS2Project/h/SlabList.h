/**
* \file SlabList.h
* \brief File providing the functions for the manipulation of the lists of slabs
*/

#ifndef _slablist_h_
#define _slablist_h_
#include <stdexcept>

namespace os2bn140314d {

	struct slab_s;

	/**
	 * \brief Class representing the header of one list of slabs
	 */
	class SlabList {
	public:
		/**
		 * \brief Insert one element into the back of the list
		 * \param element Pointer to the slab
		 */
		void insert(slab_s *element) noexcept;

		/**
		 * \brief Remove one specific element from the list
		 * \param element Pointer to the slab
		 */
		void remove(slab_s *element) noexcept;

		/**
		 * \brief Get the first element from the list
		 * \throw underflow_error Thrown when the list is empty
		 */
		slab_s *first() const throw(std::underflow_error);

		/**
		 * \brief Check if list is empty
		 * \return True if the list is empty, false otherwise
		 */
		bool isEmpty() const noexcept;

		/**
		 * \brief Searches for the slab containing the object
		 * \param object Pointer to the object
		 * \return Pointer to the slab containing the slab, or null if there is no such slab
		 */
		slab_s *get(void *object) const noexcept;

	private:
		slab_s *first_ = nullptr;
		slab_s *last_ = nullptr;
	};

}

#endif
