#ifndef TRANSACTIONSYSTEM_LOGGER_H
#define TRANSACTIONSYSTEM_LOGGER_H

#include "singleton.h"
#include "concurrentqueue.h"
#include <thread>

namespace KVSilo {

class Logger : public NonCopyable, NonMovable{
public:
  void run();

private:
};

}


#endif //TRANSACTIONSYSTEM_LOGGER_H
