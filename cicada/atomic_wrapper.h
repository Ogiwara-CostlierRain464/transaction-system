#ifndef TRANSACTIONSYSTEM_ATOMIC_WRAPPER_H
#define TRANSACTIONSYSTEM_ATOMIC_WRAPPER_H

#include <cstdint>
#include "common.h"

/**
 * Wrappers of atomic built-in.
 *
 * @see https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html
 * @see https://yohhoy.hatenablog.jp/entry/2014/12/21/171035
 * @see http://web.mit.edu/6.173/www/currentsemester/readings/R06-scalable-synchronization-1991.pdf
 */

template <typename T>
T load(T &ptr){
  return __atomic_load_n(&ptr, __ATOMIC_RELAXED);
}

template <typename T>
T loadAcquire(T& ptr) {
  return __atomic_load_n(&ptr, __ATOMIC_ACQUIRE);
}

template <typename T, typename T2>
void store(T& ptr, T2 val) {
  __atomic_store_n(&ptr, (T)val, __ATOMIC_RELAXED);
}

template <typename T, typename T2>
void storeRelease(T& ptr, T2 val) {
  __atomic_store_n(&ptr, (T)val, __ATOMIC_RELEASE);
}

template <typename T, typename T2>
bool compareExchange(T& m, T& before, T2 after) {
  return __atomic_compare_exchange_n(
    &m, &before, (T)after, false,
    __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE);
}

#endif //TRANSACTIONSYSTEM_ATOMIC_WRAPPER_H
