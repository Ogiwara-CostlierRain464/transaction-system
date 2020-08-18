#include <iostream>
#include <cmath>
#include <chrono>
#include "primary_tree.h"
#include "database.h"
#include <cassert>

using namespace KVSilo;
using namespace std::literals::chrono_literals;


int main(){
  Database db;

  db.executeTransaction([](Transaction& trn){
    int value = trn.read(1);
    trn.write(2, value + 3);
  });

  std::this_thread::sleep_for(1ms);

  db.executeTransaction([](Transaction& trn){
    assert(trn.read(2) == 4);
  });

  std::this_thread::sleep_for(1ms);

  db.terminate();

  return 0;
}