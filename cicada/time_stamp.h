#ifndef TRANSACTIONSYSTEM_TIME_STAMP_H
#define TRANSACTIONSYSTEM_TIME_STAMP_H

#include "common.h"

struct TimeStamp{
  alignas(CACHE_LINE_SIZE) uint64_t ts = 0;
  uint64_t  localClock = 0;
  uint64_t  clockBoost = 0;
  uint8_t  threadId;


};

#endif //TRANSACTIONSYSTEM_TIME_STAMP_H
