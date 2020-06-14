#include <gtest/gtest.h>

using namespace std;

/**
 * Used to check C++ features.
 */
class Features: public ::testing::Test{};

template <typename T>
bool typeCheck ( T x )
{
  auto r=ref(x);
  cout<<boolalpha;
  // should return false, so ref(int) ≠ int&
  return is_same<T&,decltype(r)>::value;
}

TEST_F(Features, std_ref){
  int x = 5;
  EXPECT_EQ(typeCheck(x), false);
}