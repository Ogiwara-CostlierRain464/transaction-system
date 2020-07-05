#ifndef TRANSACTIONSYSTEM_COMMON_H
#define TRANSACTIONSYSTEM_COMMON_H

#include <vector>

constexpr size_t CACHE_LINE_SIZE = 64;
constexpr size_t VALUE_SIZE = 4;
constexpr size_t PAGE_SIZE = 4096;
constexpr size_t SPIN_WAIT_TIMEOUT_US = 2;
constexpr size_t THREAD_NUM = 2;
constexpr size_t MAX_OPERATIONS = 10;
constexpr size_t GROUP_COMMIT = 0;
constexpr size_t TUPLE_NUM = 1000;


struct uint64_t_64byte{
  alignas(CACHE_LINE_SIZE) uint64_t body;

  uint64_t_64byte() noexcept : body(0){}
  uint64_t_64byte(uint64_t init) noexcept : body(init){}
};

alignas(CACHE_LINE_SIZE) extern uint64_t_64byte *ThreadWtsArray;
alignas(CACHE_LINE_SIZE) extern uint64_t_64byte *ThreadRtsArray;
alignas(CACHE_LINE_SIZE) extern uint64_t_64byte *ThreadRtsArrayForGroup;
alignas(CACHE_LINE_SIZE) extern uint64_t_64byte *GroupCommitIndex;
alignas(CACHE_LINE_SIZE) extern uint64_t_64byte *GroupCommitCounter;

struct Version;
alignas(CACHE_LINE_SIZE) extern Version ***PLogSet;
alignas(CACHE_LINE_SIZE) extern Version **SLogSet;

struct RWLock;
extern RWLock SwalLock; //?
extern RWLock CtrlLock; //?

alignas(CACHE_LINE_SIZE) extern uint64_t_64byte *GCFlag;
alignas(CACHE_LINE_SIZE) extern uint64_t_64byte *GCExecuteFlag;

struct Tuple;
alignas(CACHE_LINE_SIZE) extern Tuple *Table;
alignas(CACHE_LINE_SIZE) extern uint64_t InitialWts;


#endif //TRANSACTIONSYSTEM_COMMON_H
