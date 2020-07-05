#ifndef TRANSACTIONSYSTEM_OP_ELEMET_H
#define TRANSACTIONSYSTEM_OP_ELEMET_H

#include <cstdint>
#include "version.h"

template <typename T>
struct OpElement{
  uint64_t key;
  T *recordPtr;

  OpElement(): key(0), recordPtr(nullptr){}
  OpElement(uint64_t key_): key(key_){}
  OpElement(uint64_t key_, T *recordPtr_)
  : key(key_), recordPtr(recordPtr_){}
};

template <typename T>
struct ReadElement: public OpElement<T>{
  using OpElement<T>::OpElement;

  Version *laterVer, *ver;

  ReadElement(
    uint64_t key, T *recordPtr_,
    Version *laterVer_, Version *ver_)
    : OpElement<T>::OpElement(key, recordPtr_)
    , laterVer(laterVer_)
    , ver(ver_){}


  bool operator<(const ReadElement &rhs) const{
    return this->key < rhs.key;
  }
};

template <typename T>
struct WriteElement: public OpElement<T>{
  using OpElement<T>::OpElement;

  Version *laterVer, *newVer;
  bool RMW; // read-modify-write
  bool finishVersionInstall;

  WriteElement(
    uint64_t key, T *recordPtr,
    Version *laterVer_, Version *newVer_, bool RMW_)
    : OpElement<T>::OpElement(key, recordPtr)
    , laterVer(laterVer_)
    , newVer(newVer_)
    , RMW(RMW_)
    , finishVersionInstall(false)
    {}

  bool operator<(const WriteElement &rhs)const{
    return this->key < rhs.key;
  }
};

template <typename T>
struct GCElement: public OpElement<T>{
  using OpElement<T>::OpElement;

  Version *version;
  uint64_t wts;

  GCElement(): version(nullptr), wts(0){
    this->key = 0;
  }

  GCElement(
    uint64_t key, T *recordPtr,
    Version *ver_, uint64_t wts_)
    : OpElement<T>::OpElement(key, recordPtr)
    , version(ver_)
    , wts(wts_){}
};

#endif //TRANSACTIONSYSTEM_OP_ELEMET_H
