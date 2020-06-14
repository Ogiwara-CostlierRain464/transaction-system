#include "txn_executor.h"

static void genStringRepeatedNumber(){

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
