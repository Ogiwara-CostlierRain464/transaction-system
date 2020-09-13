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

  // 物理thread数だけクライアントを立ち上げる

  auto num = std::thread::hardware_concurrency();
  std::vector<std::thread> threads;
  threads.reserve(num);

  for(size_t i = 0; i < num / 2; ++i){
    threads.emplace_back([&db](){
      for(size_t j = 0; j < 100000; ++j){
        db.executeTransaction([](Transaction& trn){
          int value = trn.read(5);
          trn.write(5, value - 1);
          int value2 = trn.read(6);
          trn.write(6, value2 + 1);
        });
      }
      std::this_thread::sleep_for(200ms);
    });
  }
  for(size_t i = num / 2; i < num; ++i){
    threads.emplace_back([&db](){
      for(size_t j = 0; j < 100000; ++j){
        db.executeTransaction([](Transaction& trn){
          int value = trn.read(6);
          trn.write(6, value - 1);
          int value2 = trn.read(5);
          trn.write(5, value2 + 1);
        });
      }
      std::this_thread::sleep_for(200ms);
    });
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