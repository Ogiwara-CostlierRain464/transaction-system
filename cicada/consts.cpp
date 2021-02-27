#include "consts.h"
#include "version.h"
#include "tuple.h"
#include "../common/result.h"

alignas(CACHE_LINE_SIZE) std::atomic<uint64_t> MinRts;

alignas(CACHE_LINE_SIZE) std::atomic<uint64_t> MinWts;
alignas(CACHE_LINE_SIZE) std::atomic<unsigned int> FirstAllocateTimestamp;

alignas(CACHE_LINE_SIZE) uint64_t_64byte *ThreadWtsArray;
alignas(CACHE_LINE_SIZE) uint64_t_64byte *ThreadRtsArray;
alignas(CACHE_LINE_SIZE) uint64_t_64byte *ThreadRtsArrayForGroup;
alignas(CACHE_LINE_SIZE) uint64_t_64byte *GroupCommitIndex;
alignas(CACHE_LINE_SIZE) uint64_t_64byte *GroupCommitCounter;

alignas(CACHE_LINE_SIZE) Version ***PLogSet;
alignas(CACHE_LINE_SIZE) Version **SLogSet;

alignas(CACHE_LINE_SIZE) uint64_t_64byte *GCFlag;
alignas(CACHE_LINE_SIZE) uint64_t_64byte *GCExecuteFlag;

alignas(CACHE_LINE_SIZE) Tuple *Table;
alignas(CACHE_LINE_SIZE) uint64_t InitialWts;

alignas(CACHE_LINE_SIZE) std::vector<Result> CicadaResult;