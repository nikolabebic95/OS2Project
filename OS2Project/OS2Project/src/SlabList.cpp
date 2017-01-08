/**
* \file SlabList.cpp
* \brief File implementing the functions for the manipulation of the lists of slabs
*/

#include "SlabList.h"
#include "SlabStructs.h"

namespace os2bn140314d {
	void SlabList::insert(slab_s * element) noexcept {
		if (first_ == nullptr) {
			first_ = element;
		}
		else {
			last_->next_ = element;
		}

		element->next_ = nullptr;
		element->prev_ = last_;
		last_ = element;
	}

	void SlabList::remove(slab_s * element) noexcept {
		auto left = element->prev_;
		auto right = element->next_;

		if (left != nullptr) {
			left->next_ = right;
		}
		else {
			first_ = right;
		}
		
		if (right != nullptr) {
			right->prev_ = left;
		}
		else {
			last_ = left;
		}
	}

	slab_s *SlabList::first() const throw(std::underflow_error) {
		if (first_ == nullptr) {
			throw std::underflow_error("List is empty");
		}

		return first_;
	}

	bool SlabList::isEmpty() const noexcept {
		return first_ == nullptr;
	}

	slab_s *SlabList::get(void * object) const noexcept {
		auto curr = first_;
		while (curr != nullptr) {
			if (curr->contains(object)) {
				return curr;
			}

			curr = curr->next_;
		}

		return nullptr;
	}
}

