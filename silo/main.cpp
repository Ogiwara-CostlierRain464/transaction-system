#include <cstdio>
#include <pthread.h>
#include <vector>
#include <thread>
#include "consts.h"

void worker(int threadID){

}

int main(){
  alignas(CACHE_LINE_SIZE) bool start = false;

  std::vector<std::thread> threads;
  for(int i = 0; i < THREAD_NUM; i++){
    //threads.emplace_back(worker, i, )
  }

  return 0;
}