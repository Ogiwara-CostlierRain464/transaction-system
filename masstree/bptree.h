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
 * NormalなB+ treeを参考にして作る
 * rootが更新される場合があるので、rootへのポインタを返す
 */
Node *insert(Node *root, Key key, void* value){
  /**
   * Case 1: すでにKeyが存在している場合
   */
  if(write(root, key, value)){
    return root;
  }

  /**
   * Case 2: Treeのrootが空の場合
   */
  if(root == nullptr){
    return start_new_tree(key, value);
  }

  /**
   * Case 3: border nodeに空きがある場合
   */


  /**
   * Case 4: border nodeに空きが無い場合
   */
}

#endif //TRANSACTIONSYSTEM_BPTREE_H
