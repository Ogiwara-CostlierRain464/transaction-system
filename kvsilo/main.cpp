#include <iostream>
#include <cmath>
#include <chrono>
#include "primary_tree.h"
#include "database.h"
#include <cassert>

using namespace KVSilo;

int main(){
  Database db;

  db.executeTransaction([](Transaction& trn){
    int value = trn.read(1);
    trn.write(2, value + 3);
  });

  std::this_thread::sleep_for(std::chrono::seconds(1));

  db.executeTransaction([](Transaction& trn){
    printf("%d\n", trn.read(2));
    //assert(trn.read(2) == 4);
  });

  std::this_thread::sleep_for(std::chrono::seconds(3));

  db.terminate();

  return 0;
}