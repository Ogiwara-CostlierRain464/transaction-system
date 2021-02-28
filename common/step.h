#ifndef TRANSACTIONSYSTEM_STEP_H
#define TRANSACTIONSYSTEM_STEP_H

#include <cstddef>
#include <cstdio>

#include <cstdint>
#include "xoroshiro128_plus.h"
#include "zip_fian.h"

enum Operation: uint8_t{
  Read,
  Write
};

/**
 * Corresponds to w(x) or r(y).
 */
struct Step{
  Operation operation;
  uint64_t key;

  bool readOnly = false;
  bool writeOnly = false;

  Step(): operation(Operation::Read), key(0){}
  Step(Operation operation_, uint64_t key_)
    : operation(operation_), key(key_){}


  bool operator<(const Step &rhs)const{
    /**
     * Keyが同じならw -> rの順に
     */
    if(key == rhs.key
       && operation == Operation::Write
       && rhs.operation == Operation::Read){
      return true;
    }
    else if(key == rhs.key
            && operation == Operation::Write
            && rhs.operation == Operation::Write
      ){
      return true;
    }
    /**
     * Keyが同じ場合:
     *
     * ここまで来るとthis->operationはReadとなる
     * 以下がfalseとなることにより、rhs < this(r) になる
     *
     */

    return key < rhs.key;
  }
};


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
  // generate write value for this thread.
  sprintf(outString, "%ld", threadId);
  for(size_t i = digit; i < valueSize - 2; i++){
    outString[i] = '0';
  }
}

/**
 *
 * @param steps
 * @param rand
 * @param zipf
 * @param tupleNum
 * @param maxOp
 * @param ratio R/Wのうち、Readの割合(0~100)
 * @param res
 */
inline static void makeSteps(
  std::vector<Step> &steps,
  Xoroshiro128Plus &rand,
  FastZipf &zipf,
  size_t tupleNum,
  size_t maxOp,
  size_t ratio,
  Result &res
){
  steps.clear();
  bool readOnlyFlag{true}, writeOnlyFlag{true};
  for(size_t i = 0; i < maxOp; i++){
    uint64_t tmpKey;

    // アクセスするキーをランダムに決める
    //tmpKey = zipf() % tupleNum;
    tmpKey = zipf() & (tupleNum - 1); // x % 2 == x & 1, and & is much faster than %.
    // R/wをランダムに決定
    if((rand.next() % 100) < ratio){
      writeOnlyFlag = false;
      steps.emplace_back(Operation::Read, tmpKey);
    }else{
      readOnlyFlag = false;
      steps.emplace_back(Operation::Write, tmpKey);
    }
  }

  steps.begin()->readOnly = readOnlyFlag;
  steps.begin()->writeOnly = writeOnlyFlag;
}


#endif //TRANSACTIONSYSTEM_STEP_H
