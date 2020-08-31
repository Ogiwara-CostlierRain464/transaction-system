#include "logger.h"
#include <iostream>
#include <xmmintrin.h>

KVSilo::Logger::Logger(SiloEnv &env_)
: env(env_)
{
}

void KVSilo::Logger::run() {
  while(!env.start.load(std::memory_order_relaxed)){
    _mm_pause();
  }

  while(!env.stop.load(std::memory_order_relaxed)){
    _mm_pause();
  }
}
