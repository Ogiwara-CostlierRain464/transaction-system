#ifndef TRANSACTIONSYSTEM_BPTREE_H
#define TRANSACTIONSYSTEM_BPTREE_H

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
next_layer:
  auto current = key.getCurrentSlice();
  if(1 <= current.size and current.size <= 7){

  }else{

  }

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

  }

  /**
   * Case 3: border nodeに空きがある場合
   */


  /**
   * Case 4: border nodeに空きが無い場合
   */
}

#endif //TRANSACTIONSYSTEM_BPTREE_H
