#include <iostream>
#include "common.h"
#include "helper.h"
#include "version.h"
#include "tuple.h"
#include "time_stamp.h"

void init(uint64_t *initialWts){
  if(posix_memalign((void**)&ThreadRtsArrayForGroup,
    CACHE_LINE_SIZE,
    THREAD_NUM * sizeof(uint64_t_64byte)) != 0){
    ERR;
  }
  if(posix_memalign((void**)&ThreadWtsArray,
    CACHE_LINE_SIZE,
    THREAD_NUM * sizeof(uint64_t_64byte)) != 0){
    ERR;
  }
  if(posix_memalign((void**)&ThreadRtsArray,
     CACHE_LINE_SIZE,
     THREAD_NUM * sizeof(uint64_t_64byte)) != 0){
    ERR;
  }
  if(posix_memalign((void**)&GroupCommitIndex,
     CACHE_LINE_SIZE,
     THREAD_NUM * sizeof(uint64_t_64byte)) != 0){
    ERR;
  }
  if(posix_memalign((void**)&GroupCommitCounter,
    CACHE_LINE_SIZE,
    THREAD_NUM * sizeof(uint64_t_64byte)) != 0){
    ERR;
  }
  if(posix_memalign((void**)&GCFlag,
    CACHE_LINE_SIZE,
    THREAD_NUM * sizeof(uint64_t_64byte)) != 0){
    ERR;
  }
  if(posix_memalign((void**)&GCExecuteFlag,
    CACHE_LINE_SIZE,
    THREAD_NUM * sizeof(uint64_t_64byte)) != 0){
    ERR;
  }

  SLogSet = new Version* [MAX_OPERATIONS * GROUP_COMMIT];
  PLogSet = new Version** [THREAD_NUM];

  for(size_t i = 0; i < THREAD_NUM; ++i){
    PLogSet[i] = new Version* [MAX_OPERATIONS * GROUP_COMMIT];
  }

  for(size_t i = 0; i < THREAD_NUM; ++i){
    GCFlag[i].body = 0;
    GCExecuteFlag[i].body = 0;
    GroupCommitIndex[i].body = 0;
    GroupCommitCounter[i].body = 0;
    ThreadRtsArray[i].body = 0;
    ThreadWtsArray[i].body = 0;
    ThreadRtsArrayForGroup[i].body = 0;
  }

  if(posix_memalign((void**)&Table,
    PAGE_SIZE,
    TUPLE_NUM * sizeof(Tuple)) != 0){
    ERR;
  }

  TimeStamp timeStamp;
  timeStamp.generateTimeStampFirst(0);

  *initialWts = timeStamp.body;

  size_t maxThread = decideParallelTableBuildNumber(TUPLE_NUM);
  auto partTableInit = [](
    size_t threadId, uint64_t initTimeStamp,
    uint64_t start, uint64_t end){
    for(size_t i = start; i <= end; ++i){
      Tuple *tuple;
      tuple = &Table[i];
      tuple->minWts = initTimeStamp;
      tuple->gcLock.store(0, std::memory_order_release);
      tuple->continuingCommit.store(0, std::memory_order_release);

      tuple->latest.store(new Version(), std::memory_order_release);
      (tuple->latest.load(std::memory_order_acquire))
      ->atomicSet(0, initTimeStamp, nullptr, Committed);
      (tuple->latest.load(std::memory_order_acquire))
      ->value[0] = '\0';
    }
  };

  std::vector<std::thread> threads;
  for(size_t i =0 ; i < maxThread; ++i){
    threads.emplace_back(partTableInit,
      i ,timeStamp.body,
      i * (TUPLE_NUM / maxThread),  (i+1) * (TUPLE_NUM / maxThread) -1);
  }
  for(auto &thread: threads){
    thread.join();
  }
}

int main(){
  uint64_t initialWts;
  init(&initialWts);
  MinWts.store(initialWts + 2, )

  return 0;
}