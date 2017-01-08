/**
* \file CacheHeaderList.cpp
* \brief File implementing the functions for the manipulation of the lists of cache headers
*/

#include "CacheHeaderList.h"
#include "SlabStructs.h"

namespace os2bn140314d {
	void CacheHeaderList::insert(cache_header_s * element) noexcept {
		element->next_ = first_;
		element->prev_ = nullptr;

		if (first_ != nullptr) {
			first_->prev_ = element;
		}

		first_ = element;
	}

	void CacheHeaderList::remove(cache_header_s * element) noexcept {
		auto left = element->prev_;
		auto right = element->next_;

		if (left != nullptr) {
			left->next_ = right;
		}
		else {
			first_ = right;
		}

		if (right != nullptr) {
			right->next_ = left;
		}
	}

	cache_header_s *CacheHeaderList::remove() throw(std::underflow_error) {
		if (first_ == nullptr) {
			throw std::underflow_error("List is empty");
		}

		auto ret = first_;
		first_ = first_->next_;

		if (first_ != nullptr) {
			first_->prev_ = nullptr;
		}

		return ret;
	}

	bool CacheHeaderList::isEmpty() const noexcept {
		return first_ == nullptr;
	}
}