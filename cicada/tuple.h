#ifndef TRANSACTIONSYSTEM_TUPLE_H
#define TRANSACTIONSYSTEM_TUPLE_H

#include <atomic>
#include "common.h"

struct Tuple{
  std::atomic<Version *> latest;
  std::atomic<uint64_t> minWts;
  std::atomic<uint64_t> countingCommit;
  std::atomic<uint8_t> gcLock;

  Tuple()
  : latest(nullptr), gcLock(0){}




};

#endif //TRANSACTIONSYSTEM_TUPLE_H
