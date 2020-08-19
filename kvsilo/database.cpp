#include <thread>
#include <random>
#include <chrono>
#include "database.h"
#include "tsc.h"

using std::literals::chrono_literals::operator""ms;

KVSilo::Database::Database()
  : leaderThread(std::thread(&Database::leaderWork, this))
  {
  for(size_t i = 0; i < 1; ++i){
    auto logger = new Logger(env);
    loggers.emplace_back(logger);

    loggerThreads.emplace_back([logger](){
      logger->run();
    });
  }

  size_t threads_num = std::thread::hardware_concurrency();

  workerThreads.reserve(threads_num);
  workers.reserve(threads_num);
  for(size_t i = 0; i < threads_num; ++i){
    auto *worker = new Worker(i, loggers[0], &primaryTree, &env);
    workers.emplace_back(worker);
    env.workerE.push_back(std::make_unique<std::atomic<Epoch>>(0));

    workerThreads.emplace_back([worker](){
      worker->run();
    });
  }

  env.start.store(true, std::memory_order_relaxed);
}

void KVSilo::Database::executeTransaction(const Query &query) {
  //workerになげて、あとは完了するまでyield waitする
  thread_local static std::mt19937 random(0xDEADBEEF);
  // select worker randomly.
  auto worker = workers[random() % workers.size()];
  worker->addQueryToQueue(query);

  // 待ち操作はとりあえずあとで！
}

void KVSilo::Database::leaderWork() {
  auto start = std::chrono::high_resolution_clock::now();

  while(!env.stop.load(std::memory_order_relaxed)){
    std::this_thread::sleep_for(1ms);

    auto check = std::chrono::high_resolution_clock::now();

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(check - start).count();

    if(ms < 40)
      continue;

    Epoch E = env.E.load(std::memory_order_relaxed);

    for(auto &ew: env.workerE){
      if(ew->load(std::memory_order_relaxed) != E){
        continue;
      }
    }

    env.E.fetch_add(1);
    //printf("Epoch update.\n");
    start = check;
  }
}


void KVSilo::Database::terminate() {
  env.stop.store(true, std::memory_order_relaxed);

  leaderThread.join();

  for(auto &t: workerThreads){
    t.join();
  }
  for(auto &w: workers){
    delete w;
  }

  for(auto &t: loggerThreads){
    t.join();
  }
  for(auto &l: loggers){
    delete l;
  }
}

