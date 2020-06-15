#include <pthread.h>
#include <vector>
#include <thread>
#include <xmmintrin.h>
#include <iostream>
#include <sys/syscall.h>
#include "common.h"
#include "atomic_wrapper.h"
#include "helper.h"
#include "random.h"
#include "txn_executor.h"

using std::cout;
using std::endl;
using std::ref;
using std::string;

/**
 * Tableの初期化など
 */
void init(){
  if(posix_memalign((void**) &ThreadLocalEpochs,
    CACHE_LINE_SIZE,
    THREAD_NUM * sizeof(uint64_t_64byte)) != 0){
    ERR;
  }
  if(posix_memalign((void**) &CTIDW,
    CACHE_LINE_SIZE,
    THREAD_NUM * sizeof(uint64_t_64byte)) != 0){
    ERR;
  }
  if(posix_memalign((void**) &Table,
    PAGE_SIZE,
    THREAD_NUM * sizeof(Tuple)) != 0){
    ERR;
  }
  for(size_t i = 0; i < THREAD_NUM; i++){
    ThreadLocalEpochs[i].obj = 0;
    CTIDW[i].obj = 0;
  }

  size_t maxThread = decideParallelTableBuildNumber(TUPLE_NUM);
  auto partTableInit = [](size_t threadId, uint64_t start, uint64_t end){
    for(auto i = start; i <= end; i++){
      Tuple *tmp;
      tmp = &Table[i];
      tmp->tidWord.epoch = 1;
      tmp->tidWord.latest = true;
      tmp->tidWord.lock = false;
      tmp->value[0] = 'a';
      tmp->value[1] = '\0';
    }
  };

  std::vector<std::thread> threads;
  threads.reserve(maxThread);
  for(size_t i = 0; i < maxThread; i++){
    threads.emplace_back(partTableInit,
      i, i * (TUPLE_NUM / maxThread),(i+1)*(TUPLE_NUM/maxThread)-1);
  }
  for(auto &th: threads){
    th.join();
  }
}

void worker(
  size_t threadID,
  char &ready,
  const bool &start,
  const bool &quit){
  Xoroshiro128Plus random{};
  random.init();
  TXNExecutor txnExecutor(threadID);

  storeRelease(ready, 1);
  while(!loadAcquire(start)) _mm_pause();

  while(!loadAcquire(quit)){

  }
}

int main(){
  init();

  alignas(CACHE_LINE_SIZE) bool start = false;
  alignas(CACHE_LINE_SIZE) bool quit = false;

  std::vector<char> readFlags(THREAD_NUM);
  std::vector<std::thread> threads;
  threads.reserve(THREAD_NUM);

  for(size_t i = 0; i < THREAD_NUM; i++){
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