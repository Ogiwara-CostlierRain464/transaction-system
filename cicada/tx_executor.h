#ifndef TRANSACTIONSYSTEM_TX_EXECUTOR_H
#define TRANSACTIONSYSTEM_TX_EXECUTOR_H

#include <vector>
#include <deque>
#include "time_stamp.h"
#include "op_element.h"
#include "../common/step.h"

enum TXStatus{
  InValid,
  InFlight,
  Commit,
  Abort
};

struct TXExecutor {
  TXStatus status = InValid;
  TimeStamp wts;
  std::vector<ReadElement<Tuple>> readSet;
  std::vector<WriteElement<Tuple>> writeSet;
  std::deque<GCElement<Tuple>> GCQueue;
  std::deque<Version*> reuseVersionFromGC;
  std::vector<Step> steps;
  Result *result = nullptr;

  char returnValue[VALUE_SIZE] = {};
  char writeValue[VALUE_SIZE] = {};

  bool readOnly;
  uint8_t threadId = 0;
  uint64_t rts;
  // for one-sided synchronization
  uint64_t start, stop;
  // for group commit
  uint64_t groupCommitStart, groupCommitStop;
  // for garbage collection
  uint64_t GCStart, GCStop;

  TXExecutor(uint8_t threadID, Result *result);
  ~TXExecutor();

  void abort();
  void begin();
  void read(const uint64_t &key);
  void write(const uint64_t &key);
  bool validation();
  void writePhase();

  ReadElement<Tuple> *searchReadSet(uint64_t key);
  WriteElement<Tuple> *searchWriteSet(uint64_t key);
};


#endif //TRANSACTIONSYSTEM_TX_EXECUTOR_H
