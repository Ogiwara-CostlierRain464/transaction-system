#ifndef TRANSACTIONSYSTEM_GEN_STRIN_H
#define TRANSACTIONSYSTEM_GEN_STRIN_H

#include <cstddef>
#include <cstdio>

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

#endif //TRANSACTIONSYSTEM_GEN_STRIN_H
