#ifndef TRANSACTIONSYSTEM_SAMPLE_H
#define TRANSACTIONSYSTEM_SAMPLE_H

#include "masstree.h"
/**
 * まずは、手動でデータを用意する
 */

void sample1(){
  // the   : 1
  // their : 2
  // them  : 3
  // three : 4
  // を含むとする


  auto tree = new Masstree;

  auto t = new InteriorNode;
  t->key_slice[0] = 't';

  // どうやってchildがInteriorかBorderかはんていするか？
  // 全てのBorder nodeの高さ(rootからの距離)は同じだから、
  // それについてもLayerに保存しておく？

  auto th = new InteriorNode;
  th->key_slice[0] = 'h';

  auto the = new InteriorNode;
  the->key_slice[0] = 'e';

  auto thei = new InteriorNode;
  auto their = new InteriorNode;
  auto them = new InteriorNode;

  auto thr = new InteriorNode;
  thr->key_slice[0] = 'r';

  auto thre = new InteriorNode;
  auto three = new InteriorNode;


}

Masstree* sample2(){
  // 0x01234567AB: 1
  // を含む

  auto tree = new Masstree;

  auto _0 = new InteriorNode;
  auto _01 = new InteriorNode;
  auto _012 = new InteriorNode;
  auto _0123 = new InteriorNode;
  auto _01234 = new InteriorNode;
  auto _012345 = new InteriorNode;
  auto _0123456 = new InteriorNode;
  auto _01234567 = new InteriorNode;
  auto border = new BorderNode;

  _0->n_keys = 1;
  _0->key_slice[0] = 0x00000000;
  _01->n_keys = 1;
  _01->key_slice[0] = 0x01000000;
  _012->n_keys = 1;
  _012->key_slice[0] = 0x01200000;
  _0123->n_keys = 1;
  _0123->key_slice[0] = 0x01230000;
  _01234->n_keys = 1;
  _01234->key_slice[0] = 0x01234000;
  _012345->n_keys = 1;
  _012345->key_slice[0] = 0x01234500;
  _0123456->n_keys = 1;
  _0123456->key_slice[0] = 0x01234560;
  _01234567->n_keys = 1;
  _01234567->key_slice[0] = 0x01234567;

  border->key_len[0] = 2;
  border->key_slice[0] = 0x000000AB;
  border->lv[0].value = new int(1);
  border->version.is_border = true;

  tree->root = _0;
  _0->version.is_root = true;
  _0->child[0] = _01;
  _01->parent = _0;
  _01->child[0] = _012;
  _012->parent = _01;
  _012->child[0] = _0123;
  _0123->parent = _012;
  _0123->child[0] = _01234;
  _01234->parent = _0123;
  _01234->child[0] = _012345;
  _012345->parent = _01234;
  _012345->child[0] = _0123456;
  _0123456->parent = _012345;
  _0123456->child[0] = _01234567;
  _01234567->parent = _0123456;
  _01234567->child[0] = border;
  border->parent = _01234567;

  return tree;
}

#endif //TRANSACTIONSYSTEM_SAMPLE_H
