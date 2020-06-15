#include <gtest/gtest.h>

using namespace std;

/**
 * Used to check C++ features.
 */
class Features: public ::testing::Test{};

template <typename T>
bool typeCheck ( T x )
{
  reference_wrapper<int> r=ref(x);
  // should return false, so ref(int) ≠ int&
  return is_same<T&,decltype(r)>::value;
}

/**
 * A reference_wrapper object can be used to create an array of references
 * which was not possible with T&.
 *
 * @see https://stackoverflow.com/questions/33240993/c-difference-between-stdreft-and-t/33241552
 */
TEST_F(Features, std_ref){
  int x = 5;
  EXPECT_EQ(typeCheck(x), false);

  int x_ = 5, y = 7, z = 8;
  //int& ar[] = {x,y,z}; NOT Possible
  reference_wrapper<int> arr[] = {x, y, z};
}

TEST_F(Features, asm_ext_basic){
  int src = 1;
  int dst;
  asm("mov %1, %0\n\t"
      "add $1, %0"
      : "=r" (dst)  // output operands
      : "r" (src)); // input operands

  EXPECT_EQ(dst, 2);
}

TEST_F(Features, asm_volatile){
  uint64_t msr;
  asm volatile (
    "rdtsc\n\t"
    "shl $32, %%rdx\n\t"
    "or %%rdx, %0"
    : "=a" (msr)
    :
    : "rdx"
    );

  printf("%lld\n", msr);

  asm volatile (
  "rdtsc\n\t"
  "shl $32, %%rdx\n\t"
  "or %%rdx, %0"
  : "=a" (msr)
  :
  : "rdx"
  );

  printf("%lld\n", msr);
}

