#include "txn_executor.h"

/**
 * Generate write value for this thread.
 */
static void genStringRepeatedNumber(
  char *outString,
  int valueSize,
  int threadId){
  int digit = 1, tmp = threadId;
  for(;;){
    tmp /= 10;
    if(tmp != 0)
      ++digit;
    else
      break;
  }
  sprintf(outString, "%d", threadId);
  for(int i = digit; i < valueSize - 2; i++){
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


}
