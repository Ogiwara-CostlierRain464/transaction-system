#include <gtest/gtest.h>
#include <vector>
#include <atomic>

using namespace std;

/**
 * Used to check C++ features.
 */
class Play: public ::testing::Test{};

void f(vector<int> &p){
  vector<int>q(p);

  q.push_back(42);
}

void g(vector<int> &&p){
  p.push_back(42);
}

TEST_F(Play, play) {
  atomic_int_least64_t k;
  atomic_flag h = ATOMIC_FLAG_INIT;

  auto t = vector<int>{1,2,3};
  auto &r = t;
  f(t);
  g(move(t));
  t.push_back(6);
}
