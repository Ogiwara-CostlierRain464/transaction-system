#include <cstdio>
#include <pthread.h>
#include <vector>
#include <thread>
#include "consts.h"

void worker(int threadID, const bool &start, const bool &quit){

}

int main(){
  alignas(CACHE_LINE_SIZE) bool start = false;
  alignas(CACHE_LINE_SIZE) bool quit = false;

  std::vector<std::thread> threads;
  threads.reserve(THREAD_NUM);
  for(int i = 0; i < THREAD_NUM; i++){
    threads.emplace_back(worker, i, start, quit);
  }

  for(auto &thread: threads){
    thread.join();
  }

  return 0;
}