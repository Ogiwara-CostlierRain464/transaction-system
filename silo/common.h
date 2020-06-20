#ifndef TRANSACTIONSYSTEM_COMMON_H
#define TRANSACTIONSYSTEM_COMMON_H


/**
 * 各種定数、共有変数をここにまとめる
 */
extern constexpr size_t CACHE_LINE_SIZE = 64;
constexpr size_t VALUE_SIZE = 4;
constexpr size_t THREAD_NUM = 4;
constexpr size_t MAX_OPERATIONS = 10;
constexpr size_t PAGE_SIZE = 4096;
constexpr size_t TUPLE_NUM = 1000000;
constexpr size_t ZIPF_SKEW = 0;
constexpr size_t RATIO = 50;
constexpr size_t EPOCH_TIME = 40;
constexpr size_t CLOCKS_PER_US = 2100;


struct uint64_t_64byte{
  alignas(CACHE_LINE_SIZE) uint64_t body;

  uint64_t_64byte(): body(0){}
  uint64_t_64byte(uint64_t init): body(init){}
};

alignas(CACHE_LINE_SIZE) uint64_t_64byte GlobalEpoch;
alignas(CACHE_LINE_SIZE) uint64_t_64byte *ThreadLocalEpochs;
alignas(CACHE_LINE_SIZE) uint64_t_64byte *CTIDW;
struct Tuple;
alignas(CACHE_LINE_SIZE) Tuple *Table;

struct Result;
std::vector<Result> SiloResult;



//alignas(CACHE_LINE_SIZE) extern

#endif //TRANSACTIONSYSTEM_COMMON_H
