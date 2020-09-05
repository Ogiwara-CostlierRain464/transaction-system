#include <gtest/gtest.h>

class Unit: public ::testing::Test{};

TEST_F(Unit, init){
EXPECT_EQ(1+1,2);
}

int main(int argc, char **argv){

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}