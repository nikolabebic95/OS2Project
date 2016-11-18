/**
 * \file DummyHeader.h
 * \brief Dummy file testing the include path dependency
 * TODO: Remove the file
 */

#ifndef _dummyheader_h_
#define _dummyheader_h_

#include <iostream>

namespace dummy {
	inline void f() {
		std::cout << "Hello World!" << std::endl;
	}
}

#endif
