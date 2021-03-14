#ifndef TRANSACTIONSYSTEM_RECORD_H
#define TRANSACTIONSYSTEM_RECORD_H

#include <cstdint>
#include <atomic>

namespace KVSilo{

struct TidWord{
  union {
    uint64_t body;
    struct {
      bool lock: 1;
      bool latest: 1;
      bool absent: 1;
      uint32_t tid: 29;
      uint32_t epoch: 32;
    };
  };

  TidWord()
  : epoch(0)
  , tid(0)
  , absent(false)
  , latest(true)
  , lock(false)
  {}

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

template <typename T>
struct is_atomic{
  static constexpr bool value =
    std::is_trivially_copyable<T>::value
    && std::is_copy_constructible<T>::value
    && std::is_copy_assignable<T>::value
    && std::is_move_assignable<T>::value;
};

static void aaa(){

}

struct Record{
  std::atomic<TidWord> tidWord;
  std::atomic_int value;

  explicit Record(int value_)
  : tidWord(TidWord())
  , value(value_){}
};



}

#endif //TRANSACTIONSYSTEM_RECORD_H
