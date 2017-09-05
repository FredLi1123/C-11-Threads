#include <iostream>
#include <thread>


void hello_world()
{
  std::cout << "Hello World from thread " << std::this_thread::get_id() << std::endl;
}

int main()
{
  auto id = std::this_thread::get_id();
  std::cout << "Primary Thread id: " << id << std::endl;
  
  // spawn a thread to execute print "Hello World"
  std::thread hello(hello_world);
  hello.join();
 
  return 0; 
}

