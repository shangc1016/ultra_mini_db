#include <gtest/gtest.h>

// 测试
int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  // filter based on the name of `test_suite_name`.
  ::testing::GTEST_FLAG(filter) = "database_test*";

  return RUN_ALL_TESTS();
}