#ifndef TRANSACTIONSYSTEM_TXN_EXECUTOR_H
#define TRANSACTIONSYSTEM_TXN_EXECUTOR_H

#include <cstdint>
#include <vector>
#include "op_elemet.h"
#include "tuple.h"
#include "step.h"

/**
 * TXN Status.
 * @see TXN System P128
 */
enum class TXNStatus: uint8_t {
  Active, Committed, Aborted
};

/**
 * Transaction Executor
 * Transaction stands for TXN.
 */
class TXNExecutor {
public:
  TXNExecutor(size_t threadId);

  void abort();
  void begin();
  void displayWriteSet();
  void lockWriteSet();
  void read();

private:
  size_t threadId;
  std::vector<ReadElement<Tuple>> readSet;
  std::vector<WriteElement<Tuple>> writeSet;
  std::vector<Step> steps;
  TXNStatus status;

  TidWord maxReadTid, maxWriteTid;

  char writeValues[VALUE_SIZE];
  char returnValues[VALUE_SIZE];
};


#endif //TRANSACTIONSYSTEM_TXN_EXECUTOR_H
