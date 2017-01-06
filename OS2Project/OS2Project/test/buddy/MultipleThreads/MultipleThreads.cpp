#include "Buddy.h"
#include "AllocatorUtility.h"
#include <iostream>
#include <string>
#include <vector>

using namespace os2bn140314d;
using namespace std::chrono_literals;

const int NUMBER_OF_ITERATIONS = 26;

std::mutex write_mutex;

std::mutex counter_mutex;

char counter = 'A';

void testFunction() {
	
	auto size = rand() % 10;
	auto sleep_time_before = rand() % 1000;
	auto sleep_time_after = rand() % 1000;

	counter_mutex.lock();
	auto id = counter++;
	counter_mutex.unlock();
	
	try {
		write_mutex.lock();
		std::cout << "Thread " << id << " started" << std::endl;
		write_mutex.unlock();

		std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time_before));

		auto pointer = static_cast<unsigned char *>(Buddy::allocate(size));

		write_mutex.lock();
		std::cout << "Thread " << id << " allocated address " << reinterpret_cast<int>(pointer) << std::endl;
		write_mutex.unlock();

		/*for (auto i = 0; i < size; i++) {
			pointer[i] = id;
		}*/

		std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time_after));

		/*for (auto i = 0; i < size; i++) {
			if (pointer[i] != id) {
				write_mutex.lock();
				std::cout << pointer[i] << id << std::endl;
				write_mutex.unlock();
			}
		}*/

		Buddy::deallocate(pointer, size);

		write_mutex.lock();
		std::cout << "Thread " << id << " finished" << std::endl;
		write_mutex.unlock();
	}
	catch(std::exception &e) {
		write_mutex.lock();
		std::cout << "Thread " << id << " " << e.what() << std::endl;
		write_mutex.unlock();
	}
}

int main() {
	try {

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
	catch(std::exception &e) {
		std::cout << e.what() << std::endl;
	}
}
