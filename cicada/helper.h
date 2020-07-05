#ifndef TRANSACTIONSYSTEM_HELPER_H
#define TRANSACTIONSYSTEM_HELPER_H

#include <thread>
#include <xmmintrin.h>
#include "../common/atomic_wrapper.h"

/**
 * Utility functionをまとめる
 */

#define ERR         \
do {                \
  perror("ERROR");  \
  exit(1);          \
} while(0)

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

#endif //TRANSACTIONSYSTEM_HELPER_H
