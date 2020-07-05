#include <iostream>
#include "common.h"
#include "helper.h"
#include "version.h"
#include "tuple.h"

void init(){
  if(posix_memalign((void**)&ThreadRtsArrayForGroup,
    CACHE_LINE_SIZE,
    THREAD_NUM * sizeof(uint64_t_64byte)) != 0){
    ERR;
  }
  if(posix_memalign((void**)&ThreadWtsArray,
    CACHE_LINE_SIZE,
    THREAD_NUM * sizeof(uint64_t_64byte)) != 0){
    ERR;
  }
  if(posix_memalign((void**)&ThreadRtsArray,
     CACHE_LINE_SIZE,
     THREAD_NUM * sizeof(uint64_t_64byte)) != 0){
    ERR;
  }
  if(posix_memalign((void**)&GroupCommitIndex,
     CACHE_LINE_SIZE,
     THREAD_NUM * sizeof(uint64_t_64byte)) != 0){
    ERR;
  }
  if(posix_memalign((void**)&GroupCommitCounter,
    CACHE_LINE_SIZE,
    THREAD_NUM * sizeof(uint64_t_64byte)) != 0){
    ERR;
  }
  if(posix_memalign((void**)&GCFlag,
    CACHE_LINE_SIZE,
    THREAD_NUM * sizeof(uint64_t_64byte)) != 0){
    ERR;
  }
  if(posix_memalign((void**)&GCExecuteFlag,
    CACHE_LINE_SIZE,
    THREAD_NUM * sizeof(uint64_t_64byte)) != 0){
    ERR;
  }

  SLogSet = new Version* [MAX_OPERATIONS * GROUP_COMMIT];
  PLogSet = new Version** [THREAD_NUM];

  for(size_t i = 0; i < THREAD_NUM; ++i){
    PLogSet[i] = new Version* [MAX_OPERATIONS * GROUP_COMMIT];
  }

  for(size_t i = 0; i < THREAD_NUM; ++i){
    GCFlag[i].body = 0;
    GCExecuteFlag[i].body = 0;
    GroupCommitIndex[i].body = 0;
    GroupCommitCounter[i].body = 0;
    ThreadRtsArray[i].body = 0;
    ThreadWtsArray[i].body = 0;
    ThreadRtsArrayForGroup[i].body = 0;
  }

  if(posix_memalign((void**)&Table,
    PAGE_SIZE,
    TUPLE_NUM * sizeof(Tuple)) != 0){
    ERR;
  }


}

int main(){
  printf("Hi!");
  return 0;
}