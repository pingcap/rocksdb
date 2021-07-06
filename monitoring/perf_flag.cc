#include "rocksdb/perf_flag.h"

namespace rocksdb {
#ifdef ROCKSDB_SUPPORT_THREAD_LOCAL
__thread uint8_t perf_flags[FLAGS_LEN];
#else
uint8_t perf_flags[FLAGS_LEN];
#endif

void EnablePerfFlag(uint64_t flag) {
  if (CheckPerfFlag(flag)) {
  } else {
    GET_FLAG(flag) ^= (uint64_t)0b1 << ((uint64_t)flag & (uint64_t)0b111);
  }
}

void DisablePerfFlag(uint64_t flag) {
  if (CheckPerfFlag(flag)) {
    GET_FLAG(flag) ^= (uint64_t)0b1 << ((uint64_t)flag & (uint64_t)0b111);
  } else {
  }
}

bool CheckPerfFlag(uint64_t flag) {
  return ((uint64_t)GET_FLAG(flag) & (uint64_t)0b1
                                         << (flag & (uint64_t)0b111)) == 0;
}

}  // namespace rocksdb