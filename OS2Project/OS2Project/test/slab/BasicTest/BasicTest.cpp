#include "Slab.h"
#include <iostream>
#include <vector>

auto counter = 0;

const int ITERATIONS = 10;

const int LETTERS = 5;

void ctor(void *object) {
	auto pointer = reinterpret_cast<int *>(object);
	for (auto i = 0; i < LETTERS; i++) {
		pointer[i] = counter;
	}

	counter++;
}

void dtor(void *object) {
	auto pointer = reinterpret_cast<int *>(object);
	for (auto i = 0; i < LETTERS; i++) {
		std::cout << pointer[i] << " ";
	}

	std::cout << std::endl;
}

int main() {
	auto memory = malloc(4096 * 130);
	kmem_init(memory, 130);

	std::vector<kmem_cache_t *> vector;
	std::vector<void *> objects;

	for (auto i = 0; i < ITERATIONS; i++) {
		auto cache = kmem_cache_create("Cache", sizeof(int) * LETTERS, ctor, dtor);
		vector.push_back(cache);
	}

	for (auto i = 0; i < ITERATIONS; i++) {
		auto object = kmem_cache_alloc(vector[i]);
		kmem_cache_free(vector[i], object);
	}

	std::cout << "OK" << std::endl;
}
