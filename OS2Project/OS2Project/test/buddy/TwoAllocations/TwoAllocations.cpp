#include "AllocatorUtility.h"
#include "Buddy.h"
#include "TestHelpers.h"

using namespace os2bn140314d;
using namespace os2bn140314d::test;

int main() {
	try {
		auto pointer = malloc(4096 * 130);
		AllocatorUtility::initialize(pointer, 130);

		auto allocated1 = Buddy::allocate(5);

		writePointer(allocated1);

		Buddy::deallocate(allocated1, 5);

		allocated1 = Buddy::allocate(5);

		writePointer(allocated1);

		auto allocated2 = Buddy::allocate(5);

		writePointer(allocated2);
		
		return 0;
	}
	catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
}