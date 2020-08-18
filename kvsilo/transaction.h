#ifndef TRANSACTIONSYSTEM_TRANSACTION_H
#define TRANSACTIONSYSTEM_TRANSACTION_H

#include <cstddef>
#include "primary_tree.h"

namespace KVSilo{

struct Operation{
  Key key;
};


class Transaction{
public:
  using Key = size_t;
  using Value = int;

  explicit Transaction(PrimaryTree *tree);

  Value read(Key);
  void write(Key key, Value value);
  void commit();
private:
  PrimaryTree *primaryTree;

};

}


#endif //TRANSACTIONSYSTEM_TRANSACTION_H