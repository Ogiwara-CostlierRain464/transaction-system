#ifndef TRANSACTIONSYSTEM_STEP_H
#define TRANSACTIONSYSTEM_STEP_H

#include <cstdint>

enum class Operation: uint8_t{
  Read,
  Write
};

/**
 * Corresponds to w(x) or r(y).
 */
struct Step{
  Operation operation;
  uint64_t key;

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

#endif //TRANSACTIONSYSTEM_STEP_H
