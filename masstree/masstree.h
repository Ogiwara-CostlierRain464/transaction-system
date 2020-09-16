#ifndef TRANSACTIONSYSTEM_MASSTREE_H
#define TRANSACTIONSYSTEM_MASSTREE_H

#include <cstdint>
#include <cstddef>
#include <cassert>
#include <tuple>
#include <utility>
#include <vector>
#include <algorithm>
#include <optional>
#include <tuple>
#include "version.h"
#include "key.h"
#include "permutation.h"

struct InteriorNode;
struct BorderNode;

struct Node{
  static constexpr size_t ORDER = 16;

  /*alignas(?)*/ Version version = {};
  InteriorNode *parent = nullptr;
};

struct InteriorNode: Node{
  /* 0~15 */
  uint8_t n_keys = 0;
  uint64_t key_slice[ORDER - 1] = {};
  Node *child[ORDER] = {};

  Node* findChild(KeySlice key){
    /**
     * B+ treeと同じ走査方法
     */
    for(size_t i = 0; i < n_keys; ++i){
      if(key.slice < key_slice[i]){
        return child[i];
      }
    }

    return child[n_keys];
  }

  bool isNotFull(){
    return n_keys != ORDER - 1;
  }

  void printNode(){
    printf("/");
    for(size_t i = 0; i < ORDER - 1; ++i){
      printf("%llu/", key_slice[i]);
    }
    printf("\\\n");
  }
};

union LinkOrValue{
  /*alignas*/ Node *next_layer = nullptr;
  /*alignas*/ void *value;
};

/**
 * Border nodes store the suffixes of their keys in key-suffixes
 * data structure.
 *
 */
struct KeySuffix{
  // NOTE: 性能改善の余地
  std::array<KeySlice, Node::ORDER - 1> body;

  KeySuffix()= default;

  KeySlice operator[](size_t i){
    return body[i];
  }

  KeySlice get(size_t i){
    return body[i];
  }

  void set(size_t i, KeySlice slice){
    body[i] = slice;
  }

  /**
   * bodyをリセットする
   */
  void reset(){
    std::fill(body.begin(), body.end(), KeySlice());
  }
};


enum ExtractResult: uint8_t {
  NOTFOUND,
  VALUE,
  LAYER,
  UNSTABLE
};

struct BorderNode: Node{
  static constexpr uint8_t key_len_layer = 255;
  static constexpr uint8_t key_len_unstable = 254;

  uint8_t n_removed = 0;
  /**
   * border nodeの各key_sliceの中の
   * sliceの長さ(byte数)を表す
   *
   * 1~7の時
   * LinkOrValueにはvalueがある
   *
   * 8の時
   * 残りのSuffixがkey_suffixesにある
   *
   * 254の時
   * LinkOrValueの状態が不安定、readerは最初からやり直し
   *
   * 255の時
   * LinkOrValueにはnext_layerがある
   *
   * NOTE: どんな時にslice長が0byteになるか考察する
   */
  uint8_t key_len[ORDER - 1] = {};
  Permutation permutation = {};
  uint64_t key_slice[ORDER - 1] = {};
  LinkOrValue lv[ORDER - 1] = {};
  BorderNode *next = nullptr;
  BorderNode *prev = nullptr;
  KeySuffix key_suffixes = {};

  explicit BorderNode(){
    version.is_border = true;
  }

  /**
   * BorderNode内で、keyに該当するLinkOrValueを取得する。
   * @param key
   * @return
   */
  std::pair<ExtractResult, LinkOrValue> extractLinkOrValueFor(Key key){
    auto triple = extractLinkOrValueWithIndexFor(key);
    return std::pair(std::get<0>(triple), std::get<1>(triple));
  }

  /**
   * BorderNode内で、keyに該当するLinkOrValueを取得する
   * また、見つけた位置のindexも返す
   * @param key
   * @return
   */
  std::tuple<ExtractResult, LinkOrValue, size_t> extractLinkOrValueWithIndexFor(Key key){
    /**
     * 現在のkey sliceの次のkey sliceがあるかどうかで場合わけ
     *
     * 次のkey sliceがない場合は、現在のkey sliceの長さは1~8
     * 15個全てのスロットについて、現在のkey sliceと一致するかどうか確認し、valueを返す
     *
     * 次のkey sliceがある場合は、現在のkey sliceの長さは8
     * 現在のkey sliceとnode中のkey_sliceが一致する場所を見つけ、そこでさらに場合わけ
     *
     * もし、key_lenが8の場合は、残りのkey sliceは一個のみであり、次のkey sliceはKey Suffixに保存されている
     * のでそこからvalueを返す
     * key_lenがLAYERを表す場合は、次のlayerを返す
     * key_lenがUNSTABLEの場合は、UNSTABLEを返す
     *
     * どれにも該当しない場合、NOTFOUNDを返す
     *
     */
    auto current = key.getCurrentSlice();

    if(!key.hasNext()){ // next key sliceがない場合

      for(size_t i = 0; i < ORDER - 1; ++i){
        if(key_slice[i] == current.slice and key_len[i] == current.size){
          return std::tuple(VALUE, lv[i], i);
        }
      }
    }else{ // next key sliceがある場合
      for(size_t i = 0; i < ORDER - 1; ++i){

        if(key_slice[i] == current.slice){
          if(key_len[i] == 8){
            // suffixの中を見る
            if(key_suffixes[i].slice == key.next().slice){
              return std::tuple(VALUE, lv[i], i);
            }else{
              return std::tuple(NOTFOUND, LinkOrValue{}, 0);
            }
          }

          if(key_len[i] == BorderNode::key_len_layer){
            return std::tuple(LAYER, lv[i], i);
          }
          if(key_len[i] == BorderNode::key_len_unstable){
            return std::tuple(UNSTABLE, LinkOrValue{}, 0);
          }
        }
      }
    }

    return std::tuple(NOTFOUND, LinkOrValue{}, 0);
  }

  /*
   * 新たにkey sliceを挿入する余裕があるかどうか
   */
  bool hasSpace() const{
    // key_lenが0 ⇒ その要素は空、と仮定
    // NOTE: key sliceの長さが0の場合があった場合は変更の必要
    return key_len[ORDER - 2] == 0;
  }

  /**
   * 有効なkeyの数
   * @return
   */
  size_t numberOfKeys(){
    // key_lenが0 ⇒ その要素は空、と仮定
    // NOTE: key sliceの長さが0の場合があった場合は変更の必要
    size_t result = 0;
    for(size_t i = 0; i < ORDER - 1; ++i){
      if(key_len[i] > 0){
        ++result;
      }else{
        break;
      }
    }
    return result;
  }

  void printNode(){
    printf("|");
    for(size_t i = 0; i < ORDER - 1; ++i){
      printf("%llu|", key_slice[i]);
    }
    printf("\n");
  }
};

Version stableVersion(Node *n){
  Version v = n->version;
  while(v.inserting or v.splitting)
    v = n->version;
  return v;
}

void lock(Node *n){
  if(n == nullptr){
    return;
  }

  n->version.locked = true;

//  for(;;){
//    Version expected = n->version;
//    Version desired = expected;
//    desired.locked = true;
//
//    if(__atomic_compare_exchange_n(&n->version.body,
//                                &expected.body,
//                                desired.body,
//                                false, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)){
//      break;
//    }
//  }
}

void unlock(Node *n){
  auto copy_v = n->version;

  if(copy_v.inserting){
    ++copy_v.v_insert;
  } else if(copy_v.splitting){
    ++copy_v.v_split;
  }
  copy_v.locked = false;
  copy_v.inserting = false;
  copy_v.splitting = false;

  n->version = copy_v;
}

InteriorNode *lockedParent(Node *n){
  retry: InteriorNode *p = n->parent; lock(p);
  if(p != n->parent){
    unlock(p); goto retry;
  }

  return p;
}

struct Masstree{
  Node *root = nullptr;
};

std::pair<BorderNode *, Version> findBorder(Node *root, Key key){
  retry:
  auto n = root; auto v = stableVersion(n);

  if(!v.is_root){
    root = root->parent; goto retry;
  }
  descend:
  if(n->version.is_border){
    return std::pair(reinterpret_cast<BorderNode *>(n), v);
  }
  auto interior_n = reinterpret_cast<InteriorNode *>(n);
  auto n1 = interior_n->findChild(key.getCurrentSlice());
  Version v1 = stableVersion(n1);
  if((n->version ^ v) <= Version::lock){
    n = n1; v = v1; goto descend;
  }
  auto v2 = stableVersion(n);
  if(v2.v_split != v.v_split){
    goto retry;
  }
  v = v2; goto descend;
}

void *get(Node *root, Key k){
  retry:
  auto n_v = findBorder(root, k); auto n = n_v.first; auto v = n_v.second;
  forward:
  if(v.deleted)
    goto retry;
  auto t_lv = n->extractLinkOrValueFor(k); auto t = t_lv.first; auto lv = t_lv.second;
  if((n->version ^ v) > Version::lock){
    v = stableVersion(n); auto next = n->next;
    while(!v.deleted and next != nullptr /**/){
      n = next; v = stableVersion(n); next = n->next;
    }
    goto forward;
  }else if(t == NOTFOUND){
    return nullptr;
  }else if(t == VALUE){
    return lv.value;
  }else if(t == LAYER){
    root = lv.next_layer;
    // advance k to next slice
    k.next();
    goto retry;
  }else{ // t == UNSTABLE
    goto forward;
  }
}

/**
 * 存在するkeyに対して、そのvalueを書き換える
 * writeする位置の探索までは、論文中のgetを参考にする。
 *
 * keyが存在し、書き込みに成功した時にはtrueを、
 * keyが存在しなかった時にはfalseを返す
 */
bool write(Node* root, Key k, void* value){
  retry:
  auto n_v = findBorder(root, k); auto n = n_v.first; auto v = n_v.second;
  forward:
  if(v.deleted)
    goto retry;
  auto t_lv_i = n->extractLinkOrValueWithIndexFor(k);
  auto t = std::get<0>(t_lv_i);
  auto lv = std::get<1>(t_lv_i);
  auto index = std::get<2>(t_lv_i);
  if((n->version ^ v) > Version::lock){
    v = stableVersion(n); auto next = n->next;
    while(!v.deleted and next != nullptr /**/){
      n = next; v = stableVersion(n); next = n->next;
    }
    goto forward;
  }else if(t == NOTFOUND){
    return false;
  }else if(t == VALUE){
    n->lv[index].value = value;
    return true;
  }else if(t == LAYER){
    root = lv.next_layer;
    // advance k to next slice
    k.next();
    goto retry;
  }else{ // t == UNSTABLE
    goto forward;
  }
}



#endif //TRANSACTIONSYSTEM_MASSTREE_H
