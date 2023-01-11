// clear && g++ -std=c++20 -Wall bench.cpp && ./a.exe
#include <thread>
#include <memory>
#include <iostream>

void func(){std::cout << "func()" << std::endl;}

void sope(std::shared_ptr<std::thread[]> &spot)
{
    std::shared_ptr<std::thread[]> point(new std::thread[5]);
    point[0] = std::thread(func);
    spot = point;
}

int main()
{
    std::shared_ptr<std::thread[]> point;
    
    sope(point);
    
    // point here refers to the point made in sope
    point[0].join(); // this prevents an exception error
}
