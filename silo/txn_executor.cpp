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

  Tuple *tuple;
}
