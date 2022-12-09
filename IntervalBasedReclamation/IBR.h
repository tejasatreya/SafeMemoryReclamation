#include <climits>
#include <iostream>
#include <cstdint>
#include <list>
#include <vector>
#include "Node.h"

class IBR{
    public:
        int epochFreq;
        int emptyFreq;
        int threadCount;
        std::atomic<long>  reclaimNumber;

    private:
        int* counterIBR;
        int* allocatedNodeCounter;
        int* retireCounter;
        std::atomic<int>* reservationUpper;
        std::atomic<int>* reservationLower;
        std::atomic<long> epoch;
        std::list<Node*> *retired;
        
public:         // Methods
    IBR(int threadCount, int epochFreq, int emptyFreq){
        this->threadCount = threadCount;
        this->epochFreq = epochFreq;
        this->emptyFreq = emptyFreq;
        reservationUpper = new std::atomic<int>[threadCount];
        reservationLower = new std::atomic<int>[threadCount];
        retired = new std::list<Node*>[threadCount];                    //Making the reservation array and retired list

        for(int i = 0; i < threadCount; i++){
            //Tellling all threads that no operations are running. 
            reservationUpper[i].store(INT_MAX, std::memory_order_release);   //(Equivalent to End_op assigning all threads to MAX)
            reservationLower[i].store(INT_MAX, std::memory_order_release);
        }
        
        counterIBR = new int[threadCount]();
        allocatedNodeCounter = new int[threadCount]();
        retireCounter = new int[threadCount]();
        epoch.store(0, std::memory_order_release);                      // Fence for the epoch when the new object for EBR is made.
        reclaimNumber.store(0, std::memory_order_release);
    }

    void start_op(int threadID){
        // Loading epoch value from global scope
        long localEpoch = epoch.load(std::memory_order_acquire);
        reservationUpper[threadID].store(localEpoch, std::memory_order_seq_cst);
        reservationLower[threadID].store(localEpoch, std::memory_order_seq_cst);
        
    }

    void end_op(int threadID){
        // Putting max value for the integer in the local epoch variable to show that operations are not complete.
        reservationUpper[threadID].store(INT_MAX, std::memory_order_seq_cst);
        reservationLower[threadID].store(INT_MAX, std::memory_order_seq_cst);
        
    }
    
    void empty(int threadID){
        int counter = 0;
        std::vector<int> threadEpochUpper;
        std::vector<int> threadEpochLower;
        for(int i = 0; i < threadCount; i++){
            int resUpper = reservationUpper[i].load(std::memory_order_acquire);
            int resLower = reservationLower[i].load(std::memory_order_acquire);
            threadEpochUpper.push_back(resUpper);
            threadEpochLower.push_back(resLower);
        }

        std::list<Node*>* threadLocalList = &(retired[threadID]);
        auto head = threadLocalList->begin();
        auto tail = threadLocalList->end();
        
        while(head!=tail){                                                              // Block belongs to retired list.
            Node* node = *head;
            bool conflict = false;
            for (int i = 0; i < threadCount; i++){
                if( (node->birth <= threadEpochUpper[i]) && (node->retire >= threadEpochLower[i])){
                    conflict = true;
                }    
            }
            if(!conflict){
                head = threadLocalList->erase(head);
                delete node;
                reclaimNumber.fetch_add(1,std::memory_order_acq_rel);
            }
            head++;    
        }

    }

    void retire(Node* node, int birth, int threadID){
        retired[threadID].push_back(node);                              // Pushing the node to the end of retired list of the thread.
        int retire = epoch.load(std::memory_order_acquire);
        node->retire = retire;
        counterIBR[threadID] = counterIBR[threadID]+1;
        // if((counterIBR[threadID] % epochFreq)==0){
        //     epoch.fetch_add(1,std::memory_order_acq_rel);
        // }
        if((retired[threadID].size()  %  emptyFreq) == 0){
            empty(threadID);
        }
    }

    Node* alloc(int threadID){                                          // Returning node pointer for allocation
        allocatedNodeCounter[threadID] = allocatedNodeCounter[threadID] + 1;

        if((allocatedNodeCounter[threadID] % epochFreq) == 0){
            epoch.fetch_add(1, std::memory_order_acq_rel);
        }
        Node* newNode = new Node(0);
        newNode->birth = epoch.load(std::memory_order_acquire);
        return newNode;
    }
};