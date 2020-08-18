#include "transaction.h"
#include "silo_env.h"
#include <cassert>
#include <algorithm>

KVSilo::Transaction::Transaction(PrimaryTree *tree, SiloEnv *env_)
: primaryTree(tree)
, env(env_)
{}


KVSilo::Transaction::Value KVSilo::Transaction::read(Key key) {
  Record *r = primaryTree->find(key);

  // what should I save to R set?
  // (record*, tid at that time)

  Value data;

  TidWord before;
  TidWord after;
  for(;;){
    do{
      before = r->tidWord.load(std::memory_order_relaxed);
    }while(before.lock);

    //assert(before.latest);

    data = r->value;

    std::atomic_thread_fence(std::memory_order_acq_rel);

    after = r->tidWord.load(std::memory_order_relaxed);

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

void KVSilo::Transaction::commit() {
  std::sort(WSet.begin(), WSet.end());
  lockWSet();

  std::atomic_thread_fence(std::memory_order_acquire);
  Epoch e = env->E.load(std::memory_order_relaxed);
  std::atomic_thread_fence(std::memory_order_release);

  for(auto &read: RSet){
    Record *r = read.first;
    TidWord tid = read.second;

    TidWord check = r->tidWord.load(std::memory_order_relaxed);
    if(tid.tid != check.tid){

      assert(false);
    }

    maxReadTid = std::max(maxReadTid, check);
  }


  // Phase 3
  TidWord tidA, tidB, tidC;

  tidA = std::max(maxWriteTid ,maxReadTid);
  tidA.tid++;

  tidB = mostRecentlyChosenTid;
  tidB.tid++;

  tidC.epoch = env->E.load(std::memory_order_relaxed);

  TidWord maxTid = std::max({tidA, tidB, tidC});
  maxTid.lock = false;
  maxTid.latest = true;
  mostRecentlyChosenTid = maxTid;

  for(auto &w: WSet){
    w.first->value = w.second;
    w.first->tidWord.store(maxTid, std::memory_order_relaxed);
  }

  WSet.clear();
  RSet.clear();
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
