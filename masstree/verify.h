#ifndef TRANSACTIONSYSTEM_VERIFY_H
#define TRANSACTIONSYSTEM_VERIFY_H

#include "masstree.h"
#include <string>
#include <atomic>

template <typename T>
struct can_atomic{
  static constexpr bool value =
    std::is_trivially_copyable<T>::value
    && std::is_copy_constructible<T>::value
    && std::is_move_constructible<T>::value
    && std::is_copy_assignable<T>::value
    && std::is_move_assignable<T>::value;
};

void verify_struct_property(){

  // check struct/union size.
  static_assert(sizeof(Version) == 4);
  static_assert(sizeof(Permutation) == 8);
  static_assert(sizeof(LinkOrValue) == 8);

  // check atomic.
  static_assert(can_atomic<Version>::value);

}

std::string ver_string(int a,int b, int c){
  std::string s;
  s += (std::to_string(a) + "." + std::to_string(b) + "." + std::to_string(c));
  return s;
}

void check(){
  const char *true_cxx =
#ifdef __clang__
    "clang";
#else
    "g++";
#endif

  std::string true_cxx_ver =
#ifdef __clang__
    "clang";
#else
    ver_string(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#endif

  printf("Compiler: %s, %s\n", true_cxx, true_cxx_ver.c_str());
}

#endif //TRANSACTIONSYSTEM_VERIFY_H
