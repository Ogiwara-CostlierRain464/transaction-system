#ifndef TRANSACTIONSYSTEM_BPTREE_H
#define TRANSACTIONSYSTEM_BPTREE_H

#include <cassert>
#include "masstree.h"

/**
 * KeyのBorderNodeへのinsert時に、keyの長さによっては
 * new layerを作る必要がある。
 *
 * BorderNodeにkeyのsliceをとりあえず入れ、new layerを
 * 作る必要がある場合には、new layerのrootとなるBorderNode
 * を作り、このprocedureを呼び出す。
 *
 */
void keyInsertAfterNewLayer(BorderNode *root, Key k, void *value){
  // Rootとなるborderを渡す必要
  assert(root->version.is_root);
  // hasNextでないなら、新しいlayerを作る必要はない。
  assert(k.hasNext());
  /**
   * keyの長さの分だけ、layerを作る
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
next_layer:
  auto current = k.getCurrentSlice();
  if(1 <= current.size and current.size <= 7){
    root->key_len[0] = current.size;
    root->key_slice[0] = current.slice;
    root->lv[0].value = value;
  }else{
    assert(current.size == 8);
    if(k.hasNext()){
      root->key_slice[0] = current.slice;
      k.next();
      if(k.hasNext()){
        root->key_len[0] = BorderNode::key_len_layer;
        auto next = new BorderNode;
        next->version.is_root = true;
        root->lv[0].next_layer = next;
        root = next;
        goto next_layer;
      }else{
        root->key_len[0] = 8;
        root->key_suffixes.set(0, k.getCurrentSlice());
        root->lv[0].value = value;
      }
    }else{
      root->key_len[0] = 8;
      root->key_slice[0] = current.slice;
      root->lv[0].value = value;
    }
  }
}

Node *start_new_tree(Key key, void *value){
  auto root = new BorderNode;
  root->version.is_root = true;
  auto result = root;

  auto current = key.getCurrentSlice();
  if(1 <= current.size and current.size <= 7){
    root->key_len[0] = current.size;
    root->key_slice[0] = current.slice;
    root->lv[0].value = value;
  }else{
    assert(current.size == 8);
    if(key.hasNext()){
      root->key_slice[0] = current.slice;
      key.next();
      if(key.hasNext()){
        root->key_len[0] = BorderNode::key_len_layer;
        auto next = new BorderNode;
        next->version.is_root = true;
        root->lv[0].next_layer = next;
        keyInsertAfterNewLayer(next, key, value);
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
  }else{
    assert(current.size == 8);
    if(key.hasNext()){
      border->key_slice[insertion_point] = current.slice;
      key.next();
      if(key.hasNext()){
        border->key_len[insertion_point] = BorderNode::key_len_layer;
        auto next = new BorderNode;
        next->version.is_root = true;
        border->lv[insertion_point].next_layer = next;
        keyInsertAfterNewLayer(next, key, value);
      }else{
        border->key_len[insertion_point] = 8;
        border->key_suffixes.set(insertion_point, key.getCurrentSlice());
        border->lv[insertion_point].value = value;
      }
    }else{
      border->key_len[insertion_point] = 8;
      border->key_slice[insertion_point] = current.slice;
      border->lv[insertion_point].value = value;
    }
  }
}

/**
 * Finds the appropriate place to
 * split a node that is too big into two.
 * @return
 */
size_t cut(size_t len){
  if(len % 2 == 0)
    return len/2;
  else
    return len/2+1;
}

void split_keys_among(InteriorNode *p, InteriorNode *p1, KeySlice slice, Node *n1){
  uint64_t temp_key_slice[Node::ORDER];
  Node* temp_child[Node::ORDER + 1];

  size_t insertion_index = 0;
  while (insertion_index < Node::ORDER - 1
  && p->key_slice[insertion_index] < slice.slice){
    ++insertion_index;
  }

  for(size_t i, j = 0; i < p->n_keys + 1; ++i, ++j){
    if(j == insertion_index + 1) ++j;
    temp_child[j] = p->child[i];
  }
  for(size_t i, j = 0; i < p->n_keys; ++i, ++j){
    if(j == insertion_index) ++j;
    temp_key_slice[j] = p->key_slice[i];
  }

  temp_child[insertion_index + 1] = n1;
  temp_key_slice[insertion_index] = slice.slice;

  std::fill(p->key_slice, p->key_slice + Node::ORDER + 1, 0);
  std::fill(p->child, p->child + Node::ORDER, nullptr);
  p->n_keys = 0;

  size_t split = cut(Node::ORDER);

  size_t i, j;
  for(i = 0; i < split - 1; ++i){
    p->child[i] = temp_child[i];
    p->key_slice[i] = temp_key_slice[i];
    ++p->n_keys;
  }
  p->child[i] = temp_child[i];
  for(++i, j = 0; i < Node::ORDER; ++i, ++j){
    p1->child[j] = temp_child[i];
    p1->key_slice[j] = temp_key_slice[i];
    ++p1->n_keys;
  }
  p1->child[j] = temp_child[i];

  p1->parent = p->parent;
  for(i = 0; i <= p1->n_keys; ++i){
    p1->child[i]->parent = p1;
  }
}

void split_keys_among(BorderNode *n, BorderNode *n1, Key &k, void *value){
  uint8_t temp_key_len[Node::ORDER];
  uint64_t temp_key_slice[Node::ORDER];
  LinkOrValue temp_lv[Node::ORDER];
  KeySlice temp_suffix[Node::ORDER];

  size_t insertion_index = 0;
  while (insertion_index < Node::ORDER - 1
  && n->key_slice[insertion_index] < k.getCurrentSlice().slice){
    ++insertion_index;
  }

  // 16個分、tempにコピー
  for(size_t i, j = 0; i < n->numberOfKeys(); ++i, ++j){
    if(j == insertion_index) ++j;
    temp_key_len[j] = n->key_len[i];
    temp_key_slice[j] = n->key_slice[i];
    temp_lv[j] = n->lv[i];
    temp_suffix[j] = n->key_suffixes.get(i);
  }

  // さて、ここでもnew layerへのリンクの貼り直し
  // keyの幅が尽きるまで
  auto current = k.getCurrentSlice();
  if(1 <= current.size and current.size <= 7){
    temp_key_len[insertion_index] = current.size;
    temp_key_slice[insertion_index] = current.slice;
    temp_lv[insertion_index].value = value;
  }else{
    assert(current.size == 8);
    if(k.hasNext()){
      temp_key_slice[insertion_index] = current.slice;
      k.next();
      if(k.hasNext()){
        temp_key_len[insertion_index] = BorderNode::key_len_layer;
        auto next = new BorderNode;
        next->version.is_root = true;
        temp_lv[insertion_index].next_layer = next;
        keyInsertAfterNewLayer(next, k, value);
      }else{
        temp_key_len[insertion_index] = 8;
        temp_suffix[insertion_index] = k.getCurrentSlice();
        temp_lv[insertion_index].value = value;
      }
    }else{
      temp_key_len[insertion_index] = 8;
      temp_key_slice[insertion_index] = current.slice;
      temp_lv[insertion_index].value = value;
    }
  }

  // clear both nodes.
  std::fill(n->key_len, n->key_len + Node::ORDER - 1, 0);
  std::fill(n->key_slice, n->key_slice + Node::ORDER - 1, 0);
  std::fill(n->lv, n->lv + Node::ORDER - 1, LinkOrValue{});
  n->key_suffixes.reset();

  std::fill(n1->key_len, n1->key_len + Node::ORDER - 1, 0);
  std::fill(n1->key_slice, n1->key_slice + Node::ORDER - 1, 0);
  std::fill(n1->lv, n1->lv + Node::ORDER - 1, LinkOrValue{});
  n1->key_suffixes.reset();

  size_t split = cut(Node::ORDER - 1);

  for(size_t i = 0; i < split; ++i){
    n->key_len[i] = temp_key_len[i];
    n->key_slice[i] = temp_key_slice[i];
    n->lv[i] = temp_lv[i];
    n->key_suffixes.set(i, temp_suffix[i]);
  }

  for(size_t i = split, j = 0; i < Node::ORDER; ++i, ++j){
    n1->key_len[j] = temp_key_len[i];
    n1->key_slice[j] = temp_key_slice[i];
    n1->lv[j] = temp_lv[i];
    n1->key_suffixes.set(j, temp_suffix[i]);
  }

  n1->next = n->next;
  n->next = n1;
  n1->prev = n;

  n1->parent = n->parent;

}

InteriorNode *create_root_with_children(Node *left, KeySlice slice, Node *right){
  auto root = new InteriorNode;
  root->version.is_root = true;
  root->n_keys = 1;
  root->key_slice[0] = slice.slice;
  root->child[0] = left;
  root->child[1] = right;
  left->parent = root;
  right->parent = root;
  return root;
}

void insert_into_parent(InteriorNode *p, Node *n1, KeySlice slice){
  size_t insertion_index = 0;

  while(insertion_index <= p->n_keys &&
  p->key_slice[insertion_index] < slice.slice){
    ++insertion_index;
  }

  // move to right
  for(size_t i = p->n_keys; i > insertion_index; --i){
    p->key_slice[i + 1] = p->key_slice[i];
    p->child[i + 1] = p->child[i];
  }
  p->child[insertion_index + 1] = n1;
  p->key_slice[insertion_index] = slice.slice;
  ++p->n_keys;
}

/**
 *
 * @param n
 * @param k
 * @param value
 * @return new root if not nullptr.
 */
Node *split(Node *n, Key k, void *value){
  // precondition: n locked.
  assert(n->version.locked);
  Node *n1 = new BorderNode;
  n->version.splitting = true;
  // n1 is initially locked
  n1->version = n->version;
  auto slice = k.getCurrentSlice();
  split_keys_among(
    reinterpret_cast<BorderNode *>(n),
    reinterpret_cast<BorderNode *>(n1), k, value);
ascend:
  InteriorNode *p = lockedParent(n);
  if(p == nullptr){
    p = create_root_with_children(n, slice, n1);
    unlock(n);
    std::atomic_thread_fence(std::memory_order_acq_rel);
    unlock(n1);
    return p;
  }else if(p->isNotFull()){
    p->version.inserting = true;
    insert_into_parent(p, n1, slice);
    unlock(n);
    std::atomic_thread_fence(std::memory_order_acq_rel);
    unlock(n1);
    std::atomic_thread_fence(std::memory_order_acq_rel);
    unlock(p);
    return nullptr;
  }else{
    p->version.splitting = true;
    unlock(n);
    Node *p1 = new InteriorNode;
    p1->version = p->version;
    split_keys_among(
      reinterpret_cast<InteriorNode *>(p),
      reinterpret_cast<InteriorNode *>(p1), slice, n1);
    unlock(n1); n = p; n1 = p1; goto ascend;
  }
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
  lock(border);
  auto new_root = split(border, key, value);
  return new_root ? new_root : root;
}

#endif //TRANSACTIONSYSTEM_BPTREE_H
