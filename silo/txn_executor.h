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
enum TXNStatus{
  Active, Committed, Aborted
};

/**
 * Transaction Executor
 * Transaction stands for TXN.
 */
class TXNExecutor {
public:
  explicit TXNExecutor(size_t threadId);

  void abort();
  void begin();
  void displayWriteSet();
  void lockWriteSet();
  void read(uint64_t key);
  /**
   * Search element of local set corresponding to given key.
   *
   */
  ReadElement<Tuple> *searchReadSet(uint64_t key);
  WriteElement<Tuple> *searchWriteSet(uint64_t key);

  void unlockWriteSet();

  bool validationPhase();

  void write(uint64_t key);
  void writePhase();

  std::vector<Step> steps;
private:
  size_t threadId;
  std::vector<ReadElement<Tuple>> readSet;
  std::vector<WriteElement<Tuple>> writeSet;
  TXNStatus status;

  TidWord maxReadTid, maxWriteTid;

  char writeValue[VALUE_SIZE];
  char returnValue[VALUE_SIZE];
};


#endif //TRANSACTIONSYSTEM_TXN_EXECUTOR_H
