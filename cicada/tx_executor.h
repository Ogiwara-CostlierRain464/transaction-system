#ifndef TRANSACTIONSYSTEM_TX_EXECUTOR_H
#define TRANSACTIONSYSTEM_TX_EXECUTOR_H

#include <vector>
#include "time_stamp.h"

enum TXStatus{
  InValid,
  InFlight,
  Commit,
  Abort
};

struct TXExecutor {
  TXStatus status = InValid;
  TimeStamp wts;
};


#endif //TRANSACTIONSYSTEM_TX_EXECUTOR_H
