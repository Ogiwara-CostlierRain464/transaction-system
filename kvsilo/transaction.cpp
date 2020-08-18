#include "transaction.h"

KVSilo::Transaction::Transaction(PrimaryTree *tree)
: primaryTree(tree)
{}


KVSilo::Transaction::Value KVSilo::Transaction::read(Key key) {
  return primaryTree->read(key);
}

void KVSilo::Transaction::write(Key key, Value value) {
  primaryTree->write(key, value);
}

void KVSilo::Transaction::commit() {

}
