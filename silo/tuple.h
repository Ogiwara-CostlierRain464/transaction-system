#ifndef TRANSACTIONSYSTEM_TUPLE_H
#define TRANSACTIONSYSTEM_TUPLE_H

#include <cstdint>
#include "common.h"

/**
 * TID wordを指す
 * @see https://en.cppreference.com/w/cpp/language/bit_field
 */
struct TidWord{
  union {
    uint64_t body;
    struct {
      // NOTE: is ordering correct?
      bool lock: 1{};
      bool latest: 1{};
      bool absent: 1{};
      uint64_t tid: 29{};
      uint64_t epoch: 32{};
    };
  };

  TidWord(): body(0){};

  bool operator==(const TidWord &rhs)const{
    return body == rhs.body;
  }
  bool operator!=(const TidWord &rhs)const{
    return !(*this == rhs);
  }

  bool operator<(const TidWord &rhs)const{
    return body < rhs.body;
  }
};

struct Tuple{
  alignas(CACHE_LINE_SIZE) TidWord tidWord;
  char value[VALUE_SIZE]{};
};

#endif //TRANSACTIONSYSTEM_TUPLE_H
