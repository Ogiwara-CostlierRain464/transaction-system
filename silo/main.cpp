#include <cstdio>
#include <pthread.h>
#include <vector>
#include <thread>
#include <xmmintrin.h>
#include <iostream>
#include "consts.h"
#include "atomic_wrapper.h"
#include "helper.h"

using std::cout;
using std::endl;
using std::ref;
using std::string;

void worker(
  int threadID,
  char &ready,
  const bool &start,
  const bool &quit){

  storeRelease(ready, 1);
  while(!loadAcquire(start)) _mm_pause();

  while(!loadAcquire(quit)){

  }
}

int main(){
  alignas(CACHE_LINE_SIZE) bool start = false;
  alignas(CACHE_LINE_SIZE) bool quit = false;

  std::vector<char> readFlags(THREAD_NUM);
  std::vector<std::thread> threads;
  threads.reserve(THREAD_NUM);

  for(int i = 0; i < THREAD_NUM; i++){
    threads.emplace_back(worker,
      i, ref(readFlags[i]), ref(start), ref(quit));
  }

  waitForReady(readFlags);
  storeRelease(start, true);

  sleepMs(1000);

  storeRelease(quit, true);

  for(auto &thread: threads){
    thread.join();
  }

  return 0;
}