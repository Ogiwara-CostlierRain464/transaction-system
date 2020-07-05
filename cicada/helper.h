#ifndef TRANSACTIONSYSTEM_HELPER_H
#define TRANSACTIONSYSTEM_HELPER_H

#include <thread>

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

#endif //TRANSACTIONSYSTEM_HELPER_H
