// clear && g++ -std=c++20 -Wall bench.cpp && ./a.exe
#include <mutex>
#include <thread>
#include <span>
#include <functional>
#include <queue>
#include <memory>

#include <iostream>
#include <unistd.h>

template<typename T>
class conccopy
{
    public:
        
        conccopy(std::function<void(T)> &&);
        
        void max_copies(const int);
        
        void append_work(const T);
        
    private:
    
    std::function<void(T)> assigned_function;
    
    // here to prevent the mutexes from being de-scoped
    std::shared_ptr<std::mutex[]> mutex_tape_start;
    // here as a simple "view" api into the above
    std::span<std::mutex, std::dynamic_extent> thread_mutexes;
    
    // this function with a wierd name is the function that
    // threads will run
    static void conccopy_thread_shell(std::mutex &, int); // this function is a nightmare
    
    // where work will be appended
    std::queue<T> argument_queue;
    // a mutex on the above. accessors must lock this mutex first
    std::mutex argument_queue_mutex;
    
    // where blocked mutexes will append their ids
    // to indicate they are waiting for work
    // a thread id is referring to the index position of their
    // mutex within span::thread_mutexes.
    std::queue<int> sleeper_indexes;
    // a mutex on the above. accessors must lock this mutex first
    std::mutex sleeper_indexes_mutex;
    
};

//constructor
template<typename T>
conccopy<T>::conccopy(std::function<void(T)> &&func)
{
    assigned_function = func;
}

template<typename T>
void conccopy<T>::max_copies(const int num)
{   
    //sets up an array of mutexes for future use
    mutex_tape_start.reset(new std::mutex[num]);
    
    //sets a span of the above mutexes to allow for an easier manipulation api
    std::span<std::mutex, std::dynamic_extent> sequence_of_mutexes(mutex_tape_start.get(), num);
    thread_mutexes = sequence_of_mutexes;
    
    //locks all above mutexes
    for(std::mutex &i : thread_mutexes)
        i.lock();
    
    //make an array of threads
    //append an id to the worker queue
    //make threads running loop function with their id# and mutex
}

template<typename T>
void conccopy<T>::conccopy_thread_shell(std::mutex &wall, int id)
{
    // pos1
    wall.lock();
    wall.unlock();
    // pos2
    // lock the work queue
    // check for work
    /* if no work
        lock the worker queue
        add self id to worker queue
        unlock work queue
        unlock worker queue
        go back to pos1
    */
    // grab work
    // unlock work queue
    // do work
    // go to pos2
}

void some_func(int){std::cout << 'a' << std::endl;} // used for testing
int main()
{
    conccopy<int> coconut(some_func);
    
    coconut.max_copies(11);
}
