// clear && g++ -std=c++20 -fmodules-ts -Wall conccopy.hpp
#include <mutex>
#include <thread>
#include <functional>
#include <queue>
#include <memory>

template<typename T>
class conccopy
{
    public:
        
        conccopy(std::function<void(T)> &&);
        
        void max_copies(const int);
        
        void append_work(T);
        
    private:
    
        std::function<void(T)> assigned_function;
        
        std::shared_ptr<std::mutex[]> top_mutexes;
        std::shared_ptr<std::thread[]> threads;
        
        void shell(int id);
        
        std::queue<T> arg_cue;
        std::mutex arg_cue_mutex;
        
        std::queue<int> waiting_cue;
        std::mutex waiting_cue_mutex;
};

template<typename T>
conccopy<T>::conccopy(std::function<void(T)> &&func)
{
    assigned_function = func;
}

template<typename T>
void conccopy<T>::max_copies(const int num)
{   
    top_mutexes.reset(new std::mutex[num]);
    
    for(int i = 0; i < num; i++)
        top_mutexes[i].lock();
    
    threads.reset(new std::thread[num]);
    
    for(int i = 0; i < num; i++)
    {
        waiting_cue.push(i);
        threads[i] = std::thread(shell, this, i);
    }
}

template<typename T>
void conccopy<T>::shell(int id)
{
    pos1:
    top_mutexes[id].lock();
    top_mutexes[id].unlock();
    
    pos2:
    arg_cue_mutex.lock();
    if(arg_cue.empty() == true)
    { // if no work
        waiting_cue_mutex.lock();
        waiting_cue.push(id);
        arg_cue_mutex.unlock();
        waiting_cue_mutex.unlock();
        goto pos1;
    }
    
    T arg = arg_cue.front();
    arg_cue.pop();
    arg_cue_mutex.unlock();
    
    assigned_function(arg);
   
    goto pos2;
}

template<typename T>
void conccopy<T>::append_work(T new_arg)
{
    arg_cue_mutex.lock();
    arg_cue.push(new_arg);
    waiting_cue_mutex.lock();
    if(waiting_cue.empty() == true)
    {
        waiting_cue_mutex.unlock();
        arg_cue_mutex.unlock();
        return;
    }

    int id = waiting_cue.front();
    waiting_cue.pop();
    
    top_mutexes[id].unlock();
    top_mutexes[id].lock();
    
    arg_cue_mutex.unlock();
    waiting_cue_mutex.unlock();
}
