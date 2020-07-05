#ifndef TRANSACTIONSYSTEM_EPOCH_H
#define TRANSACTIONSYSTEM_EPOCH_H

#include <cstdint>
#include "common.h"

inline uint64_t atomicLoadGlobalEpoch(){
  uint64_t_64byte result =
    __atomic_load_n(&(GlobalEpoch.body), __ATOMIC_ACQUIRE);
  return result.body;
}

inline void atomicStoreThreadLocalEpoch(
  size_t threadID, uint64_t newVal){
  __atomic_store_n(
    &(ThreadLocalEpochs[threadID].body),
    newVal,
    __ATOMIC_RELEASE);
}

inline void atomicAddGlobalEpoch(){
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

#endif //TRANSACTIONSYSTEM_EPOCH_H
