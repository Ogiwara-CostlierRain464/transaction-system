#ifndef TRANSACTIONSYSTEM_RESULT_H
#define TRANSACTIONSYSTEM_RESULT_H

#include <cstdint>
#include "common.h"

struct Result{
  alignas(CACHE_LINE_SIZE) uint64_t localAbortCounts = 0;
  uint64_t localCommitCounts = 0;
  uint64_t totalAboutCounts = 0;
  uint64_t totalCommitCounts = 0;


};

#endif //TRANSACTIONSYSTEM_RESULT_H
