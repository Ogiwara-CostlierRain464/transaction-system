#ifndef TRANSACTIONSYSTEM_OP_ELEMET_H
#define TRANSACTIONSYSTEM_OP_ELEMET_H

#include <cstdint>
#include <cstring>
#include "tuple.h"

template <typename T>
struct OpElement{
  uint64_t key;
  T *recordPtr;

  OpElement(): key(0), recordPtr(nullptr){}
  OpElement(uint64_t key): key(key){}
  OpElement(uint64_t key, T* recordPtr)
  : key(key), recordPtr(recordPtr){}
};

template <typename T>
struct ReadElement: public OpElement<T>{
  using OpElement<T>::OpElement;

  TidWord tidWord;
  char value[VALUE_SIZE]{};

  ReadElement(
    uint64_t key,
    T* recordPtr,
    char *value_,
    TidWord tidWord_)
  : OpElement<T>::OpElement(key, recordPtr)
  {
    tidWord.body = tidWord_.body;
    memcpy(value, value_, VALUE_SIZE);
  }

  bool operator<(const ReadElement &rhs)const{
    return this->key < rhs.key;
  }
};

template <typename T>
class WriteElement: public OpElement<T>{
public:
  using OpElement<T>::OpElement;

  WriteElement(uint64_t key, T *recordPtr)
    : OpElement<T>::OpElement(key, recordPtr)
  {}

  bool operator<(const WriteElement &rhs)const{
    return this->key < rhs.key;
  }
};

#endif //TRANSACTIONSYSTEM_OP_ELEMET_H
