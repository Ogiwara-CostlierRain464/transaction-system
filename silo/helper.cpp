#include "helper.h"
#include "atomic_wrapper.h"
#include <thread>
#include <chrono>
#include <xmmintrin.h>


void sleepMs(int ms){
  std::this_thread::sleep_for(
    std::chrono::milliseconds(ms)
    );
}

bool isReady(const std::vector<char> &readyFlags){
  for(auto &flag: readyFlags){
    if(!loadAcquire(flag)) return false;
  }
  return true;
}

void waitForReady(const std::vector<char> &readyFlags){
  while(!isReady(readyFlags)){
    _mm_pause();
  }
}