#include "rocksdb/perf_flag.h"

#include <gtest/gtest.h>

namespace rocksdb {
class PerfFlagTest : testing::Test {};
TEST(PerfFlagTest, TestEnableFlag) {
  for (int i = 0; i < 10; ++i) {
    EnablePerfFlag(i);
    ASSERT_EQ(CheckPerfFlag(i), true);
  }
}
TEST(PerfFlagTest, TestDisableFlag) {
  for (int i = 0; i < 10; ++i) {
    EnablePerfFlag(i);
  }
  for (int i = 0; i < 10; ++i) {
    DisablePerfFlag(i);
    ASSERT_EQ(CheckPerfFlag(i), false);
  }
}
}  // namespace rocksdb

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}