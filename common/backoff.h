#ifndef TRANSACTIONSYSTEM_BACKOFF_H
#define TRANSACTIONSYSTEM_BACKOFF_H

#include <cstdint>
#include <cstddef>
#include "tsc.h"

struct Backoff{
  uint64_t lastTime = 0;
  size_t clockPerUs;

  Backoff(size_t clockPerUs_)
  : lastTime(rdtscp())
  , clockPerUs(clockPerUs_)
  {}
};

#endif //TRANSACTIONSYSTEM_BACKOFF_H
