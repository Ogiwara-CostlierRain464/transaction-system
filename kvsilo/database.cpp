#include <thread>
#include <random>
#include "database.h"

KVSilo::Database::Database() {
  for(size_t i = 0; i < 1; ++i){
    auto logger = new Logger;
    loggers.emplace_back(logger);

    loggerThreads.emplace_back([logger](){
      logger->run();
    });
  }

  size_t threads_num = std::thread::hardware_concurrency();

  workerThreads.reserve(threads_num);
  workers.reserve(threads_num);
  for(size_t i = 0; i < threads_num; ++i){
    auto *worker = new Worker(i, loggers[0], &primaryTree);
    workers.emplace_back(worker);

    workerThreads.emplace_back([worker](){
      worker->run();
    });
  }

}

void KVSilo::Database::executeTransaction(const Query &query) {
  //workerになげて、あとは完了するまでyield waitする
  thread_local static std::mt19937 random(0xDEADBEEF);
  // select worker randomly.
  auto worker = workers[random() % workers.size()];
  worker->addQueryToQueue(query);

  // 待ち操作はとりあえずあとで！
}

void KVSilo::Database::terminate() {

}
