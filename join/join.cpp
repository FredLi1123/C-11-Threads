#include <iostream>
#include <thread>
#include <chrono>

void count()
{

  std::this_thread::sleep_for(std::chrono::seconds(2)); //sleep for 2000 milliseconds
  for (int i=0; i<1000; ++i)
    std::cout << i << std::endl;
}


int main()
{
  std::thread counting(count);
    
  //if I don't care if the thread doesn't finish executing
  //counting.detach();
    
  //if I want the thread to finish executing
  counting.join();
  std::cout << "main thread is exiting" << std::endl;

  return 0;
}
