#ifndef TRANSACTIONSYSTEM_WORKER_H
#define TRANSACTIONSYSTEM_WORKER_H

#include "singleton.h"

namespace KVSilo{

class Worker: NonMovable, NonCopyable{
  // main working
  void run();
};
}

#endif //TRANSACTIONSYSTEM_WORKER_H
