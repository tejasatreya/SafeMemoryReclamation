#include <iostream>
#include <atomic>
#include <memory>
#include <thread>
#include <chrono>
#include <vector>
#include <list>
#include <string>
#include "IBR.h"
#include "Node.h"


class LockFreeQueue{
public:
    std::atomic<Node*> head;
    std::atomic<Node*> tail;
    IBR* memManager;
    
    LockFreeQueue(IBR* object){
        Node* node = new Node(0);
        memManager = object;
        head.store(node);
        tail.store(node);
    }

    void enqeue(int data, int threadID){
        memManager->start_op(threadID);
        Node* enqNode = memManager->alloc(threadID);
        enqNode->data = data;

        while(true){
            Node* enqTail = tail.load();
            Node* enqNext = (enqTail->nextNode).load();      
            if(enqTail == tail.load()){                        // Validation to check if tail has not been changed 
                if(enqNext == NULL){                           // Checking if new node has been added after selecting tail
                    if((tail.load()->nextNode).compare_exchange_strong(enqNext,enqNode)){
                        tail.compare_exchange_strong(enqTail, enqNode);
                        memManager->end_op(threadID);
                        return;
                    }
                }
                else{
                    tail.compare_exchange_strong(enqTail, enqNext);// Benevolently completing the enqueue operation knowing that the other process is slow.
                }
            }
        }
    }
    
    int dequeue(int threadID){
        memManager->start_op(threadID);
        while(true){
            Node* deqHead = head.load();
            Node* deqTail = tail.load();
            Node* deqNext = (deqHead->nextNode).load();
            if(deqHead == deqTail){
                if(deqNext == NULL){
                    // outdataExec<<"Right before throwing exc...\n";
                    throw "Empty";
                    // throw(Exception("Exception..."));
                    // continue;
                }
                tail.compare_exchange_strong(deqTail, deqNext);         // Putting CAS on global tail if local tail and global tail are equal
            }
            else{
                int value = deqNext->data;
                if(head.compare_exchange_strong(deqHead, deqNext)){     // If local head is still equal to global head, point the global head to next.
                    memManager->retire(deqHead, deqHead->birth, threadID);
                    memManager->end_op(threadID);
                    return value;
                }
            }
        }
    }
};