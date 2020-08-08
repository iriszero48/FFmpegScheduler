#pragma once

#include <mutex>

class Semaphore
{
public:
	Semaphore(int count);

	void Release();

	void WaitOne();

private:
	std::mutex mtx;
	std::condition_variable cv;
	int count;
};
