#include "worker.h"
#include <thread>

KVSilo::Worker::Worker(
  size_t worker_id,
  KVSilo::Logger *logger_,
  PrimaryTree *tree)
: workerId(worker_id)
, logger(logger_)
, primaryTree(tree)
{
}


void KVSilo::Worker::run() {

  Query query;

  // 新しいQueryが来るまでまつ
  while(!waitingQueries.try_dequeue(query)){
    std::this_thread::yield();
  }

  // ここでqueryを実行することにより、Worker上で行うのと同じ意味になる
  Transaction trn(primaryTree);
  query(trn);
}

void KVSilo::Worker::addQueryToQueue(const Query &query) {
  waitingQueries.enqueue(query);
}

