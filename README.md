# C++ Implementation of Safe Memory Reclamation schemes for Lock-Free Concurrent Data Structures

Memory reclamation for a lock based structure is very straightforward and a node can be reclaimed when no lock is held on it. A lock based concurrent data structure ensures that as long as an operation is pending on the objects of a data structure, the lock on them is not released. The results in a simple check to verify if any thread still holds reference (that is, working on) to an object. However, in a lock free concurrent data structure, this verification is not trivial. If we reclaim a node without proper checking, it could lead to read-reclaim races. For example, overlapping reclaim and read calls could result in the read reading the address of an object which is then reclaimed. And when the read call tries to dereference the address, it will get a garbage value. In scenarios such as this one, improper garbage collection may violate legality of a data structure and could also violate progress properties.

In this project we have implemented two types of Memory Reclamation techniques, Epoch-Based Reclamation (EBR) and Interval-Based Reclamation (IBR). In the EBR scheme, each thread will try to reserve all the memory blocks that were created after a certain time. However, in IBR based schemes, only a set of processes will be able to reserve a block based on a time-interval and based on their existence/lifetime as an active reference for at least one thread. The IBR strategy is a stronger strategy as it will allow other threads to be released in case of one of the thread being stuck in a stall. The EBR strategy blocks all the threads in such a scenario and has very problematic consequences in terms of memory exhaustion. IBR provides a great solution to this by giving us a time interval or a window so to say, to check if the retired processes lie beyond the window of being reclaimed. If they do, we reclaim them without having to run a validation or a checking operation on them. 

## Compilation and Execution
We have used the -std=C++11 compiler along with -lpthread to implement the techniques. 
For implementation, move to the EBR, IBR or NoSMR directories for Epoch based, Interval based and a baseline no-SMR based implementation. In the directories, you will have to run the following command for compiling.
```
g++ <File name (Eg. epochBasedReclamation)>.cpp -o main  -std=c++11 -lpthread

./main <thread count> <iterations> <epoch frequency> <empty frequency>
```
The output is displayed as follows and has the following format. An example is presented below:
```
  Threads= 64     MaxT= 42754187  TP = 1496.93    SOPT = 16.1875  Sp = 1036
```
1. "Threads" represents the total thread count.
2. "MaxT" represents the Maximum time taken by a thread. This is used for calculating the throughput.
3. "TP" represents the throughput for a single execution. 
4. "SOPT" represents the space overhead per thread.
5. "Sp" represents the space overhead for the execution, calculated by taking the difference between total number of enqueue operations and total reclaims performed. 

The code also contains multiple commented print statements which can allow us to output additional information for analysis purposes. 
There is also a prewritten shell script for performing a batch of executions in a single go. We ran this on the RLogin server and the outputs are presented in the text files in every techniques particular folder by the following name "<technique>_out.txt"

## References
Haosen Wen, Joseph Izraelevitz, Wentao Cai, H. Alan Beadle, and Michael L. Scott, "Interval-Based Memory Reclamation", PPoPP ’18, February 24–28, 2018, Vienna, Austria.
      
Maurice Herlihy and Nir Shavit. 2012. The Art of Multiprocessor Programming, Revised Reprint (1st. ed.). Morgan Kaufmann Publishers Inc., San Francisco, CA, USA.
