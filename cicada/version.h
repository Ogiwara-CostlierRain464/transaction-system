#ifndef TRANSACTIONSYSTEM_VERSION_H
#define TRANSACTIONSYSTEM_VERSION_H

#include <atomic>
#include "common.h"

enum VersionStatus : uint8_t {
  Pending,
  Aborted,
  Committed,
  Deleted,
  Unused
};

/**
 * """
 * A version contains
 * (1) a write timestamp(wts) that is the timestamp
 * of the transaction that has created this version.
 * (2) a read timestamp(rts) that indicates the maximum
 * timestamp of (possibly) committed transactions
 * that read this version.
 * (3) the record data
 * (4) commit status that indicates the validity of this version
 * (5) allocation information including the NUMA node ID
 * for NUMA-aware allocation and the version size.
 *
 * All fields (except rts and status) are immutable.
 * """
 *
 * cf. §3.2 Multi-Version Execution.
 */
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

  void atomicSet(
    const uint64_t rts_, const uint64_t wts_,
    Version *next_, const VersionStatus status_
    ){
    rts.store(rts_, std::memory_order_relaxed);
    wts.store(wts_, std::memory_order_relaxed);
    status.store(status_, std::memory_order_release);
    next.store(next_, std::memory_order_release);
  }

};

#endif //TRANSACTIONSYSTEM_VERSION_H
