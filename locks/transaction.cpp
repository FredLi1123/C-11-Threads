//
//  main.cpp
//  transaction
//
//  Created by CLICC User on 4/24/16.
//  Copyright © 2016 CLICC User. All rights reserved.
//

#include <iostream>
#include <mutex>
#include <thread>

std::mutex g_mutex_a, g_mutex_b;
int acct_a = 1500000;
int acct_b = 2500000;

void safe_transaction(int& acct_a, int& acct_b, int amount)
{
    // one thread places two locks on both acct_a and acct_b, then guards the lock and operates on both without deadlock
    std::lock(g_mutex_a, g_mutex_b);
    
    // if std::adopt_lock is added, then lock::guard only works when the mutex is locked (as this case).
    std::lock_guard<std::mutex> guard_a(g_mutex_a,std::adopt_lock);
    std::lock_guard<std::mutex> guard_b(g_mutex_b,std::adopt_lock);
    for(int i=0;i<amount;i++)
    {
        acct_a--;
        acct_b++;
    }
}

int main()
{
    std::thread t1(safe_transaction, std::ref(acct_a), std::ref(acct_b), 300000);
    std::thread t2(safe_transaction, std::ref(acct_b), std::ref(acct_a), 250000);
    
    t1.join();
    t2.join();
    
    std::cout << "Account a balance is:" << acct_a << std::endl;
    std::cout << "Account b balance is:" << acct_b << std::endl;
    
}