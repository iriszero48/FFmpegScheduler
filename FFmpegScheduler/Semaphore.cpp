#include "Semaphore.h"

Semaphore::Semaphore(const int count) : count(count) {}

void Semaphore::Release()
{
	std::unique_lock<std::mutex> lock(mtx);
	count++;
	cv.notify_one();
}

void Semaphore::WaitOne()
{
	std::unique_lock<std::mutex> lock(mtx);
	while (count == 0) cv.wait(lock);
	count--;
}
