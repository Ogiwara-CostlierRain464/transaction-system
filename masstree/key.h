#ifndef TRANSACTIONSYSTEM_KEY_H
#define TRANSACTIONSYSTEM_KEY_H

#include <cstdint>
#include <cassert>
#include <vector>

/**
 * Keyのスライスを表す。
 * 例えば、スライス0x0A0Bなら、
 * \code{.cpp}
 *  KeySlice(0x0A0B, 2)
 * \endcode
 *
 * で表す。
 *
 * スライスの長さは1byte~8byte。
 */
struct KeySlice{
  uint64_t slice = 0;
  /* 1~8 */
  uint8_t size = 0;

  KeySlice() = default;

  explicit KeySlice(uint64_t slice_, uint8_t size_)
    : slice(slice_)
    , size(size_)
  {
    assert(1 <= size and size <= 8);
  }

  bool operator==(const KeySlice &rhs) const{
    return slice == rhs.slice and size == rhs.size;
  }

  bool operator!=(const KeySlice &rhs) const{
    return !(*this == rhs);
  }
};

/**
 * 任意byte長のKeyを表す。
 * KeySliceのlistと、listのサイズで管理する
 */
struct Key{
  // NOTE: 性能改善の余地
  std::vector<KeySlice> body;
  size_t size;
  size_t cursor = 0;

  Key(KeySlice slice): Key({slice}, 1)
  {}

  Key(std::vector<KeySlice> body_ , size_t size_)
    : body(body_), size(size_)
  {}

  bool hasNext() {
    if (size == cursor + 1)
      return false;

    return true;
  }

  KeySlice getCurrentSlice(){
    return body[cursor];
  }

  KeySlice next(){
    assert(hasNext());
    ++cursor;
    return body[cursor];
  }

  bool operator==(const Key &rhs){
    if(size != rhs.size){
      return false;
    }
    for(size_t i = 0; i < size; ++i){
      if(body[i] != rhs.body[i]){
        return false;
      }
    }
    return true;
  }

  bool operator!=(const Key &rhs){
    return !(*this == rhs);
  }
};

#endif //TRANSACTIONSYSTEM_KEY_H
