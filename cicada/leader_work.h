#ifndef TRANSACTIONSYSTEM_LEADER_WORK_H
#define TRANSACTIONSYSTEM_LEADER_WORK_H

#include "../common/backoff.h"
#include "consts.h"
#include "../common/atomic_wrapper.h"

static void leaderWork(Backoff &backoff){
  bool gcUpdate = true;
  for(size_t i = 0; i < THREAD_NUM; ++i){
    if(loadAcquire(GCFlag[i].body) == 0){
      gcUpdate = false;
      break;
    }
  }
  if(gcUpdate){
    uint64_t minWrite = loadAcquire(ThreadWtsArray[1].body);
    uint64_t minRead;
    if(GROUP_COMMIT == 0){
      minRead = loadAcquire(ThreadRtsArray[1].body);
    }else{
      minRead = loadAcquire(ThreadRtsArrayForGroup[1].body);
    }

    for(size_t i = 1; i < THREAD_NUM; ++i){
      uint64_t  tmp = loadAcquire(ThreadWtsArray[i].body);


    }
  }
}

#endif //TRANSACTIONSYSTEM_LEADER_WORK_H
