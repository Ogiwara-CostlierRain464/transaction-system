#ifndef TRANSACTIONSYSTEM_TUPLE_H
#define TRANSACTIONSYSTEM_TUPLE_H

#include <atomic>
#include "consts.h"

struct Tuple{
  std::atomic<Version *> latest;
  std::atomic<uint64_t> minWts;
  std::atomic<uint64_t> continuingCommit; // 独自？
  // gc用のlock機構
  std::atomic<uint8_t> gcLock;

  Tuple()
  : latest(nullptr), gcLock(0){}

  Version *loadAcquireLatest(){
    return latest.load(std::memory_order_acquire);
  }

  // GCするときに、thread_idがこれをGCする権利を得る
  bool getGCRight(uint8_t thread_id){
    uint8_t expected, desired(thread_id);
    expected = gcLock.load();
    for(;;){
      // もう誰かに取られてたらGCの権利はえられない
      if(expected != 0) return false;
      if(this->gcLock.compare_exchange_strong(expected, desired)) return true;
    }
  }

  void returnGCRight(){
    this->gcLock.store(0);
  }
};

#endif //TRANSACTIONSYSTEM_TUPLE_H
