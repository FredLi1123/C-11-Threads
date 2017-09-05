#ifndef BARRIER_H
#define BARRIER_H

#include <atomic>
#include <mutex>
   
class spin_barrier{
 public:
  void wait();
  spin_barrier(const spin_barrier&) = delete; //dangerous, so disallow the copy constructor
  spin_barrier(int thread_count);
 private:
  std::atomic<int> counter[2];
  int lock[2];
  int cur_idx;
  int thread_count;

};


#endif
