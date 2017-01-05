#include "Buddy.h"
#include "AllocatorUtility.h"
#include <iostream>
#include <string>
#include <vector>

using namespace os2bn140314d;
using namespace std::chrono_literals;

const int NUMBER_OF_ITERATIONS = 100;

std::mutex write_mutex;

std::mutex counter_mutex;

int counter = 0;

void testFunction() {
	auto size = rand() % 10;
	auto sleep_time_before = rand() % 1000;
	auto sleep_time_after = rand() % 1000;

	counter_mutex.lock();
	auto id = counter++;
	counter_mutex.unlock();
	
	write_mutex.lock();
	std::cout << "Thread " << id << " started" << std::endl;
	write_mutex.unlock();

	std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time_before));
	
	auto pointer = Buddy::allocate(size);

	write_mutex.lock();
	std::cout << "Thread " << id << " allocated" << std::endl;
	write_mutex.unlock();

	std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time_after));

	Buddy::deallocate(pointer, size);

	write_mutex.lock();
	std::cout << "Thread " << id << " finished" << std::endl;
	write_mutex.unlock();
}

int main() {
	std::cout << "Hello, World!" << std::endl;

	srand(NULL);

	std::vector<std::thread> threads;

	auto memory = malloc(256 * 1024 * 4096);

	AllocatorUtility::initialize(memory, 256 * 1024);

	for (auto i = 0; i < NUMBER_OF_ITERATIONS; i++) {
		threads.push_back(std::thread(testFunction));
	}

	for (auto i = 0; i < NUMBER_OF_ITERATIONS; i++) {
		threads[i].join();
	}

	std::cout << "Good bye, World!" << std::endl;

	return 0;
}
