#ifndef TRANSACTIONSYSTEM_HELPER_H
#define TRANSACTIONSYSTEM_HELPER_H

#include <thread>
#include <vector>
#include <xmmintrin.h>
#include "atomic_wrapper.h"
#include "debug.h"

static void sleepMs(int ms){
  std::this_thread::sleep_for(
    std::chrono::milliseconds(ms)
  );
}

static bool isReady(const std::vector<char> &readyFlags){
  for(auto &flag: readyFlags){
    if(!loadAcquire(flag)) return false;
  }
  return true;
}

static void waitForReady(const std::vector<char> &readyFlags){
  while(!isReady(readyFlags)){
    _mm_pause();
  }
}
static size_t decideParallelTableBuildNumber(size_t tupleNum){
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

// Clockのspanが閾値を超えたか
static inline bool checkClockSpan(
  const uint64_t start,
  const uint64_t stop,
  const uint64_t threshold
){
  uint64_t diff = stop - start;
  return diff > threshold;
}

#endif //TRANSACTIONSYSTEM_HELPER_H
