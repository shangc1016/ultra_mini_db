#include <gtest/gtest.h>

// 测试
int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  // filter spec: `<test_suite_name>.<test_name>`
  // ::testing::GTEST_FLAG(filter) = "database_test.put_get_loops1";

  return RUN_ALL_TESTS();
}