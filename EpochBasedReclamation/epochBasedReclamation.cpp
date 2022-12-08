#include <iostream>
#include <atomic>
#include <memory>
#include <thread>
#include <chrono>
#include <vector>
#include <list>
#include <string>
// #include "EBR.h"
// #include "LockFreeQueue.h"
#include "TestThread.h"
#include "Node.h"
using namespace std;

int main(int argc, char** argv){
    switch(argc){
        case 5:
            std::cout<<"Running the program...\n";
            break;
        default:
            std::cout<<"Incorrect parameters provided."<<std::endl;
            return -1;
    }            
    const int threadCount = stoi(argv[1]);
    const int iterations = stoi(argv[2]);
    const int epochFreq = stoi(argv[3]);
    const int emptyFreq = stoi(argv[4]);
    const int itersPerThread = int(iterations/threadCount);
    // std::cout<<itersPerThread<<"\n";
    long totalTime = 0;  
    EBR* memManager = new EBR(threadCount, epochFreq, emptyFreq);
    LockFreeQueue* queue = new LockFreeQueue(memManager);
    
    thread threadPool[threadCount];

    std::vector<TestThread*> threads;

    for(int ID = 0; ID < threadCount; ID++){
        // std::cout<<"Making thread"<<ID<<"...\n";
        TestThread* TestThreadObj = new TestThread(queue, itersPerThread, ID);
        threads.push_back(TestThreadObj);
        threadPool[ID] = std::thread(&TestThread::runThreads, threads[ID]);
        // std::cout<<"Made the thread"<<ID<<"...\n";
    }
// std::cout<<"Completed threads"<<"...\n";
    int numEnq = 0;
    int numSuccessfulDeq = 0;    
    
    
    for(int joinID = 0; joinID < threadCount; joinID++){
        threadPool[joinID].join();
        numEnq = numEnq + threads[joinID]->enqueueCounts;
        numSuccessfulDeq = numSuccessfulDeq + threads[joinID]->dequeueCounts;
        std::cout<<"Joined thread "<<joinID<<" ...\n";
    }

    std::cout<<"Total Enqs performed so far = "<<numEnq<<"\n";
    std::cout<<"Total Successful Deqs performed so far = "<<numSuccessfulDeq<<"\n";
    int totalReclaims = memManager->reclaimNumber.load(std::memory_order_acquire);
    std::cout<<"Total Reclaims performed so far = "<<totalReclaims<<"\n";
    
    return 0;
}

