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
#include "version.h"
#include "key.h"
#include "permutation.h"

struct InteriorNode;

struct Node{
  Version version = {};
  InteriorNode *parent = nullptr;
};

struct InteriorNode: Node{
  uint8_t n_keys = 0;
  uint64_t key_slice[15] = {};
  Node *child[16] = {};

  Node* findChild(KeySlice key){
    /**
     * B+ treeと同じ走査方法
     */
    for(size_t i = 0; i < n_keys; ++i){
      if(key.slice <= key_slice[i]){
        return child[i];
      }
    }

    return nullptr;
  }
};

union LinkOrValue{
  Node *next_layer = nullptr;
  void *value;
};

/**
 * Border nodes store the suffixes of their keys in key-suffixes
 * data structure.
 *
 */
struct KeySuffix{
  std::array<KeySlice, 15> body;

  KeySuffix(){}

  KeySlice operator[](size_t i){
    return body[i];
  }

  void set(size_t i, KeySlice slice){
    body[i] = slice;
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
   */
  uint8_t key_len[15] = {};
  Permutation permutation = {};
  uint64_t key_slice[15] = {};
  LinkOrValue lv[15] = {};
  BorderNode *next = nullptr;
  BorderNode *prev = nullptr;
  KeySuffix key_suffixes = {};

  /**
   * BorderNode内で、keyに該当するLinkOrValueを取得する。
   * @param key
   * @return
   */
  std::pair<ExtractResult, LinkOrValue> extractLinkOrValueFor(Key key){
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

      for(size_t i = 0; i < 15; ++i){
        if(key_slice[i] == current.slice and key_len[i] == current.size){
          return std::pair(VALUE, lv[i]);
        }
      }
    }else{ // next key sliceがある場合
      for(size_t i = 0; i < 15; ++i){

        if(key_slice[i] == current.slice){
          if(key_len[i] == 8){
            // suffixの中を見る
            if(key_suffixes[i].slice == key.next().slice){
              return std::pair(VALUE, lv[i]);
            }else{
              return std::pair(NOTFOUND, LinkOrValue{});
            }
          }

          if(key_len[i] == BorderNode::key_len_layer){
            return std::pair(LAYER, lv[i]);
          }
          if(key_len[i] == BorderNode::key_len_unstable){
            return std::pair(UNSTABLE, LinkOrValue{});
          }
        }
      }
    }

    return std::pair(NOTFOUND, LinkOrValue{});
  }
};

Version stableVersion(Node *n){
  Version v = n->version;
  while(v.inserting or v.splitting)
    v = n->version;
  return v;
}

void lock(Node *n){
  //while(n != nullptr and n->version.locked)
}

void unlock(Node *n){
  // do this atomically

  if(n->version.inserting)
    ++n->version.v_insert;
  else if (n->version.splitting)
    ++n->version.v_split;
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

#endif //TRANSACTIONSYSTEM_MASSTREE_H
