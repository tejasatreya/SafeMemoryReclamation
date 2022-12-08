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

class TPointer{    
public:
    int born_before;
    Node* node;

    TPointer(Node* node){
        this->node = node;

    }
    

};