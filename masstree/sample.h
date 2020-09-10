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
  Version v; v.is_root = true;v.is_border = true;
  root->version.store(v);

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
  Version v; v.is_root = true;v.is_border = true;
  root->version.store(v);

  return tree;
}

Masstree *sample3(){
  /**
   * 論文中と同じサンプル
   * 0x0001020304050607 を共有し、
   * suffixは0x0A0B: 1と0x0C0D: 2
   */

// init
  auto tree = new Masstree;
  auto l1_root = new BorderNode;
  auto l2_root = new BorderNode;

// ↓
  tree->root = l1_root;

  l1_root->key_slice[0] = 0x0001020304050607;
  l1_root->key_len[0] = BorderNode::key_len_layer;
  l1_root->lv[0].next_layer = l2_root;
  Version v1; v1.is_root = true;v1.is_border = true;
  l1_root->version.store(v1);

  l2_root->key_slice[0] = 0x0A0B;
  l2_root->key_len[0] = 2;
  l2_root->lv[0].value = new int(1);
  l2_root->key_slice[1] = 0x0C0D;
  l2_root->key_len[1] = 2;
  l2_root->lv[1].value = new int(2);
  Version v2; v2.is_root = true;v2.is_border = true;
  l2_root->version.store(v2);

  return tree;
}

Masstree *sample4(){
  /**
   * Interior Nodeを使うサンプル
   *
   * {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150}
   * |   |
   * |   |
   * (9:18)
   *     |
   *     |
   *     (11:22)
   */
  auto tree = new Masstree;
  auto root = new InteriorNode;
  auto _9 = new BorderNode;
  auto _11 = new BorderNode;
  auto _160 = new BorderNode;

  tree->root = root;

  Version v; v.is_root = true;
  root->version.store(v);
  root->n_keys = 15;
  root->key_slice[0] = 0x0100;
  root->key_slice[1] = 0x0200;
  root->key_slice[2] = 0x0300;
  root->key_slice[3] = 0x0400;
  root->key_slice[4] = 0x0500;
  root->key_slice[5] = 0x0600;
  root->key_slice[6] = 0x0700;
  root->key_slice[7] = 0x0800;
  root->key_slice[8] = 0x0900;
  root->key_slice[9] = 0x010000;
  root->key_slice[10] = 0x010100;
  root->key_slice[11] = 0x010200;
  root->key_slice[12] = 0x010300;
  root->key_slice[13] = 0x010400;
  root->key_slice[14] = 0x010500;
  root->child[0] = _9;
  root->child[1] = _11;
  root->child[15] = _160;

  Version v9; v9.is_border = true;
  _9->version.store(v9);
  _9->key_len[0] = 1;
  _9->key_slice[0] = 0x09;
  _9->lv[0].value = new int(18);
  _9->parent = root;


  Version v11; v11.is_border = true;
  _11->version.store(v11);
  _11->key_len[0] = 2;
  _11->key_slice[0] = 0x0101;
  _11->lv[0].value = new int(22);
  _11->parent = root;

  Version v160; v160.is_border = true;
  _160->version.store(v160);
  _160->key_len[0] = 3;
  _160->key_slice[0] = 0x010600;
  _160->lv[0].value = new int(320);
  _160->parent = root;

  return tree;
}


#endif //TRANSACTIONSYSTEM_SAMPLE_H
