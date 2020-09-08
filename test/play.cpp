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
  auto p = a();
  printf("%d", p[1]);
}
