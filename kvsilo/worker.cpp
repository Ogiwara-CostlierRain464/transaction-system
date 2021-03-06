#include "worker.h"
#include <thread>
#include <xmmintrin.h>

KVSilo::Worker::Worker(
  size_t worker_id,
  KVSilo::Logger *logger_,
  PrimaryTree *tree,
  SiloEnv *env_)
: workerId(worker_id)
, logger(logger_)
, primaryTree(tree)
, env(env_)
{
}


void KVSilo::Worker::run() {
  while(!env->start.load(std::memory_order_acquire)){
    _mm_pause();
  }

  while(!env->stop.load(std::memory_order_acquire)){
    Query query;

    // 新しいQueryが来るまでまつ
    while(!waitingQueries.try_dequeue(query)){
      if(env->stop.load(std::memory_order_acquire)){
        return;
      }

      _mm_pause();
    }

    // ewを更新
    env->workerE[workerId]->store(
      env->E.load(std::memory_order_acquire),
      std::memory_order_release
    );

    // ここでqueryを実行することにより、Worker上で行うのと同じ意味になる
    Transaction trn(primaryTree, env);
    query(trn);
    trn.commit(result);
  }
}


void KVSilo::Worker::addQueryToQueue(const Query &query) {
  waitingQueries.enqueue(query);
}

