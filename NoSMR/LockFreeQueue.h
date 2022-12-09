#include <iostream>
#include <atomic>
#include <memory>
#include <thread>
#include <chrono>
#include <vector>
#include <list>
#include <string>
#include "Node.h"


class LockFreeQueue{
public:
    std::atomic<Node*> head;
    std::atomic<Node*> tail;
    
    LockFreeQueue(){
        Node* node = new Node(0);
        head.store(node);
        tail.store(node);
    }

    void enqeue(int data, int threadID){
        // std::cout<<"Thread "<<threadID<<" entered Enqueue...\n";
        Node* enqNode = new Node(0);
        enqNode->data = data;

        while(true){
            Node* enqTail = tail.load();
            Node* enqNext = (enqTail->nextNode).load();      
            if(enqTail == tail.load()){                        // Validation to check if tail has not been changed 
                if(enqNext == NULL){                           // Checking if new node has been added after selecting tail
                    if((tail.load()->nextNode).compare_exchange_strong(enqNext,enqNode)){
                        tail.compare_exchange_strong(enqTail, enqNode);
                        return;
                    }
                }
                else{
                    tail.compare_exchange_strong(enqTail, enqNext);// Benevolently completing the enqueue operation knowing that the other process is slow.
                }
            }

        }
        // std::cout<<"... Thread "<<threadID<<": Finished Enqueue\n";
    
    }
    
    int dequeue(int threadID){
        // std::cout<<"Thread "<<threadID<<" Entered Dequeue...\n";
        while(true){
            Node* deqHead = head.load();
            Node* deqTail = tail.load();
            Node* deqNext = (deqHead->nextNode).load();
            if(deqHead == deqTail){
                if(deqNext == NULL){
                    // std::cout<<"Right before throwing exc...\n";
                    throw "Empty";
                    // throw(Exception("Exception..."));
                    // continue;
                }
                tail.compare_exchange_strong(deqTail, deqNext);         // Putting CAS on global tail if local tail and global tail are equal
            }
            else{
                int value = deqNext->data;
                if(head.compare_exchange_strong(deqHead, deqNext)){     // If local head is still equal to global head, point the global head to next.
                    return value;
                }
            }
        }
        // std::cout<<"... Thread "<<threadID<<" exiting Dequeue.\n";
    }
};