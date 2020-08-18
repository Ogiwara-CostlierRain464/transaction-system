#ifndef TRANSACTIONSYSTEM_RECORD_H
#define TRANSACTIONSYSTEM_RECORD_H

#include <cstdint>
#include <atomic>

namespace KVSilo{

struct TidWord{
  union {
    uint64_t body;
    struct {
      bool lock: 1{};
      bool latest: 1{};
      bool absent: 1{};
      uint64_t tid: 29{};
      uint64_t epoch: 32{};
    };
  };

  TidWord(): body(0){}

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

struct Record{
  std::atomic<TidWord> tidWord;
  std::atomic_int value;

  explicit Record(int value_)
  : tidWord()
  , value(value_){}
};



}

#endif //TRANSACTIONSYSTEM_RECORD_H
