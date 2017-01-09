#include <iostream>
#include "Slab.h"
#include <vector>
#include <mutex>
#include <iomanip>

const size_t NUM_OF_BLOCKS = 1000;
const int NUM_OF_ITERATIONS = 100;
const size_t NUM_OF_THREADS = 100;

const long SLEEP_TIME_MILISECONDS = 200;

kmem_cache_t *cache;

std::vector<int *> vectors[NUM_OF_THREADS];

std::mutex write_mutex;

std::mutex error_mutex;

bool error = false;

int random() {
	return rand() % SLEEP_TIME_MILISECONDS;
}

void threadAllocBody(int index) {
	for (auto i = 0; i < NUM_OF_ITERATIONS; i++) {
		auto pointer = reinterpret_cast<int *>(kmem_cache_alloc(cache));
		*pointer = index;
		vectors[index].push_back(pointer);

		write_mutex.lock();
		std::cout << "Thread " << std::setw(2) << index << " allocation " << std::setw(2) << i << std::endl;
		write_mutex.unlock();

		std::this_thread::sleep_for(std::chrono::milliseconds(random()));
	}
}

void threadDeallocBody(int index) {
	for (auto i = 0; i < NUM_OF_ITERATIONS; i++) {
		auto pointer = vectors[index][i];

		if (*pointer != index) {
			error_mutex.lock();
			error = true;
			error_mutex.unlock();
		}

		kmem_cache_free(cache, pointer);

		write_mutex.lock();
		std::cout << "Thread " << std::setw(2) << index << " deallocation " << std::setw(2) << i << std::endl;
		write_mutex.unlock();

		std::this_thread::sleep_for(std::chrono::milliseconds(random()));
	}
}

int main() {
	auto memory = malloc(BLOCK_SIZE * NUM_OF_BLOCKS);
	srand(static_cast<unsigned>(time(nullptr)));

	kmem_init(memory, NUM_OF_BLOCKS);

	cache = kmem_cache_create("Cache", sizeof(int), nullptr, nullptr);

	std::vector<std::thread> threads;

	for (auto i = 0; i < NUM_OF_THREADS; i++) {
		threads.push_back(std::thread(threadAllocBody, i));
	}

	for (auto i = 0; i < NUM_OF_THREADS; i++) {
		threads[i].join();
	}

	threads.clear();

	std::cout << std::endl;

	for (auto i = 0; i < NUM_OF_THREADS; i++) {
		threads.push_back(std::thread(threadDeallocBody, i));
	}

	for (auto i = 0; i < NUM_OF_THREADS; i++) {
		threads[i].join();
	}

	std::cout << std::endl;

	if (error) {
		std::cout << "There was an error" << std::endl;
	}
	else {
		std::cout << "Everything OK" << std::endl;
	}

	kmem_cache_destroy(cache);

	free(memory);
}
