#ifndef _testhelpers_h_
#define _testhelpers_h_

#include <iostream>

namespace os2bn140314d {
	namespace test {
		inline void writePointer(void *pointer) {
			std::cout << "0x" << std::hex << reinterpret_cast<long>(pointer) << std::endl;
		}
	}
}

#endif
