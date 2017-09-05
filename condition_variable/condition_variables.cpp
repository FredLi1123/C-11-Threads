//
//  main.cpp
//  Conditioning Variables
//
//  Created by CLICC User on 4/23/16.
//  Copyright © 2016 CLICC User. All rights reserved.
//

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex m;
std::condition_variable cv;
std::string data;
bool ready = false;
bool processed = false;

void worker_thread()
{
    // Wait until main() sends data
    std::unique_lock<std::mutex> lk(m);
    
    // only when the statement becomes true does the wait end
    cv.wait(lk, []{return ready;});
    
    // after the wait, we own the lock.
    std::cout << "Worker thread is processing data\n";
    data += " after processing";
    
    // Send data back to main()
    processed = true;
    std::cout << "Worker thread signals data processing completed\n";
    
    // Manual unlocking is done before notifying, to avoid waking up the waiting thread only to block again
    // the lock does not need to be held for notification
    lk.unlock();
    cv.notify_one();
}

int main()
{
    std::thread worker(worker_thread);
    
    data = "Example data";
    // send data to the worker thread
    // use a curly bracket to define the scope of lock_guard
    {
        std::lock_guard<std::mutex> lk(m);
        ready = true;
        std::cout << "main() signals data ready for processing\n";
    }
    
    // unblock one of the waiting threads
    cv.notify_one();
    
    // wait for the worker
    // note that any lock is only valid in its scope
    {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, []{return processed;});
    }
    std::cout << "Back in main(), data = " << data << '\n';
    
    worker.join();
}
