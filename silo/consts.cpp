#include "consts.h"
#include "../common/result.h"
#include "tuple.h"

alignas(CACHE_LINE_SIZE) uint64_t_64byte GlobalEpoch;
alignas(CACHE_LINE_SIZE) uint64_t_64byte *ThreadLocalEpochs;
alignas(CACHE_LINE_SIZE) Tuple *Table;
std::vector<Result> SiloResult;