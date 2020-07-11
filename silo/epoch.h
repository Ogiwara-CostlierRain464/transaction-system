#ifndef TRANSACTIONSYSTEM_EPOCH_H
#define TRANSACTIONSYSTEM_EPOCH_H

#include <cstdint>
#include "consts.h"
#include "../common/tsc.h"
#include "../common/helper.h"

static inline uint64_t atomicLoadGlobalEpoch(){
  uint64_t_64byte result =
    __atomic_load_n(&(GlobalEpoch.body), __ATOMIC_ACQUIRE);
  return result.body;
}

static inline void atomicStoreThreadLocalEpoch(
  size_t threadID, uint64_t newVal){
  __atomic_store_n(
    &(ThreadLocalEpochs[threadID].body),
    newVal,
    __ATOMIC_RELEASE);
}

static inline void atomicAddGlobalEpoch(){
  uint64_t expected, desired;
  expected = atomicLoadGlobalEpoch();
  for(;;){
    desired = expected + 1;
    if(__atomic_compare_exchange_n(
      &GlobalEpoch.body, &expected, desired,
      false, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)){
      break;
    }
  }
}

// 是諾手のWorkerスレッドが最新のEpochを読み込んだか確認する
static bool checkEpochLoaded(){
  uint64_t nowEpoch = atomicLoadGlobalEpoch();
  for(size_t i = 1; i < THREAD_NUM; i++){
    if(__atomic_load_n(&(ThreadLocalEpochs[i].body), __ATOMIC_ACQUIRE) != nowEpoch){
      return false;
    }
  }

  return true;
}

static void stepEpochTime(uint64_t &epochTimerStart, uint64_t &epochTimerStop){
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

#endif //TRANSACTIONSYSTEM_EPOCH_H
