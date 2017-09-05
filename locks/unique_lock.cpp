//
//  main.cpp
//  unique_lock
//
//  Created by CLICC User on 4/24/16.
//  Copyright © 2016 CLICC User. All rights reserved.
//

#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>

std::mutex g_mutex;

void timed_increment(int& x, int k)
{
    // unique_lock also creates a lock, but we can choose not to lock it initially by adding std::defer_lock
    // more importantly, unique_lock allows us to unlock / lock freely
    std::unique_lock<std::mutex> ul(g_mutex, std::defer_lock);
    for(int i=0;i<k;i++)
    {
        ul.lock();
        x++;
        std::cout<<"Number is now "<<x<<" "<<std::this_thread::get_id()<<std::endl;
        ul.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

int main()
{
    int x = 0;
    std::thread t1(timed_increment, std::ref(x), 5);
    std::thread t2(timed_increment, std::ref(x), 5);
    std::thread t3(timed_increment, std::ref(x), 5);
    
    t1.join();
    t2.join();
    t3.join();
}
