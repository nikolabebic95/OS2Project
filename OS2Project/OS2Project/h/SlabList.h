/**
* \file SlabList.h
* \brief File providing the functions for the manipulation of the lists of slabs
*/

#ifndef _slablist_h_
#define _slablist_h_

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
	private:
		slab_s *first_ = nullptr;
		slab_s *last_ = nullptr;
	};

}

#endif
