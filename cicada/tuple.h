#ifndef TRANSACTIONSYSTEM_TUPLE_H
#define TRANSACTIONSYSTEM_TUPLE_H

#include <atomic>
#include "consts.h"

struct Tuple{
  std::atomic<Version *> latest;
  std::atomic<uint64_t> minWts;
  std::atomic<uint64_t> continuingCommit;
  std::atomic<uint8_t> gcLock;

  Tuple()
  : latest(nullptr), gcLock(0){}

  Version *loadAcquireLatest(){
    return latest.load(std::memory_order_acquire);
  }

};

#endif //TRANSACTIONSYSTEM_TUPLE_H
