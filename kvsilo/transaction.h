#ifndef TRANSACTIONSYSTEM_TRANSACTION_H
#define TRANSACTIONSYSTEM_TRANSACTION_H

#include <cstddef>
#include "primary_tree.h"
#include "silo_env.h"
#include "result.h"
#include <utility>
#include <vector>

namespace KVSilo{

struct Operation{
  Key key;
};


class Transaction{
public:
  using Key = size_t;
  using Value = int;
  using RElement = std::pair<Record*, TidWord>;
  using WElement = std::pair<Record*, Value>;


  explicit Transaction(PrimaryTree *tree, SiloEnv *env);

  Value read(Key);
  void write(Key key, Value value);
  void commit(Result &result);
private:
  PrimaryTree *primaryTree;
  std::vector<RElement> RSet;
  std::vector<WElement> WSet;
  SiloEnv *env;

  TidWord maxWriteTid, maxReadTid;
  TidWord mostRecentlyChosenTid;

  void lockWSet();
  void unlockWSet();
  Record *searchWSet(Record *ptr);
};

}


#endif //TRANSACTIONSYSTEM_TRANSACTION_H
