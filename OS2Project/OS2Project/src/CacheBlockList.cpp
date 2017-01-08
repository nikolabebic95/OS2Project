/**
* \file CacheHeaderList.h
* \brief File implementing the functions for the manipulation of the lists of cache header blocks
*/

#include "CacheBlockList.h"
#include "SlabStructs.h"

namespace os2bn140314d {
	void CacheBlockList::insert(cache_block_header_s *element) noexcept {
		element->next_ = first_;
		element->prev_ = nullptr;

		if (first_ != nullptr) {
			first_->prev_ = element;
		}

		first_ = element;
	}

	void CacheBlockList::remove(cache_block_header_s *element) noexcept {
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
	}

	bool CacheBlockList::isEmpty() const noexcept {
		return first_ == nullptr;
	}

	cache_block_header_s *CacheBlockList::first() const throw(std::underflow_error) {
		if (first_ == nullptr) {
			throw std::underflow_error("List is empty");
		}

		return first_;
	}
}