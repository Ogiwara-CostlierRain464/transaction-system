#ifndef TRANSACTIONSYSTEM_TIME_STAMP_H
#define TRANSACTIONSYSTEM_TIME_STAMP_H

#include "consts.h"
#include "../common/tsc.h"

/**
 * """
 * Timestamps are generated by combining three factors:
 * the current local clock, a clock boost, and the threadID.
 * """
 *
 * cf. §3.1 Timestamp Allocation.
 */
struct TimeStamp{
  alignas(CACHE_LINE_SIZE) uint64_t body = 0;
  uint64_t  localClock = 0;
  uint64_t  clockBoost = 0;
  uint8_t  threadId;

  /**
   * 最初にTimeStampを初期化するときに使う
   */
  inline void generateTimeStampFirst(uint8_t threadId_){
    localClock = rdtscp();
    body = (localClock << (sizeof(threadId_) * 8)) | threadId_;
    threadId = threadId_;
  }

  /**
   * 二回目以降、clockBoostなどを元にtsを作るときに使う
   */
  inline void generateTimeStamp(uint8_t threadId_){
    uint64_t tmp = rdtscp();
    uint64_t elapsedTime = tmp - localClock;
    if(tmp < localClock) elapsedTime = 0;

    localClock += elapsedTime;
    localClock += clockBoost;

    body = (localClock << (sizeof(threadId_) * 8)) | threadId_;
  }

  inline void setClockBoost(size_t clock_per_us){
    clockBoost = clock_per_us;
  }
};

#endif //TRANSACTIONSYSTEM_TIME_STAMP_H
