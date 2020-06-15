#include "txn_executor.h"
#include "atomic_wrapper.h"

/**
 * Generate write value for this thread.
 */
static void genStringRepeatedNumber(
  char *outString,
  size_t valueSize,
  size_t threadId){
  size_t digit = 1, tmp = threadId;
  for(;;){
    tmp /= 10;
    if(tmp != 0)
      ++digit;
    else
      break;
  }
  sprintf(outString, "%ld", threadId);
  for(size_t i = digit; i < valueSize - 2; i++){
    outString[i] = '0';
  }
}

TXNExecutor::TXNExecutor(size_t threadId)
: threadId(threadId)
{
  readSet.reserve(MAX_OPERATIONS);
  writeSet.reserve(MAX_OPERATIONS);
  steps.reserve(MAX_OPERATIONS);

  maxReadTid.obj = 0;
  maxWriteTid.obj = 0;

  genStringRepeatedNumber(writeValue, VALUE_SIZE, threadId);
}

void TXNExecutor::abort() {
  readSet.clear();
  writeSet.clear();
}

void TXNExecutor::begin() {
  status = Active;
  maxReadTid.obj = 0;
  maxWriteTid.obj = 0;
}

void TXNExecutor::displayWriteSet() {
  printf("display_write_set()\n");
  printf("-------------------\n");
  for(auto &writeOp: writeSet){
    printf("key\t:\t%llu\n", writeOp.key);
  }
}

void TXNExecutor::lockWriteSet() {
  TidWord expected, desired;

  for(auto &itr : writeSet){
    expected.obj = loadAcquire(itr.recordPtr->tidWord.obj);

    // Wait until could acquire lock.
    for(;;){
      if(expected.lock){
        expected.obj = loadAcquire(itr.recordPtr->tidWord.obj);
      } else{
        desired = expected;
        desired.lock = true;
        // lockを確保を試みる
        if(compareExchange(itr.recordPtr->tidWord.obj, expected.obj, desired.obj)){
          break;
        }
      }
    }

    maxWriteTid = std::max(maxWriteTid, expected);
  }
}

/**
 * @see page 7.
 */
void TXNExecutor::read(uint64_t key) {
  TidWord expected, check;

  /**
   * read-own-writesとre-read form local read setを防ぐ
   */
  if(searchReadSet(key) || searchWriteSet(key)){
    return;
  }

  Tuple *tuple = &Table[key];

  expected.obj = loadAcquire(tuple->tidWord.obj);

  for(;;) {

    // (a) reads the TID word, spinning until the lock is clear.
    while (expected.lock) {
      expected.obj = loadAcquire(tuple->tidWord.obj);
    }

    // (b) checks whether the record is the latest version.
    ; // This repository is implemented by single version, thus omitted

    // (c) reads the data
    memcpy(returnValue, tuple->value, VALUE_SIZE);

    // (d) performs a memory fence
    ; // Order of load doesn't exchange, thus not needed.

    // (e) checks the TID word again.
    // If the record is not the latest version in step (b)
    // or the TID word changes between steps (a) and (e),
    // the worker must retry or abort.
    check.obj = loadAcquire(tuple->tidWord.obj);
    if (expected == check){
      break; // TID didn't change
    }else{
      continue; // TID changes, so retry.
    }
  }

  readSet.emplace_back(key, tuple, returnValue, expected);
}
