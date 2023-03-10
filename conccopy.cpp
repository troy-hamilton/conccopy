// clear && g++ -std=c++20 -Wall conccopy.cpp && ./a.exe
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
        
        void append_work(T);
        
    private:
    
    std::function<void(T)> assigned_function;
    
    // here to prevent the mutexes from being de-scoped
    std::shared_ptr<std::mutex[]> mutex_tape_start;
    // here as a simple "view" api into the above
    std::span<std::mutex, std::dynamic_extent> thread_mutexes;
    
    // this is to prevent the threads from being de-scoped
    std::shared_ptr<std::thread[]> thread_tape_start;
    // this allows for a simple view api into the above
    std::span<std::thread, std::dynamic_extent> thread_house;
    
    // this function with a wierd name is the function that
    // threads will run
    void thread_shell(int id);
    
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
    // sets up an array of mutexes for future use
    mutex_tape_start.reset(new std::mutex[num]);
    
    // sets a span of the above mutexes to allow for an easier manipulation api
    std::span<std::mutex, std::dynamic_extent> sequence_of_mutexes(mutex_tape_start.get(), num);
    thread_mutexes = sequence_of_mutexes;
    
    // locks all above mutexes
    for(std::mutex &i : thread_mutexes)
        i.lock();
    
    // sets an array of threads for future use
    thread_tape_start.reset(new std::thread[num]);
    
    // sets a span on the above threads for an easier api
    std::span<std::thread, std::dynamic_extent> sequence_of_threads(thread_tape_start.get(), num);
    thread_house = sequence_of_threads;
    
    // makes an int thread id (which represents the index positon of their mutex)
    // and adds it to the sleeper queue
    // and activates new threads
    for(int i = 0; i < num; i++)
    {
        sleeper_indexes.push(i);
        thread_house[i] = std::thread(thread_shell, this, i);
    }
}

template<typename T>
void conccopy<T>::thread_shell(int id)
{
    pos1:
    thread_mutexes[id].lock();
    thread_mutexes[id].unlock();
    // pos2
    argument_queue_mutex.lock(); // lock the work queue
    if(argument_queue.empty() == true) // check for work
    { // if no work
        sleeper_indexes_mutex.lock(); // lock the worker queue
        sleeper_indexes.push(id); // add self id to worker queue
        argument_queue_mutex.unlock(); // unlock work queue
        sleeper_indexes_mutex.unlock(); // unlock the worker queue
        goto pos1;// go back to pos1
    }
    
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

template<typename T>
void conccopy<T>::append_work(T)
{
    //acquire lock on work queue mutex
    //add work to work queue
    //acquire lock on worker queue mutex
    //check if there are any workers available
    /*
        if there are no workers available,
        unlock the worker queue mutex
        unlock the work queue mutex
        return
    */
    //grab a workers' id from the worker queue
    //unlock the mutex at thread_mutexes[id]
    //lock the mutex at thread_mutexes[id]
    //unlock the work queue mutex
    //unlock the worker queue mutex
    
}

void example_func(int n)
{
}

int main()
{
    conccopy<int> splitter(example_func);
    
    splitter.max_copies(2);
}
