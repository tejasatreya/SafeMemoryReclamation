#include <iostream>
#include <atomic>
#include <memory>
#include <thread>
#include <chrono>
#include <vector>
#include <list>
#include <string>
#include "TestThread.h"
#include "Node.h"
using namespace std;

int main(int argc, char** argv){
    // ofstream //outdata;
    // //outdata.open("execution_output.txt");
    
    switch(argc){
        case 3:
            //std::cout<<"Running the program...\n";
            break;
        default:
            //std::cout<<"Incorrect parameters provided."<<std::endl;
            return -1;
    }            
    const int threadCount = stoi(argv[1]);
    //outdata<<"threadCount given = "<< threadCount<<std::endl;
    const int iterations = stoi(argv[2]);
    //outdata<<"Iterations given = "<< iterations<<std::endl;
    const int itersPerThread = int(iterations/threadCount);
    //outdata<<"iterations per thread = "<< itersPerThread<<std::endl;
    // std::cout<<itersPerThread<<"\n";
    long totalTime = 0;  
    LockFreeQueue* queue = new LockFreeQueue();
    
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
    long maxTime = 0;
    
    for(int joinID = 0; joinID < threadCount; joinID++){
        threadPool[joinID].join();
        numEnq = numEnq + threads[joinID]->enqueueCounts;
        numSuccessfulDeq = numSuccessfulDeq + threads[joinID]->dequeueCounts;
        //std::cout<<"Joined thread "<<joinID<<" ...\n";
        long IDTime = threads[joinID]->threadTime.count();
        maxTime = (IDTime>maxTime)? IDTime : maxTime;
    }
    
    double throughput = iterations/(maxTime*0.000001);
    //outdata<<"Total Enqs performed so far = "<<numEnq<<"\n";
    //outdata<<"Total Successful Deqs performed so far = "<<numSuccessfulDeq<<"\n";
    int totalReclaims = 0;
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

