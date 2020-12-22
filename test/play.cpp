#include <gtest/gtest.h>
#include <vector>
#include <atomic>
#include <thread>
#include <array>
#include <optional>
#include <functional>
#include <string>
#include <boost/lockfree/stack.hpp>



using namespace std;

/**
 * Used to check C++ features.
 */
class Play: public ::testing::Test{
};


array<int,2> a(){


}

TEST_F(Play, play) {
  boost::lockfree::stack<int> l(20);

  l.push(3);
  l.push(4);

  int i;
  l.pop(i);
  EXPECT_EQ(i, 4);
}
