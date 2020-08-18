#ifndef TRANSACTIONSYSTEM_LOGGER_H
#define TRANSACTIONSYSTEM_LOGGER_H

#include "singleton.h"
#include "concurrentqueue.h"
#include "silo_env.h"
#include <thread>

namespace KVSilo {

class Logger : public NonCopyable, NonMovable{
public:
  explicit Logger(SiloEnv &env);

  void run();

private:
  SiloEnv &env;
};

}


#endif //TRANSACTIONSYSTEM_LOGGER_H
