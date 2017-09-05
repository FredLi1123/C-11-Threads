#include <iostream>
#include <future>
#include <thread>


int work(int);
void work9(std::promise<int>);

int main()
{
    // package_task invokes a callable target asychronously
    // initialize a task by a callable
    std::packaged_task<int(int)> task(work);
    
    // get a future
    std::future<int> f1 = task.get_future();
    
    // std::move does reference-copying instead of object-copying (as normal copy constructor does)
    std::thread t1(std::move(task),7);
 
    // future from an async() - automatically launches an asynchornous thread
    std::future<int> f2 = std::async(std::launch::async, work, 8);
 
    // future from a promise
    std::promise<int> p;
    std::future<int> f3 = p.get_future();
    std::thread t2(work9, std::move(p));//.detach();
 
    std::cout << "Waiting..." << std::flush;
    
    f1.wait();
    f2.wait();
    f3.wait();
    std::cout << "Done!" << "\nResults are: "
              << f1.get() << ' ' << f2.get() << ' ' << f3.get() << '\n';
    
    t1.detach();
    t2.detach();
    
    return 0;
}

int work(int x)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return x;
}

void work9(std::promise<int> p)
{
    p.set_value_at_thread_exit(9);
}