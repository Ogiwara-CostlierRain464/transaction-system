#ifndef TRANSACTIONSYSTEM_RESULT_H
#define TRANSACTIONSYSTEM_RESULT_H

#include <cstddef>
#include <cstdio>

namespace KVSilo{

struct Result{

  size_t aborts;
  size_t commits;

  void addAbort(){
    ++aborts;
  }

  void addCommit(){
    ++commits;
  }

  void printResult() const{
    printf("Commits: %zu, Aborts : %zu \n", commits, aborts);
  }

  /**
   * 他のResultインスタンスの内容を追記する
   * @param moved
   */
  void collect(Result &other){
    aborts += other.aborts;
    commits += other.commits;
  }
};

}

#endif //TRANSACTIONSYSTEM_RESULT_H
