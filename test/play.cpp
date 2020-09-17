#include <gtest/gtest.h>
#include <vector>
#include <atomic>
#include <thread>
#include <array>
#include <optional>
#include <functional>
#include <string>



using namespace std;

/**
 * Used to check C++ features.
 */
class Play: public ::testing::Test{
};


array<int,2> a(){
  array<int,2> p{1,2};

  return p;
}

TEST_F(Play, play) {
  std::vector<int> a = {1,2,3,4};
  std::vector<int> b = {1,2,3,4};
  EXPECT_EQ(a == b, true);
}
