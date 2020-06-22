#ifndef TRANSACTIONSYSTEM_COMMON_H
#define TRANSACTIONSYSTEM_COMMON_H

#include <vector>

/**
 * 各種定数、共有変数をここにまとめる
 */
constexpr size_t CACHE_LINE_SIZE = 64;
constexpr size_t VALUE_SIZE = 4;
constexpr size_t THREAD_NUM = 100;
constexpr size_t MAX_OPERATIONS = 10;
constexpr size_t PAGE_SIZE = 4096;
constexpr size_t TUPLE_NUM = 1000000;
constexpr size_t ZIPF_SKEW = 0;
constexpr size_t RATIO = 50;
constexpr size_t EPOCH_TIME = 40;
constexpr size_t CLOCKS_PER_US = 2100;
constexpr size_t WAIT_SECONDS = 3;

struct uint64_t_64byte{
  alignas(CACHE_LINE_SIZE) uint64_t body;

  uint64_t_64byte() noexcept : body(0){}
  uint64_t_64byte(uint64_t init) noexcept : body(init){}
};

alignas(CACHE_LINE_SIZE) extern uint64_t_64byte GlobalEpoch;
alignas(CACHE_LINE_SIZE) extern uint64_t_64byte *ThreadLocalEpochs;
struct Tuple;
// The actual record data is stored in the same cache line
// as the record header, avoiding an additional memory fetch to
// access field values. (cf. P5 4.3)
alignas(CACHE_LINE_SIZE) extern Tuple *Table;

struct Result;
extern std::vector<Result> SiloResult;



//alignas(CACHE_LINE_SIZE) extern

#endif //TRANSACTIONSYSTEM_COMMON_H
