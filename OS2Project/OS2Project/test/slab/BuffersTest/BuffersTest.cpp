#include "Slab.h"
#include <iostream>

int main() {
	auto memory = malloc(4096 * 130);

	kmem_init(memory, 130);

	auto pointer = kmalloc(6);

	kfree(pointer);

	std::cout << "OK" << std::endl;
}
