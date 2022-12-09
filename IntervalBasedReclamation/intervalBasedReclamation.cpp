#include <iostream>
#include <atomic>
#include <memory>
#include <thread>
#include <chrono>
#include <vector>
#include <list>
#include <string>
// #include "IBR.h"
// #include "LockFreeQueue.h"
#include "TestThread.h"
#include "Node.h"
// #include <fstream>
// using std::ofstream;
// using namespace std;

int main(int argc, char** argv){
    // ofstream //outdata;
    // //outdata.open("execution_output.txt");
    switch(argc){
        case 5:
            //outdata<<"Running the program...\n";
            break;
        default:
            //outdata<<"Incorrect parameters provided."<<std::endl;
            return -1;
    }            
    const int threadCount = stoi(argv[1]);
    //outdata<<"threadCount given = "<< threadCount<<std::endl;
    
    const int iterations = stoi(argv[2]);
    //outdata<<"Iterations given = "<< iterations<<std::endl;
    const int epochFreq = stoi(argv[3]);
    //outdata<<"epoch Frequency given = "<< epochFreq<<std::endl;
    const int emptyFreq = stoi(argv[4]);
    //outdata<<"empty Frequency given = "<< emptyFreq<<std::endl;
    const int itersPerThread = int(iterations/threadCount);
    //outdata<<"iterations per thread = "<< itersPerThread<<std::endl;
    
    // //outdata<<itersPerThread<<"\n";
    long totalTime = 0;  
    IBR* memManager = new IBR(threadCount, epochFreq, emptyFreq);
    LockFreeQueue* queue = new LockFreeQueue(memManager);
    
    thread threadPool[threadCount];

    std::vector<TestThread*> threads;

    for(int ID = 0; ID < threadCount; ID++){
        // //outdata<<"Making thread"<<ID<<"...\n";
        TestThread* TestThreadObj = new TestThread(queue, itersPerThread, ID);
        threads.push_back(TestThreadObj);
        threadPool[ID] = std::thread(&TestThread::runThreads, threads[ID]);
        // //outdata<<"Made the thread"<<ID<<"...\n";
    }
// //outdata<<"Completed threads"<<"...\n";
    int numEnq = 0;
    int numSuccessfulDeq = 0;    
    long maxTime = 0;
    
    for(int joinID = 0; joinID < threadCount; joinID++){
        threadPool[joinID].join();
        numEnq = numEnq + threads[joinID]->enqueueCounts;
        numSuccessfulDeq = numSuccessfulDeq + threads[joinID]->dequeueCounts;
        long IDTime = threads[joinID]->threadTime.count();
        maxTime = (IDTime>maxTime)? IDTime : maxTime;
        //outdata<<"Joined thread "<<joinID<<" ...\n";
    }
    double throughput = iterations/(maxTime*0.0000001);
    //outdata<<"Total Enqs performed so far = "<<numEnq<<"\n";
    //outdata<<"Total Successful Deqs performed so far = "<<numSuccessfulDeq<<"\n";
    int totalReclaims = memManager->reclaimNumber.load(std::memory_order_acquire);
    double sopt = (numEnq-totalReclaims)/(threadCount*0.1*10);
    //outdata<<"Total Reclaims performed so far = "<<totalReclaims<<"\n";
    //outdata<<"Max Time = "<< maxTime<<std::endl;
    //outdata<<"Throughput = "<< throughput<<std::endl;
    //outdata.close();
    // std::cout<<"Total Enqs performed so far = "<<numEnq<<"\n";
    // std::cout<<"Total Successful Deqs performed so far = "<<numSuccessfulDeq<<"\n";
    // std::cout<<"Total Reclaims performed so far = "<<totalReclaims<<"\n";
    std::cout<<"Threads= "<< threadCount;
    std::cout<<"\tMaxT= "<< maxTime;
    std::cout<<"\tTP = "<< throughput;
    std::cout<<"\tSOPT = "<<sopt;
    std::cout<<"\tSp = "<<(numEnq-totalReclaims)<<std::endl;
    return 0;
}
