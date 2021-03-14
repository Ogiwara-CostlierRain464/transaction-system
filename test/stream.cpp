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

class Stream: public ::testing::Test{};

TEST_F(Stream, aha) {
}