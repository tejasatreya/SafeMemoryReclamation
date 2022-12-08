#pragma once
#include<atomic>
#include<iostream>
class Node {
    public:
        int data;
        std::atomic<Node*> nextNode;
        int birth;                                           // Birth epoch variable
        int retire;                                          // Retire epoch variable
        Node(int data){
            this->data = data;
            nextNode.store(NULL, std::memory_order_relaxed); //Since we need atomicity, we use relaxed.
        }
};