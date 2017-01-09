#include <iostream>
#include "Slab.h"

const size_t NUM_OF_BLOCKS = 100;

int main() {
	auto pointer = malloc(BLOCK_SIZE * NUM_OF_BLOCKS);

	kmem_init(pointer, NUM_OF_BLOCKS);

	auto cache = kmem_cache_create("Cache", BLOCK_SIZE / 2 + 1, nullptr, nullptr);

	for (auto i = 0; i < NUM_OF_BLOCKS; i++) {
		
		kmem_cache_alloc(cache);

		std::cout << i + 1 << ". block" << std::endl;
		kmem_cache_error(cache);
		std::cout << std::endl << std::endl;
	}

	free(pointer);
}
