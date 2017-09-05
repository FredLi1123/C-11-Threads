#include <atomic>
#include <chrono>
#include <thread>
#include <iostream>
#include <vector>

std::chrono::time_point<std::chrono::system_clock> start, end;

void inner_product(int* vec_1, int* vec_2, int* sum, int length)
{
  // serial sum
  for (int i=0; i<length; ++i)
    (*sum) += vec_1[i] * vec_2[i];
}

void inner_product_atomic1(int* vec_1, int * vec_2, std::atomic<int>* sum, int length) {

  for (int i=0; i<length; ++i)
    // we must apply functions on atomic variables
    std::atomic_fetch_add(sum, vec_1[i] * vec_2[i] );
}

// atomic 2 is more efficient than atomic 1 - we generally want to reduce the number of operations on atomic variables
void inner_product_atomic2(int* vec_1, int* vec_2, std::atomic<int>* sum, int length)
{
  int local_sum=0;
  for (int i=0; i<length; ++i)
    local_sum += (vec_1[i] * vec_2[i]);

  (*sum).fetch_add(local_sum);
}

// demonstrate differing behavior between atomic and non atomic update in a dot product
void test_inner_product()
{
  int length = 10003;
  int nthreads = 4;
  int vec1[length];
  int vec2[length];

  for (int i=0; i<length; ++i)
  {
    vec1[i] = 1;
    vec2[i] = 1;
  }
  
  int sum=0;
  std::atomic<int> asum; 
  asum.store(0);

  // do 100 rounds of this test with 4 threads
  std::cout << "Testing nonatomic" << std::endl;
  
  // set time counter up
  start = std::chrono::system_clock::now();
    
  for (int i=0; i<100; ++i)
  {
    std::vector< std::thread > threads;
    int offset = 0;
    int local_length = length / nthreads;
    
    // test variant no atomic
    for (int j = 0; j < nthreads; ++j)
    {
        if(j == nthreads - 1)
            threads.push_back(std::thread(inner_product, &vec1[offset], &vec2[offset], &sum, length - local_length * (nthreads - 1)));
        else
        {
            threads.push_back(std::thread(inner_product, &vec1[offset], &vec2[offset], &sum, local_length));
            // we partition the vector by increasing offsets
            offset += local_length;
        }
    }
    
    for(auto& i : threads)
      i.join();

    if(sum != length)
    {
      std::cout <<  "Race Condition Occurred" << std::endl;
      std::cout << sum << std::endl;
    }
    sum = 0;
    
  }
    
  end = std::chrono::system_clock::now();
    
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << "elapsed time: " << elapsed_seconds.count() << std::endl;
    
  //test variant atomic 1
  std::cout << "Testing atomic 1" << std::endl;

  start = std::chrono::system_clock::now();

  for (int i = 0; i < 100; ++i)
  {
    std::vector<std::thread> threads;
    int offset = 0;
    int local_length = length / nthreads;
    
      
    // test variant no atomic
    for (int j = 0; j < nthreads; ++j)
    {
        if(j == nthreads - 1)
            threads.push_back(std::thread(inner_product_atomic1, &vec1[offset], &vec2[offset], &asum, length - local_length * (nthreads - 1)));
        else
        {
            threads.push_back(std::thread( inner_product_atomic1, &vec1[offset], &vec2[offset], &asum, local_length));
            
            // we partition the vector by manipulating offsets
            offset += local_length;
        }
    }
    
    for(auto& i : threads){
      i.join();
    }
    if( asum.load() != length ) {
      std::cout <<  "Race Condition Occurred" << std::endl;
      std::cout << asum.load() << std::endl;
    }
    asum.store(0);
    
  }
    end = std::chrono::system_clock::now();
    elapsed_seconds = end - start;
    std::cout << "elapsed time: " << elapsed_seconds.count() << std::endl;
  
  //test variant atomic2
  std::cout << "Testing atomic 2" << std::endl;

  start = std::chrono::system_clock::now();

  for (int i=0; i<100; ++i)
  {
    std::vector< std::thread > threads;
    int offset = 0;
    int local_length = length / nthreads;
  
    //test variant no atomic
    for (int j = 0; j < nthreads; ++j)
    {
        if(j == nthreads - 1) threads.push_back(std::thread(inner_product_atomic2, &vec1[offset], &vec2[offset], &asum, length - local_length * (nthreads - 1)));
        else
        {
            threads.push_back(std::thread(inner_product_atomic2, &vec1[offset], &vec2[offset], &asum, local_length));
            
            // we partition the vector by manipulating offsets
            offset += local_length;
        }
    }
    
    for(auto& i : threads)
    // i goes through all threads (advanced usage of for loop)
      i.join();
      
    if(asum.load() != length )
    {
      std::cout <<  "Race Condition Occurred" << std::endl;
      std::cout << asum.load() << std::endl;
    }
    asum.store(0);
  }
    
    end = std::chrono::system_clock::now();
    elapsed_seconds = end - start;
    std::cout << "elapsed time: " << elapsed_seconds.count() << std::endl;
}


int main()
{
  test_inner_product();
  return 0;
}
