#ifndef TRANSACTIONSYSTEM_VERSION_H
#define TRANSACTIONSYSTEM_VERSION_H

#include <cstdint>

/**
 * Version構造体を表す。
 * 4byteに収まるように、論文中のメモのように
 * オリジナルのものから変更してある。
 */
struct Version{

  /**
   * 論文中の"locked"に対応する。
   */
  static constexpr uint32_t lock = 0b1000'0000'0000'0000'0000'0000'0000'0000;

  union {
    uint32_t body;
    struct {
      uint16_t v_split: 16;
      uint8_t v_insert: 8;
      bool : 2;
      bool is_border: 1;
      bool is_root: 1;
      bool deleted: 1;
      bool splitting: 1;
      bool inserting: 1;
      bool locked: 1;
    };
  };

  Version()
    : locked{false}
    , inserting{false}
    , splitting{false}
    , deleted{false}
    , is_root{false}
    , is_border{false}
    , v_insert{0}
    , v_split{0}
  {}

  uint32_t operator ^(const Version &rhs) const{
    return (body ^ rhs.body);
  }
};


#endif //TRANSACTIONSYSTEM_VERSION_H
