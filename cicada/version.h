#ifndef TRANSACTIONSYSTEM_VERSION_H
#define TRANSACTIONSYSTEM_VERSION_H

#include <atomic>
#include "common.h"

enum VersionStatus{
  InValid,
  Pending,
  Aborted,
  // PreCommitted, //unused?
  Committed,
  Deleted,
  Unused
};

struct Version{
  alignas(CACHE_LINE_SIZE) std::atomic<uint64_t> rts;
  std::atomic<uint64_t> wts;
  std::atomic<Version*> next;
  std::atomic<VersionStatus> status;

  char value[VALUE_SIZE];

  Version(){
    status.store(Pending, std::memory_order_release);
    next.store(nullptr, std::memory_order_release);
  }

  Version(const uint64_t rts_, const uint64_t wts_){
    rts.store(rts_, std::memory_order_relaxed);
    wts.store(wts_, std::memory_order_relaxed);
    status.store(Pending, std::memory_order_release);
    next.store(nullptr, std::memory_order_release);
  }

  Version *lastCommittedVersionAfterThis(){
    Version* version = this;
    assert(false);
  }


};

#endif //TRANSACTIONSYSTEM_VERSION_H
