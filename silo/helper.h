#ifndef TRANSACTIONSYSTEM_HELPER_H
#define TRANSACTIONSYSTEM_HELPER_H

#include <vector>
#include <thread>
#include <xmmintrin.h>
#include "../common/atomic_wrapper.h"
#include "step.h"
#include "random.h"
#include "zipf.h"
#include "../common/tsc.h"

/**
 * Utility functionをまとめる
 */

#define ERR         \
do {                \
  perror("ERROR");  \
  exit(1);          \
} while(0)

void sleepMs(int ms){
  std::this_thread::sleep_for(
    std::chrono::milliseconds(ms)
  );
}

bool isReady(const std::vector<char> &readyFlags){
  for(auto &flag: readyFlags){
    if(!loadAcquire(flag)) return false;
  }
  return true;
}

void waitForReady(const std::vector<char> &readyFlags){
  while(!isReady(readyFlags)){
    _mm_pause();
  }
}
size_t decideParallelTableBuildNumber(size_t tupleNum){
  // If tuple size is sufficiently small,
  // Table should built by single thread.
  if(tupleNum < 1000) return 1;

  for(size_t i = std::thread::hardware_concurrency(); i > 0; i--){
   if(tupleNum % i == 0){
     return i;
   }
   if(i == 1) ERR;
  }

  return 0;
}

// assumption: ycsb=true, rmw=false, partition=false
inline static void makeSteps(
  std::vector<Step> &steps,
  Xoroshiro128Plus &rand,
  FastZipf &zipf,
  size_t tupleNum,
  size_t maxOp,
  size_t ratio,
  Result &res
  ){
  steps.clear();
  bool readOnlyFlag{true}, writeOnlyFlag{true};
  for(size_t i = 0; i < maxOp; i++){
    uint64_t tmpKey;

    // アクセスするキーをランダムに決める
    //tmpKey = zipf() % tupleNum;
    tmpKey = zipf() & (tupleNum - 1); // x % 2 == x & 1, and & is much faster than %.
    // R/wをランダムに決定
    if((rand.next() % 100) < ratio){
      writeOnlyFlag = false;
      steps.emplace_back(Operation::Read, tmpKey);
    }else{
      readOnlyFlag = false;
      steps.emplace_back(Operation::Write, tmpKey);
    }
  }

  steps.begin()->readOnly = readOnlyFlag;
  steps.begin()->writeOnly = writeOnlyFlag;
}

// Clockのspanが閾値を超えたか
inline bool checkClockSpan(
  const uint64_t start,
  const uint64_t stop,
  const uint64_t threshold
  ){
  uint64_t diff = stop - start;
  return diff > threshold;
}

// 是諾手のWorkerスレッドが最新のEpochを読み込んだか確認する
bool checkEpochLoaded(){
  uint64_t nowEpoch = atomicLoadGlobalEpoch();
  for(size_t i = 1; i < THREAD_NUM; i++){
    if(__atomic_load_n(&(ThreadLocalEpochs[i].body), __ATOMIC_ACQUIRE) != nowEpoch){
      return false;
    }
  }

  return true;
}

void stepEpochTime(uint64_t &epochTimerStart, uint64_t &epochTimerStop){
  epochTimerStop = rdtscp();

  if(checkClockSpan(
    epochTimerStart,
    epochTimerStop,
    EPOCH_TIME * CLOCKS_PER_US * 1000)
    && checkEpochLoaded()){
    atomicAddGlobalEpoch();
    epochTimerStart = epochTimerStop;
  }
}

#endif //TRANSACTIONSYSTEM_HELPER_H
