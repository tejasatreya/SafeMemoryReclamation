#include <iostream>
#include <atomic>
#include <memory>
#include <thread>
#include <chrono>
#include <vector>
#include <list>
#include <string>
#include "LockFreeQueue.h"

using namespace std;
using namespace std::chrono;
class TestThread {
public:
	int threadId;
	int iterations;
	LockFreeQueue* queue;
	std::atomic<long> enqueueCounts;
	std::atomic<long> dequeueCounts;
	std::chrono::nanoseconds threadTime;


	TestThread(LockFreeQueue* queue, int iterations, int threadID){
		threadTime = std::chrono::nanoseconds(0);
		enqueueCounts.store(0, std::memory_order_release);
		dequeueCounts.store(0, std::memory_order_release);
		this->threadId = threadID;
		this->queue = queue;
		this->iterations = iterations;
	}

	void runThreads() {
		int randomNumber;
		auto threadStartTime = std::chrono::high_resolution_clock::now();
		
		for (int iter = 0; iter < this->iterations; iter++) {
			if(iter%2 == 0){
				this->queue->enqeue(iter, this->threadId);
				enqueueCounts.fetch_add(1, std::memory_order_acq_rel);
			} 
			else {
				try{
					this->queue->dequeue(this->threadId);
					dequeueCounts.fetch_add(1, std::memory_order_acq_rel);
				}
				catch(string exceptionString){
					
				}
			}
		}
		auto threadEndTime = std::chrono::high_resolution_clock::now();
		this->threadTime = (std::chrono::duration_cast<std::chrono::nanoseconds>(threadEndTime - threadStartTime));
	}
};