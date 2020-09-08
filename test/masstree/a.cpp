#include <gtest/gtest.h>
#include "../../masstree/masstree.h"
#include "../../masstree/verify.h"
#include "../../masstree/sample.h"

using namespace std;

class MasstreeTest: public ::testing::Test{
  MasstreeTest(){
    verify_struct_property();
  }
};

struct DummyNode: Node{
  int value;

  explicit DummyNode(int v)
  : value(v)
  {
//    version.v_split = 0x9999;
  }
};

TEST(MasstreeTest, findChild1){
  InteriorNode n;
  DummyNode dn{0};

  n.child[0] = &dn;
  n.n_keys = 1;
  n.key_slice[0] = 0x01;

  auto n1 = reinterpret_cast<DummyNode *>(n.findChild(pair(0x00, 1)));
  EXPECT_EQ(n1->value, 0);
}

TEST(MasstreeTest, findChild2){
  /**
   * || 1 || 3 ||
   */

  InteriorNode n;
  DummyNode dn0{0};
  DummyNode dn2{2};

  n.n_keys = 2;
  n.child[0] = &dn0;
  n.key_slice[0] = 0x01;
  n.child[1] = &dn2;
  n.key_slice[1] = 0x03;

  auto n1 = reinterpret_cast<DummyNode *>(n.findChild(pair(0x02,1)));
  EXPECT_EQ(n1->value, 2);
}

TEST(MasstreeTest, sample2){
  auto tree = sample2();
  array<KeySlice, 4> slices = {pair(0x0001020304050607, 8)};
  Key key(slices, 1);

  auto b = tree->findBorder(key);
  EXPECT_EQ(*static_cast<int *>(b.first->lv[0].value), 1);
}

TEST(MasstreeTest, sample3){
  auto tree = sample3();
  array<KeySlice, 4> slices = {pair(0x0001020304050607, 8), pair(0x0A0B, 2)};
  Key key(slices, 1);

  auto b = tree->findBorder(key);
  EXPECT_EQ(b.first->key_len[0], BorderNode::key_len_layer);
}