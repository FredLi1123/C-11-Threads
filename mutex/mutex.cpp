//
//  main.cpp
//  Mutex
//
//  Created by CLICC User on 4/23/16.
//  Copyright © 2016 CLICC User. All rights reserved.
//

#include <iostream>
#include <map>
#include <string>
#include <chrono>
#include <thread>
#include <mutex>

std::map<std::string, std::string> g_pages;
std::mutex g_pages_mutex;

void save_page(const std::string &url)
{
    
    // simulate a long page fetch
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::string result = "fake content";
    
    // When a lock_guard object is created, it attempts to take ownership of the mutex it is given. When control leaves its scope, the lock_guard is destructed and the mutex is released.
    std::lock_guard <std::mutex> guard(g_pages_mutex);
    g_pages[url] = result;
}

int main()
{
    std::thread t1(save_page, "http://foo");
    std::thread t2(save_page, "http://bar");
    t1.join();
    t2.join();
    
    // safe to access g_pages without lock now, as the threads are joined
    // for loop iterates over the map and output the results
    for (const auto &pair : g_pages) {
        std::cout << pair.first << " => " << pair.second << '\n';
    }
}

