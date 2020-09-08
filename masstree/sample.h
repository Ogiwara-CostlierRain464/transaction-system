#ifndef TRANSACTIONSYSTEM_SAMPLE_H
#define TRANSACTIONSYSTEM_SAMPLE_H

#include "masstree.h"
/**
 * まずは、手動でデータを用意する
 */

Masstree *sample1(){
  // まずはLayer 0はただのB+ treeであるという事を示す
  // 0x 2pairで1byte   8byteには16pair
  // 0x0102030405060708 : 2
  // 0x01020304 : 1

// init node
  auto tree = new Masstree;
  auto root = new BorderNode;

// ↓
  tree->root = root;

  root->key_slice[0] = 0x01020304;
  root->key_len[0] = 4;
  root->lv[0].value = new int(1);
  root->key_slice[1] = 0x0102030405060708;
  root->key_len[1] = 8;
  root->lv[1].value = new int(2);
  root->version.is_root = true;
  root->version.is_border = true;

  return tree;
}

/**
 * @deprecated not valid sample tree.
 * @return
 */
Masstree* sample2(){
  /**
   * 0x00010203040506070A0B: 1 を含む
   */

// init node
  auto tree = new Masstree;
  auto root = new BorderNode;

// ↓
  tree->root = root;

  root->key_slice[0] = 0x0001020304050607;
  root->key_len[0] = 8;
  root->key_suffixes.set(0,KeySlice(0x0A0B, 2));
  root->lv[0].value = new int(1);
  root->version.is_root = true;
  root->version.is_border = true;

  return tree;
}

Masstree *sample3(){
  /**
   * 論文中と同じサンプル
   * 0x0001020304050607 を共有し、
   * suffixは0x0A0B: 1と0x0C0D: 2
   */

  auto tree = new Masstree;
  auto l1_root = new BorderNode;
  auto l2_root = new BorderNode;

  tree->root = l1_root;

  l1_root->key_slice[0] = 0x0001020304050607;
  l1_root->key_len[0] = BorderNode::key_len_layer;
  l1_root->lv[0].next_layer = l2_root;
  l1_root->version.is_root = true;
  l1_root->version.is_border = true;

  l2_root->key_slice[0] = 0x0A0B;
  l2_root->key_len[0] = 2;
  l2_root->lv[0].value = new int(1);
  l2_root->key_slice[1] = 0x0C0D;
  l2_root->key_len[1] = 2;
  l2_root->lv[1].value = new int(2);
  l2_root->version.is_root = true;
  l2_root->version.is_border = true;

  return tree;
}


#endif //TRANSACTIONSYSTEM_SAMPLE_H
