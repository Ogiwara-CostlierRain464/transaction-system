#ifndef TRANSACTIONSYSTEM_SILO_ENV_H
#define TRANSACTIONSYSTEM_SILO_ENV_H

#include <cstdint>
#include <atomic>
#include <vector>
#include <memory>

namespace KVSilo{

using Epoch = uint32_t;

struct SiloEnv{
  std::atomic<Epoch> E;
  std::vector<std::unique_ptr<std::atomic<Epoch>>> workerE; // you should align to avoid cache line contention
  std::atomic_bool start = false;
  std::atomic_bool stop = false;
};

}

#endif //TRANSACTIONSYSTEM_SILO_ENV_H
