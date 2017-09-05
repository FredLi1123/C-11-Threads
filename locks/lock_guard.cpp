//
//  main.cpp
//  lock_guard
//
//  Created by CLICC User on 4/24/16.
//  Copyright © 2016 CLICC User. All rights reserved.
//

#include <iostream>
#include <mutex>
#include <thread>

std::mutex g_mutex;
int g_num = 0;

void safe_increment(int& x, int k)
{
    std::lock_guard<std::mutex> guard(g_mutex);
    for(int i=0;i<k;i++) x++;
    
}

int main(int argc, const char * argv[])
{
    std::thread t1(safe_increment,std::ref(g_num),1000);
    std::thread t2(safe_increment,std::ref(g_num),100);
    
    t1.join();
    t2.join();
    
    std::cout << "the number is " << g_num << std::endl;
    
    return 0;
}
