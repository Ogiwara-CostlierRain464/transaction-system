#ifndef TRANSACTIONSYSTEM_WORKER_H
#define TRANSACTIONSYSTEM_WORKER_H

#include <cstddef>
#include <thread>
#include "singleton.h"
#include "logger.h"
#include "concurrentqueue.h"
#include "transaction.h"
#include "primary_tree.h"
#include "silo_env.h"

namespace KVSilo{

class Worker: NonCopyable, NonMovable{
public:
  using Query = std::function<void(Transaction&)>;

  explicit Worker(size_t worker_id, Logger *logger, PrimaryTree *tree, SiloEnv *env);

  // main working
  void run();


  void addQueryToQueue(const Query &query);

private:
  size_t workerId;
  // Correspond logger.
  Logger *logger;
  PrimaryTree *primaryTree;
  SiloEnv *env;


  moodycamel::ConcurrentQueue<Query> waitingQueries;
};
}

#endif //TRANSACTIONSYSTEM_WORKER_H
