#ifndef SCHEDULER_H
#define SCHEDULER_H

#include<atomic>
#include<functional>
#include<vector>
#include<iostream>

class scheduler{
public:
  scheduler(std::vector<std::function<void()>>& v )
  {
    std::swap(tasks,v);
    current_task.store(0);
    // by swapping tasks and v, we get the size of v (the number of tasks) in an easier way
    std::cout << "number of tasks is " << tasks.size() <<std::endl;
  };
  bool request_task(std::function<void()> &);
    
private:
  // a vector of void functions as tasks
  std::vector<std::function<void()>> tasks;
  // total number of tasks must be added one-by-one 
  std::atomic<int> current_task;
};

bool scheduler::request_task(std::function<void()>& received_task){
  int idx = current_task.fetch_add(1);
  if(idx < tasks.size())
  {
    received_task = tasks[idx];
    return true;
  }
  return false;
}


#endif
