#ifndef TRANSACTIONSYSTEM_RESULT_H
#define TRANSACTIONSYSTEM_RESULT_H

#include <cstdint>
#include <iostream>
#include "common.h"

struct Result{
  alignas(CACHE_LINE_SIZE) uint64_t localAbortCounts = 0;
  uint64_t localCommitCounts = 0;
  uint64_t totalAboutCounts = 0;
  uint64_t totalCommitCounts = 0;

  void addLocalAllResult(const Result &other){
    totalAboutCounts += other.localAbortCounts;
    totalCommitCounts += other.localCommitCounts;
  }

  void displayAllResult(){
    std::cout <<
    //" Local commits " << localCommitCounts <<
    " Total aborts " << totalAboutCounts <<
    " Total commits " << totalCommitCounts << std::endl;
  }

};

#endif //TRANSACTIONSYSTEM_RESULT_H
