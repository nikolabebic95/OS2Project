/**
* \file BlockList.cpp
* \brief Implementation of the functions manipulating the lists of free blocks
*/

#include "BlockList.h"

namespace os2bn140314d {

	void BlockList::insert(Block *&head, Block *new_block) throw(std::invalid_argument) {
		if (new_block == nullptr) {
			throw std::invalid_argument("Block which should be inserted is nullptr");
		}

		new_block->info.next = head;
		new_block->info.prev = nullptr;		

		if (head != nullptr) {
			head->info.prev = new_block;
		}

		head = new_block;
	}

	Block *BlockList::remove(Block *&head) throw(std::underflow_error) {
		if (head == nullptr) {
			throw std::underflow_error("List of blocks is empty");
		}

		auto old_head = head;
		head = head->info.next;

		if (head != nullptr) {
			head->info.prev = nullptr;
		}

		old_head->info.next = nullptr;
		return old_head;
	}

	void BlockList::remove(Block *&head, Block *block) throw(std::invalid_argument) {
		if (block == nullptr) {
			throw std::invalid_argument("Nullptr passed for block");
		}

		auto right = block->info.next;
		auto left = block->info.prev;

		if (right != nullptr) {
			right->info.prev = left;
		}

		if (left != nullptr) {
			left->info.next = right;
		}
		else if (head == block) {
			head = right;
		}
		else {
			throw std::invalid_argument("Invalid list passed");
		}
	}
}
