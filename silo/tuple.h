#ifndef TRANSACTIONSYSTEM_TUPLE_H
#define TRANSACTIONSYSTEM_TUPLE_H

#include <cstdint>
#include "consts.h"

/**
 * TID wordを指す
 * @see https://en.cppreference.com/w/cpp/language/bit_field
 */
struct TidWord{
  /**
   * 議論: 型のパンニングによる未定義動作を踏んでいないか？
   *
   * localでは常にstructがActive memberになるように
   * globalでstoreするときにはuint64_tからの引き出し
   *
   * globalから読み込むときにuint64_tがActiveになって、そこから
   * structが非active memberからの読み込み経由でActiveになって、
   * 手元で整形するときにはstructがActiveになって問題なし？
   *
   *
   */
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

/**
 * Tuple represents just one record.
 * Use key to access record via Table.
 */
struct Tuple{
  alignas(CACHE_LINE_SIZE) TidWord tidWord;
  char value[VALUE_SIZE]{};
};

#endif //TRANSACTIONSYSTEM_TUPLE_H
