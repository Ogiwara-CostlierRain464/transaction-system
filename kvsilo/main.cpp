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
    int value = trn.read(5);
    int value2 = trn.read(6);
    printf("Before: 5 is %d, 6 is %d\n", value, value2);
  });

  std::this_thread::sleep_for(100ms);

  constexpr size_t num_threads = 30000;
  std::vector<std::thread> threads;
  threads.reserve(num_threads);
  for(size_t i = 0; i < num_threads; ++i){
    if(i % 2 == 0){
      threads.emplace_back([&db](){
        db.executeTransaction([](Transaction& trn){
          int value = trn.read(5);
          trn.write(5, value - 1);
          int value2 = trn.read(6);
          trn.write(6, value2 + 1);
        });
        std::this_thread::sleep_for(100ms);
      });
    }else{
      threads.emplace_back([&db](){
        db.executeTransaction([](Transaction& trn){
          int value = trn.read(6);
          trn.write(6, value - 1);
          int value2 = trn.read(5);
          trn.write(5, value2 + 1);
        });
        std::this_thread::sleep_for(100ms);

      });
    }
  }

  for(auto &t: threads){
    t.join();
  }

  std::this_thread::sleep_for(100ms);

  db.executeTransaction([](Transaction& trn){
    int value = trn.read(5);
    int value2 = trn.read(6);
    printf("After: 5 is %d, 6 is %d\n", value, value2);
  });

  std::this_thread::sleep_for(100ms);

  db.terminate();

  return 0;
}