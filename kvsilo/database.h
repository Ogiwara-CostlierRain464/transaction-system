#ifndef TRANSACTIONSYSTEM_DATABASE_H
#define TRANSACTIONSYSTEM_DATABASE_H

#include <functional>
#include <vector>
#include "singleton.h"
#include "transaction.h"
#include "worker.h"
#include "logger.h"
#include "primary_tree.h"

namespace KVSilo{

class Database: public NonMovable, NonCopyable{
public:
  using Query = std::function<void(Transaction&)>;

  explicit Database();

  void executeTransaction(const Query &query);

  /**
   * Shutdown all workers.
   */
  void terminate();


private:
  PrimaryTree primaryTree;

  std::vector<std::thread> workerThreads;
  std::vector<Worker*> workers;
  std::vector<std::thread> loggerThreads;
  std::vector<Logger*> loggers;
};

}

#endif //TRANSACTIONSYSTEM_DATABASE_H
