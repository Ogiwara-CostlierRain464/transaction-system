#ifndef TRANSACTIONSYSTEM_LEADER_WORK_H
#define TRANSACTIONSYSTEM_LEADER_WORK_H

#include "../common/backoff.h"
#include "consts.h"
#include "../common/atomic_wrapper.h"

/**
 * """
 * The thread then declares a quiescent state by setting
 * a per-second flag regularly. If a leader thread sees
 * that every flag has been set, it resets all flags and
 * monotonically updates min_wts and min_rts, which store
 * the global minimum of (thread.wts) and (thread.rts) for
 * all threads.
 *
 * c.f. §3.8 Rapid Garbage Collection.
 * """
 */
static void leaderWork(Backoff &backoff){
  bool gcUpdate = true;
  for(size_t i = 0; i < THREAD_NUM; ++i){
    if(loadAcquire(GCFlag[i].body) == 0){
      gcUpdate = false;
      break;
    }
  }

  if(gcUpdate){
    uint64_t minWrite = loadAcquire(ThreadWtsArray[1].body); // ０は無視して1から、ここ0では？？？
    uint64_t minRead;

    minRead = loadAcquire(ThreadRtsArray[1].body);

    // NOTE: why index starts with 1? why not 0?
    // 0はleader thread用？

    for(size_t i = 1; i < THREAD_NUM; ++i){
      uint64_t  tmp = loadAcquire(ThreadWtsArray[i].body);

      minWrite = std::min(minWrite, tmp);


      tmp = loadAcquire(ThreadRtsArray[i].body);
      minRead = std::min(minRead, tmp);
    }

    MinWts.store(minWrite, std::memory_order_release);
    MinRts.store(minRead, std::memory_order_release);

    for(size_t i = 0; i < THREAD_NUM; ++i){
      storeRelease(GCFlag[i].body, 0);
      storeRelease(GCExecuteFlag[i].body, 1);
    }
  }
}

#endif //TRANSACTIONSYSTEM_LEADER_WORK_H
