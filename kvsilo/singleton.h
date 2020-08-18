#ifndef TRANSACTIONSYSTEM_SINGLETON_H
#define TRANSACTIONSYSTEM_SINGLETON_H

namespace KVSilo{

struct NonMovable{
  NonMovable() = default;
  NonMovable(NonMovable &&) = delete;
  NonMovable &operator=(NonMovable &&) = delete;
};

struct NonCopyable{
  NonCopyable() = default;
  NonCopyable(const NonCopyable&) = delete;
  NonCopyable &operator=(const NonCopyable&) = delete;
};

struct Singleton: public NonMovable, NonCopyable{};

}

#endif //TRANSACTIONSYSTEM_SINGLETON_H
