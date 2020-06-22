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
#include "zipf.h"
#include "result.h"

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
  if(posix_memalign((void**) &Table,
    PAGE_SIZE,
    TUPLE_NUM * sizeof(Tuple)) != 0){
    ERR;
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
  Result &result = SiloResult[threadID];
  Xoroshiro128Plus random(32);
  TXNExecutor txnExecutor(threadID, &result);
  FastZipf zipf(&random, ZIPF_SKEW, TUPLE_NUM);
  uint64_t epochTimerStart, epochTimerStop;

  // Workerの準備が完了したことを伝える
  storeRelease(ready, 1);
  // startになるまでspin lock
  while(!loadAcquire(start)) _mm_pause();
  // quitになるまでWorkerを実行
  while(!loadAcquire(quit)){

  makeSteps(txnExecutor.steps, random, zipf, TUPLE_NUM, MAX_OPERATIONS, RATIO, result);

  RETRY:
    if(threadID == 0){
      // Silo requires that E and e_w never diverge too far: ∀w. E - e_w ≤ 1.
      // step epoch time
      stepEpochTime(epochTimerStart, epochTimerStop);
    }

    if(loadAcquire(quit)) break;

    txnExecutor.begin();
    for(auto &op: txnExecutor.steps){
      switch(op.operation){
        case Operation::Read:
          txnExecutor.read(op.key);
          break;
        case Operation::Write:
          txnExecutor.write(op.key);
          break;
        default:
          ERR;
      }
    }

    if(txnExecutor.validationPhase()){
      txnExecutor.writePhase();

      storeRelease(
        result.localCommitCounts,
        loadAcquire(result.localCommitCounts) + 1);
    }else{
      txnExecutor.abort();
      result.localAbortCounts++;
      goto RETRY;
    }
  }
}

int main() {
  init();

  alignas(CACHE_LINE_SIZE) bool start = false;
  alignas(CACHE_LINE_SIZE) bool quit = false;

  SiloResult.resize(THREAD_NUM);

  std::vector<char> readFlags(THREAD_NUM);
  std::vector<std::thread> threads;
  threads.reserve(THREAD_NUM);

  for(size_t i = 0; i < THREAD_NUM; i++){
    threads.emplace_back(worker,
      i, ref(readFlags[i]), ref(start), ref(quit));
  }

  waitForReady(readFlags);
  storeRelease(start, true);


  sleepMs(1000 * WAIT_SECONDS);


  storeRelease(quit, true);

  for(auto &thread: threads){
    thread.join();
  }

  for(size_t i = 0; i < THREAD_NUM; i++){
    SiloResult[0].addLocalAllResult(SiloResult[i]);
  }

  SiloResult[0].displayAllResult(WAIT_SECONDS);
  return 0;
}