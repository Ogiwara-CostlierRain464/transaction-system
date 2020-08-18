#ifndef TRANSACTIONSYSTEM_SILO_ENV_H
#define TRANSACTIONSYSTEM_SILO_ENV_H

#include <cstdint>
#include <atomic>
#include <vector>

namespace KVSilo{

using Epoch = uint32_t;

struct SiloEnv{
  std::atomic<Epoch> E;
  std::vector<std::unique_ptr<std::atomic<Epoch>>> workerE;
  std::atomic_bool start = false;
  std::atomic_bool stop = false;
};

}

#endif //TRANSACTIONSYSTEM_SILO_ENV_H
