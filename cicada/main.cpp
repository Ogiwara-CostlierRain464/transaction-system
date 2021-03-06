#include <iostream>
#include "consts.h"
#include "version.h"
#include "tuple.h"
#include "time_stamp.h"
#include "../common/xoroshiro128_plus.h"
#include "tx_executor.h"
#include "../common/zip_fian.h"
#include "../common/backoff.h"
#include "../common/debug.h"
#include "../common/helper.h"
#include "../common/result.h"
#include "leader_work.h"

void aligned_memory_alloc(void** ptr, size_t alignment, size_t len){
  assert(len >= alignment && "wrong parameter");

#ifdef _WIN32
  ptr = _aligned_malloc(len, alignment);
  if(ptr == nullptr)
    ERR;
#else
  if(posix_memalign(ptr, alignment, len) != 0){
    ERR;
  }
#endif
}

void init(uint64_t *initialWts){
  aligned_memory_alloc((void**) &ThreadWtsArray,
                       CACHE_LINE_SIZE,
                       THREAD_NUM * sizeof(uint64_t_64byte) );

  aligned_memory_alloc((void**) &ThreadRtsArray,
                       CACHE_LINE_SIZE,
                       THREAD_NUM * sizeof(uint64_t_64byte) );

  aligned_memory_alloc((void**) &GCFlag,
                       CACHE_LINE_SIZE,
                       THREAD_NUM * sizeof(uint64_t_64byte));

  aligned_memory_alloc((void**) &GCExecuteFlag,
                       CACHE_LINE_SIZE,
                       THREAD_NUM * sizeof(uint64_t_64byte));


  SLogSet = new Version* [MAX_OPERATIONS * GROUP_COMMIT];
  PLogSet = new Version** [THREAD_NUM];

  for(size_t i = 0; i < THREAD_NUM; ++i){
    PLogSet[i] = new Version* [MAX_OPERATIONS * GROUP_COMMIT];
  }

  for(size_t i = 0; i < THREAD_NUM; ++i){
    GCFlag[i].body = 0;
    GCExecuteFlag[i].body = 0;
    ThreadRtsArray[i].body = 0;
    ThreadWtsArray[i].body = 0;
  }

  aligned_memory_alloc((void**) &Table,
                       PAGE_SIZE,
                       TUPLE_NUM * sizeof(Tuple));


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

void worker(
  size_t threadId,
  char &ready,
  bool &start,
  bool &quit){
  Xoroshiro128Plus random(32);
  TXExecutor txExecutor(threadId, &CicadaResult[threadId]);
  Result &result = CicadaResult[threadId];
  FastZipf zipf(&random, ZIPF_SKEW, TUPLE_NUM);
  Backoff backoff(CLOCKS_PER_US);

  storeRelease(ready, 1);
  while(!loadAcquire(start)) _mm_pause();
  while(!loadAcquire(quit)){
    makeSteps(txExecutor.steps, random, zipf, TUPLE_NUM, MAX_OPERATIONS, READ_RATIO, result);

  RETRY:
    if(threadId == 0){ // leader thread
      leaderWork(std::ref(backoff));
    }

    if(loadAcquire(quit)) break;

    txExecutor.begin();
    for(auto &&it: txExecutor.steps){
      switch (it.operation) {
        case Operation::Read:
          txExecutor.read(it.key);
        break;
        case Operation::Write:
          txExecutor.write(it.key);
        break;
        case Operation::RMW:
          txExecutor.read(it.key);
          txExecutor.write(it.key);
        break;
        default:
          ERR;
      }

      if(txExecutor.status == TXStatus::Abort){

      }
    }
  }
}

int main(){
  uint64_t initialWts;
  init(&initialWts);
  // なぜ+2 => recordを生成した時より適当に時間をすすめるため、+1だと
  // thread.rtsに-1を入れるとinitialWtsと同じになってややこしいから
  MinWts.store(initialWts + 2);

  alignas(CACHE_LINE_SIZE) bool start = false;
  alignas(CACHE_LINE_SIZE) bool quit = false;

  CicadaResult.resize(THREAD_NUM);

  std::vector<char> readyFlags(THREAD_NUM);
  std::vector<std::thread> threads;
  threads.reserve(THREAD_NUM);

  for(size_t i = 0; i < THREAD_NUM; ++i){
    threads.emplace_back(worker,
      i, std::ref(readyFlags[i]), std::ref(start), std::ref(quit));
  }

  waitForReady(readyFlags);
  storeRelease(start, true);

  sleepMs(1000 * EX_TIME);

  storeRelease(quit, true);
  for(auto &thread: threads){
    thread.join();
  }

  for(size_t i = 0; i < THREAD_NUM; ++i){
    CicadaResult[0].addLocalAllResult(CicadaResult[i]);
  }

  CicadaResult[0].displayAllResult(EX_TIME);
  //delete DB

  return 0;
}