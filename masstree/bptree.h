#ifndef TRANSACTIONSYSTEM_BPTREE_H
#define TRANSACTIONSYSTEM_BPTREE_H

#include <cassert>
#include "masstree.h"


Node *start_new_tree(Key key, void *value){
  /**
   * keyの長さの分だけ、layerを作る
   * ここの処理は、独立しそう
   *
   * key.get currentする
   *
   * next_layer:
   *
   * current key sliceが1~7のとき
   * key_lenに1~7を入れてkey_sliceに入れて、lv.valueに入れる
   *
   *
   * current key sliceが8のとき
   *
   * 次のkey sliceがある場合
   * とりあえずkey_sliceに入れ、key.nextをしcursorをすすめる
   * この状態で、さらに次のkey sliceがある場合
   * key_lenに255を入れていれて、lv.next_layerにいれ、next_layerにジャンプ
   *
   * さらに次のkey sliceがない場合
   * 残りをsuffixにいれ、lv.valueに入れる
   *
   * 次のkey sliceがない場合
   * key_lenに8を入れてkey_sliceに入れて、lv.valueに入れる
   *
   */
   auto root = new BorderNode;
   root->version.is_root = true;
   auto result = root;
next_layer:
  auto current = key.getCurrentSlice();
  if(1 <= current.size and current.size <= 7){
    root->key_len[0] = current.size;
    root->key_slice[0] = current.slice;
    root->lv[0].value = value;
  }else{ // size == 8
    assert(current.size == 8);
    if(key.hasNext()){
      root->key_slice[0] = current.slice;
      key.next();
      if(key.hasNext()){
        root->key_len[0] = BorderNode::key_len_layer;
        auto next = new BorderNode;
        next->version.is_root = true;
        root->lv[0].next_layer = next;
        root = next;
        goto next_layer;
      }else{
        root->key_len[0] = 8;
        root->key_suffixes.set(0, key.getCurrentSlice());
        root->lv[0].value = value;
      }
    }else{
      root->key_len[0] = 8;
      root->key_slice[0] = current.slice;
      root->lv[0].value = value;
    }
  }

  return result;
}

/**
 * Borderにkeyを挿入する
 */
void insert_into_border(BorderNode *border, Key key, void *value){
  // まず挿入するポイントを決める
  // 挿入ポイントまで、moving up
  // assign
  size_t insertion_point = 0;
  size_t num_keys = border->numberOfKeys();
  auto current = key.getCurrentSlice();

  while (insertion_point < num_keys
  && border->key_slice[insertion_point] < current.slice){
    ++insertion_point;
  }

  // moving up
  for(size_t i = num_keys; i > insertion_point; --i){
    border->key_len[i] = border->key_len[i - 1];
    border->key_slice[i] = border->key_slice[i - 1];
    border->lv[i] = border->lv[i - 1];
  }

  // next layerにうつす必要があるんだよなあ…
  // Masstree特有の動きとして、keyの長さの分だけlayerを付け足していく
  // ここは、start_new_treeとほぼ同じ動きになるであろう
  // いや、新しいlayerにおいては

  if(1 <= current.size and current.size <= 7){
    border->key_len[insertion_point] = current.size;
    border->key_slice[insertion_point] = current.slice;
    border->lv[insertion_point].value = value;
    return;
  }else{
    assert(current.size == 8);
    if(key.hasNext()){
      border->key_slice[insertion_point] = current.slice;
      key.hasNext();
      if(key.hasNext()){
        border->key_len[insertion_point] = BorderNode::key_len_layer;
        auto next = new BorderNode;
        next->version.is_root = true;
        border->lv[insertion_point].next_layer = next;
        border = next;
        goto next_layer;
      }else{
        border->key_len[insertion_point] = 8;
        border->key_suffixes.set(insertion_point, key.getCurrentSlice());
        border->lv[insertion_point].value = value;
        return;
      }
    }else{
      border->key_len[insertion_point] = 8;
      border->key_slice[insertion_point] = current.slice;
      border->lv[insertion_point].value = value;
      return;
    }
  }

next_layer:
  current = key.getCurrentSlice();
  if(1 <= current.size and current.size <= 7){
    border->key_len[0] = current.size;
    border->key_slice[0] = current.slice;
    border->lv[0].value = value;
  }else{ // size == 8
    assert(current.size == 8);
    if(key.hasNext()){
      border->key_slice[0] = current.slice;
      key.next();
      if(key.hasNext()){
        border->key_len[0] = BorderNode::key_len_layer;
        auto next = new BorderNode;
        next->version.is_root = true;
        border->lv[0].next_layer = next;
        border = next;
        goto next_layer;
      }else{
        border->key_len[0] = 8;
        border->key_suffixes.set(0, key.getCurrentSlice());
        border->lv[0].value = value;
      }
    }else{
      border->key_len[0] = 8;
      border->key_slice[0] = current.slice;
      border->lv[0].value = value;
    }
  }
}

Node *split(Node *n, Key k){
  assert(false && "NOT YET IMPLEMENTED");

  // precondition: n locked.
  assert(n->version.locked);
}

/**
 * NormalなB+ treeを参考にして作る
 * rootが更新される場合があるので、rootへのポインタを返す
 */
Node *insert(Node *root, Key key, void* value){

  /**
    * Case 1: Treeのrootが空の場合
    */
  if(root == nullptr){
    return start_new_tree(key, value);
  }

  /**
   * Case 2: すでにKeyが存在している場合
   */
  if(write(root, key, value)){
    return root;
  }

  auto border_v = findBorder(root, key);
  auto border = border_v.first;

  /**
   * Case 3: border nodeに空きがある場合
   */
   if(border->hasSpace()){
     insert_into_border(border, key, value);
     return root;
   }

  /**
   * Case 4: border nodeに空きが無い場合
   */
  return split(border, key);
}

#endif //TRANSACTIONSYSTEM_BPTREE_H
