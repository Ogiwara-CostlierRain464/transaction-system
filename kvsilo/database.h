#ifndef TRANSACTIONSYSTEM_DATABASE_H
#define TRANSACTIONSYSTEM_DATABASE_H

#include "singleton.h"

namespace KVSilo{

class Database: public NonMovable, NonCopyable{
  explicit Database(){
    // init each worker.

    // まずLoggerを何個か立ち上げる
    // 次にWorkerを立ち上げて、各WorkerにLoggerを対応させる
    /
  }
};

}

#endif //TRANSACTIONSYSTEM_DATABASE_H
