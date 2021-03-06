/**
* \file BlockList.h
* \brief File providing the functions for the manipulation of the lists of free blocks
*/

#ifndef _blocklist_h_
#define _blocklist_h_

#include "Definitions.h" // Block
#include <stdexcept> // Exceptions

namespace os2bn140314d {

	/**
	 * \brief Helper class used for keeping track of unused blocks
	 */
	class BlockList final {
	public:

		#pragma region Public interface

		/**
		 * \brief Insert one block into the list
		 * \param head Reference to the pointer to the beginning of the list
		 * \param new_block Pointer to the block which should be inserted
		 */
		static void insert(Block *&head, Block *new_block) throw (std::invalid_argument);

		/**
		 * \brief Remove one block from the list, and return it
		 * \param head Pointer to the head of the list
		 * \return Pointer to the removed block
		 * \throw underflow_error Thrown when the list is empty
		 */
		static Block *remove(Block *&head) throw (std::underflow_error);

		/**
		 * \brief Remove one specific block from the list
		 * \param head Pointer to the head of the list
		 * \param block Pointer to the block
		 * \throw invalid_argument Thrown when the list is empty
		 */
		static void remove(Block *&head, Block *block) throw (std::invalid_argument);

		#pragma endregion 

	private:

		#pragma region Delete constructors

		BlockList() = delete;
		BlockList(const BlockList &) = delete;
		void operator=(const BlockList &) = delete;

		#pragma endregion

	};

}
#endif
