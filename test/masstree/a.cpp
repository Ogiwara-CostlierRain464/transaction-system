#include <gtest/gtest.h>
#include "../../masstree/masstree.h"
#include "../../masstree/verify.h"
#include "../../masstree/sample.h"

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

  auto n1 = reinterpret_cast<DummyNode *>(n.findChild(0));
  EXPECT_EQ(n1->value, 0);
}

TEST(MasstreeTest, findChild2){
  InteriorNode n;
  DummyNode dn0{0};
  DummyNode dn2{2};

  n.child[0] = &dn0;
  n.key_slice[0] = 1;
  n.child[1] = &dn2;
  n.key_slice[1] = 3;

  auto n1 = reinterpret_cast<DummyNode *>(n.findChild(2));
  EXPECT_EQ(n1->value, 2);
}

TEST(MasstreeTest, sample2){
  auto tree = sample2();
  auto b = tree->findBorder(0x01234567);
  EXPECT_EQ(*static_cast<int *>(b->lv->value), 1);
}

TEST(MasstreeTest, sample3){
  auto tree = sample3();
  auto b = tree->findBorder(0x01234567);
  EXPECT_EQ(b->key_len[0], BorderNode::nextLayerKeyLen);
}