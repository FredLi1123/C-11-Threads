#include <iostream>
#include <random>
#include <thread>
#include "scheduler.h"


void dot(double* result, double* arr1, int stride1, double* arr2, int stride2, int size);

//serial matrix mult
void matrix_mult(double* matrix,int m, int n, double* v_in, double* v_out);

//serial matrix transpose mult
void matrix_transpose_mult(double* matrix,int m, int n, double* v_in, double* v_out);

//calculate error
void norm_squared_diff(double* result, double * arr1, int stride1, double * arr2, int stride2, int size);

void worker(scheduler& s){
  std::function<void()> func;
  while(s.request_task(func) == true){
    // operator overloading on the function (already becomes the task)
    func();
   }  
} 

class dot_task{
public:
    dot_task(double* result, double* arr1, int stride1, double* arr2, int stride2, int size):result(result),arr1(arr1),stride1(stride1),arr2(arr2),stride2(stride2),size(size){};
    void operator()(){
        dot(result,arr1,stride1,arr2,stride2,size);
    }

private:
    double* result;
    double* arr1;
    int stride1;
    double* arr2;
    int stride2;
    int size;
};



int main(){

  int m = 16000;
  int n = 12000;
  
  double* matrix= new double[m*n];
  double* x= new double[n];
  double* y= new double[m];
  double* y_true= new double[m];
  double* x_true= new double[m];
  
  std::default_random_engine gen;
  std::normal_distribution<double> distr(0,1);

  for(int i=0; i<m*n; ++i){
    matrix[i] = distr(gen);
  }
  for(int i=0; i<m; ++i){
    y[i] = distr(gen);
  }
  for(int i=0; i<n; ++i){
    x[i] = distr(gen);
  }

  
  std::chrono::time_point<std::chrono::system_clock> start, end;
  std::chrono::duration<double> elapsed_seconds;
  start = std::chrono::system_clock::now();
    
  //calculate time serial operations take (sequentially calculate Ax and A^TAx)
  matrix_mult(matrix,m,n,x,y_true);
  matrix_transpose_mult(matrix,m,n,y_true,x_true);
  end = std::chrono::system_clock::now();
  elapsed_seconds = end-start;
  std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
  
  //multithreading information
  int nthreads = 4;
    
  //initialize schedulers
  std::vector<std::thread> threads;
  std::vector<std::function<void()>> mm_tasks;
  std::vector<std::function<void()>> mtm_tasks;
  
  //populate tasks
 
  for(size_t i=0; i<m; ++i)
  {
    // constructors are also void functions, so the argument passing is valid
    mm_tasks.push_back(dot_task(&y[i], matrix+i*n, 1, &x[0], 1, n));
  }
  for(size_t i=0; i<n; ++i)
  {
    mtm_tasks.push_back(dot_task(&x[i], matrix+i, n, &y[0], 1, m));
  }
   
  // dot_task objects will eventually be passed to func() in the scheduler (see worker)
  scheduler mm_scheduler(mm_tasks);
  scheduler mtm_scheduler(mtm_tasks);
  start = std::chrono::system_clock::now();
  
  for(size_t i=0; i<nthreads; ++i){
    threads.push_back(std::thread(worker, std::ref(mm_scheduler)));
  }
  
  for(size_t i=0; i<nthreads; ++i){
    threads[i].join();
  
  }
  threads.clear();
    
  for(size_t i=0; i<nthreads; ++i){
    threads.push_back(std::thread(worker, std::ref(mtm_scheduler)));
  }
  
  for(size_t i=0; i<nthreads; ++i){
    threads[i].join();
  }
  
  end = std::chrono::system_clock::now();
  elapsed_seconds = end-start;
  std:: cout << "elapsed time: " << elapsed_seconds.count() << "s\n"; 
  
  double diff=0;
  norm_squared_diff(&diff,y,1,y_true,1,m);
  std::cout << "difference in serial versus multithread Ax is " << diff << std::endl;
  norm_squared_diff(&diff,x,1,x_true,1,n);
  std::cout << "difference in serial versus multithread ATAx is " << diff << std::endl;

  delete[] matrix;
  delete[] x;
  delete[] y_true;
  delete[] y;
  delete[] x_true;
}


void dot(double* result, double* arr1, int stride1, double* arr2, int stride2, int size){

  int loc1,loc2;
  loc1 = loc2 = 0;
  double local_result = 0;
  for(int i=0; i<size; ++i){
    local_result += arr1[loc1] * arr2[loc2];
    loc1+=stride1;
    loc2+=stride2;
  }
  *result=local_result;
}

//serial matrix mult
void matrix_mult(double* matrix,int m, int n, double* v_in, double* v_out){
  for(size_t i=0; i<m; ++i){
    dot(&v_out[i], matrix+i*n, 1, v_in, 1, n);
  }   
}

//serial matrix transpose mult
void matrix_transpose_mult(double* matrix,int m, int n, double* v_in, double* v_out){
  for(size_t i=0; i < n; ++i){
    dot(&v_out[i], matrix+i, n, v_in, 1, m);
  }   
}

void norm_squared_diff(double* result, double * arr1, int stride1, double * arr2, int stride2, int size){
  int loc1,loc2;
  loc1 = loc2 = 0;
  double local_result = 0;
  for(int i=0; i<size; ++i){
    double temp = arr1[loc1]-arr2[loc2];
    local_result += temp*temp;
    loc1+=stride1;
    loc2+=stride2;
  }
  *result=local_result;
}
