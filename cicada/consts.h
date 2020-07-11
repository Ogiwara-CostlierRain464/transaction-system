#ifndef TRANSACTIONSYSTEM_CONSTS_H
#define TRANSACTIONSYSTEM_CONSTS_H

#include <vector>
#include <atomic>

constexpr size_t CACHE_LINE_SIZE = 64;
constexpr size_t VALUE_SIZE = 4;
constexpr size_t PAGE_SIZE = 4096;
constexpr size_t SPIN_WAIT_TIMEOUT_US = 2;
constexpr size_t THREAD_NUM = 2;
constexpr size_t MAX_OPERATIONS = 10;
constexpr size_t GROUP_COMMIT = 0;
constexpr size_t TUPLE_NUM = 1000;
constexpr size_t EX_TIME = 3;
constexpr size_t PRE_RESERVE_VERSION = 10000;
constexpr size_t ZIPF_SKEW = 0;
constexpr size_t CLOCKS_PER_US = 2100;
constexpr size_t READ_RATIO = 70;

struct uint64_t_64byte{
  alignas(CACHE_LINE_SIZE) uint64_t body;

  uint64_t_64byte() noexcept : body(0){}
  uint64_t_64byte(uint64_t init) noexcept : body(init){}
};

/**
 * """
 * min_rts is also calculated similarly to (thread.rts),
 * and is used for safe garbage collection.
 *
 * cf. §3.1
 * """
 */
alignas(CACHE_LINE_SIZE) extern std::atomic<uint64_t> MinRts;
/**
 * """
 * ...where min_wts is the minimum of (thread.wts) for
 * all threads, updated by a leader thread periodically (§3.8).
 *
 * cf. §3.1
 * """
 */
alignas(CACHE_LINE_SIZE) extern std::atomic<uint64_t> MinWts;
alignas(CACHE_LINE_SIZE) extern std::atomic<unsigned int> FirstAllocateTimestamp;

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

struct Result;
alignas(CACHE_LINE_SIZE) std::vector<Result> CicadaResult;



#endif //TRANSACTIONSYSTEM_CONSTS_H
