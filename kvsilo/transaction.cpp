#include "transaction.h"
#include "silo_env.h"
#include <cassert>
#include <algorithm>
#include <atomic>

KVSilo::Transaction::Transaction(PrimaryTree *tree, SiloEnv *env_)
: primaryTree(tree)
, env(env_)
{}


KVSilo::Transaction::Value KVSilo::Transaction::read(Key key) {
  Record *r = primaryTree->find(key);

  Value data;

  TidWord before;
  TidWord after;
  for(;;){
    do{
      before = r->tidWord.load(std::memory_order_release);
    }while(before.lock);

    assert(before.latest);

    data = r->value;

    std::atomic_thread_fence(std::memory_order_acq_rel);

    after = r->tidWord.load(std::memory_order_acquire);

    if(before == after){
      break;
    }else{
      continue;
    }
  }
  RSet.emplace_back(r, after);

  return data;
}

void KVSilo::Transaction::write(Key key, Value value) {
  Record *r = primaryTree->find(key);

  WSet.emplace_back(r, value);
}

void KVSilo::Transaction::commit(Result &result) {
  std::sort(WSet.begin(), WSet.end());
  lockWSet();

  std::atomic_thread_fence(std::memory_order_acquire);
  Epoch e = env->E.load(std::memory_order_acquire);
  std::atomic_thread_fence(std::memory_order_release);

  for(auto &read: RSet){
    Record *record = read.first;
    TidWord read_tid = read.second;

    TidWord record_tid = record->tidWord.load(std::memory_order_acquire);
    if(read_tid.tid != record_tid.tid
    || !record_tid.latest
    || (record_tid.lock && !searchWSet(record))
    ){
      unlockWSet();
      result.addAbort();
      return;
    }

    maxReadTid = std::max(maxReadTid, record_tid);
  }

  // Phase 3
  TidWord tidA, tidB, tidC;

  tidA = std::max(maxWriteTid ,maxReadTid);
  tidA.tid++;

  tidB = mostRecentlyChosenTid;
  tidB.tid++;

  tidC.epoch = e;

  TidWord maxTid = std::max({tidA, tidB, tidC});
  maxTid.lock = false;
  maxTid.latest = true;
  mostRecentlyChosenTid = maxTid;

  for(auto &w: WSet){
    w.first->value.store(w.second, std::memory_order_release);
    std::atomic_thread_fence(std::memory_order_acq_rel);
    w.first->tidWord.store(maxTid, std::memory_order_release);
  }

  WSet.clear();
  RSet.clear();
  result.addCommit();
}

void KVSilo::Transaction::lockWSet() {
  TidWord expected, desired;

  for(auto &w_elem : WSet){
    Record *r =w_elem.first;
    expected = r->tidWord.load(std::memory_order_acquire);

    for(;;){
      if(expected.lock){
        expected = r->tidWord.load(std::memory_order_acquire);
      }else{
        desired = expected;
        desired.lock = true;
        if(r->tidWord.compare_exchange_weak(expected, desired)){
          break;
        }
      }
    }

    maxWriteTid = std::max(maxWriteTid, expected);
  }
}

void KVSilo::Transaction::unlockWSet() {
  TidWord expected, desired;

  for(auto &w_elem: WSet){
    expected.body = w_elem.first->tidWord.load(std::memory_order_acquire).body;
    desired = expected;
    desired.lock = false;
    w_elem.first->tidWord.store(desired, std::memory_order_release);
  }
}


KVSilo::Record *KVSilo::Transaction::searchWSet(Record *ptr) {
  // liner search
  for(auto &w: WSet){
    if(w.first == ptr) return w.first;
  }
  return nullptr;
}