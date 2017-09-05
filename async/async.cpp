//
//  main.cpp
//  async
//
//  Created by Readn on 4/30/16.
//  Copyright © 2016 UCLA Applied Mathematics. All rights reserved.
//

#include <iostream>
#include <future>
#include <atomic>

std::atomic<int> x(0);

int fib(int n)
{
    if (n < 3) return 1;
    else return fib(n-1) + fib(n-2);
}

int main()
{
    // std::launch::async starts an async thread
    std::future<void> f1 = std::async(std::launch::async, [](){
        
        // thread 1, task 1
        x.fetch_add(fib(40));
        
        // thread 1, task 2
        std::cout << "waiting, from thread 1" << '\n';
        
    });
    std::future<void> f2 = std::async(std::launch::async, [](){
        
        // thread 2, task 1
        x.store(fib(2));
        
        // thread 2, task 2
        std::cout << "waiting, from thread 2" << '\n';
        
        // thread 2, task 3
        x.fetch_add(fib(20));
        
        // thread 2, task 4
        std::cout << x.load() << std::endl;
    });
    
    // wait() waits until the thread reaches the end
    f1.wait();
    f2.wait();
    
    std::cout << x.load() << '\n';
}