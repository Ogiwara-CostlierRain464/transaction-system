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

TXNExecutor::TXNExecutor(size_t threadId, Result *result)
: threadId(threadId)
, result(result)
{
  readSet.reserve(MAX_OPERATIONS);
  writeSet.reserve(MAX_OPERATIONS);
  steps.reserve(MAX_OPERATIONS);

  maxReadTid.body = 0;
  maxWriteTid.body = 0;

  genStringRepeatedNumber(writeValue, VALUE_SIZE, threadId);
}

void TXNExecutor::abort() {
  readSet.clear();
  writeSet.clear();
}

void TXNExecutor::begin() {
  status = Active;
  maxReadTid.body = 0;
  maxWriteTid.body = 0;
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
    expected.body = loadAcquire(itr.recordPtr->tidWord.body);

    // Wait until could acquire lock.
    for(;;){
      if(expected.lock){
        expected.body = loadAcquire(itr.recordPtr->tidWord.body);
      } else{
        desired = expected;
        desired.lock = true;
        // lockを確保を試みる
        if(compareExchange(itr.recordPtr->tidWord.body, expected.body, desired.body)){
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

  expected.body = loadAcquire(tuple->tidWord.body);

  for(;;) {

    // (a) reads the TID word, spinning until the lock is clear.
    while (expected.lock) {
      expected.body = loadAcquire(tuple->tidWord.body);
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
    check.body = loadAcquire(tuple->tidWord.body);
    if (expected == check){
      break; // TID didn't change
    }else{
      expected = check;
      continue; // TID changes, so retry.
    }
  }

  readSet.emplace_back(key, tuple, returnValue, expected);
}

ReadElement<Tuple> *TXNExecutor::searchReadSet(uint64_t key) {
  // linear search
  for(auto &op: readSet){
    if(op.key == key) return &op;
  }
  return nullptr;
}

WriteElement<Tuple> *TXNExecutor::searchWriteSet(uint64_t key) {
  // linear search
  for(auto &op: writeSet){
    if(op.key == key) return &op;
  }
  return nullptr;
}

void TXNExecutor::unlockWriteSet() {
  TidWord expected, desired;

  for(auto &op: writeSet){
    expected.body = loadAcquire(op.recordPtr->tidWord.body);
    desired = expected;
    desired.lock = false;
    storeRelease(op.recordPtr->tidWord.body, desired.body);
  }
}

/**
 * cf. P6 Figure 2
 */
bool TXNExecutor::validationPhase() {
  // Phase 1
  std::sort(writeSet.begin(), writeSet.end());
  lockWriteSet();

  asm volatile("" ::: "memory");
  atomicStoreThreadLocalEpoch(threadId, atomicLoadGlobalEpoch());
  asm volatile("" ::: "memory");

  // Phase 2
  TidWord check;
  for(auto &op: readSet){
    check.body = loadAcquire(op.recordPtr->tidWord.body);
    if(op.tidWord.tid != check.tid
    // || !check.latest
    || (check.lock && !searchWriteSet(op.key))
    || op.tidWord.epoch != check.epoch){
      // abort
      status = Aborted;
      unlockWriteSet();
      return false;
    }
    maxReadTid = std::max(maxReadTid, check);
  }

  // Phase 3
  // NOTE: code is omitted in original?
  status = Committed;
  return true;
}

void TXNExecutor::write(uint64_t key) {
  // re-writeを防ぐ
  if(searchWriteSet(key)) return;

  // TODO: implement each step (a)~(c).
  Tuple *tuple;
  ReadElement<Tuple> *re;
  re = searchReadSet(key);
  if(re){
    tuple = re->recordPtr;
  }else{
    tuple = &Table[key];
  }

  writeSet.emplace_back(key, tuple);
}

void TXNExecutor::writePhase() {
  // TODO
}
