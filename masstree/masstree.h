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

struct KeySlice{
  // 最大8byteのスライス
  uint64_t slice = 0;
  // スライスの長さ(1~8)
  uint8_t size = 0;

  KeySlice() = default;

  explicit KeySlice(uint64_t slice_, uint8_t size_)
  : slice(slice_)
  , size(size_)
  {
    assert(1 <= size and size <= 8);
  }

  bool operator==(const KeySlice &rhs) const{
    return slice == rhs.slice and size == rhs.size;
  }

  bool operator!=(const KeySlice &rhs) const{
    return !(*this == rhs);
  }
};

struct Key{
  std::vector<KeySlice> body;
  size_t size;
  size_t cursor = 0;

  Key(std::vector<KeySlice> &body_ , size_t size_)
  : body(std::move(body_)), size(size_)
  {
    assert(1 <= size and size <= 8);
  }

  bool hasNext() {
    if (size == cursor + 1)
      return false;

    return true;
  }

  KeySlice getCurrentSlice(){
    return body[cursor];
  }

  KeySlice next(){
    assert(hasNext());
    ++cursor;
    return body[cursor];
  }

  bool operator==(const Key &rhs){
    if(size != rhs.size){
      return false;
    }
    for(size_t i = 0; i < size; ++i){
      if(body[i] != rhs.body[i]){
        return false;
      }
    }
    return true;
  }

  bool operator!=(const Key &rhs){
    return !(*this == rhs);
  }
};

// 論文のメモのように改造。
struct Version{

  static constexpr uint32_t lock = 0b1000'0000'0000'0000'0000'0000'0000'0000;

  union {
    uint32_t body;
    struct {
      uint16_t v_split: 16;
      uint8_t v_insert: 8;
      bool : 2;
      bool is_border: 1;
      bool is_root: 1;
      bool deleted: 1;
      bool splitting: 1;
      bool inserting: 1;
      bool locked: 1;
    };
  };

  Version()
    : locked{false}
    , inserting{false}
    , splitting{false}
    , deleted{false}
    , is_root{false}
    , is_border{false}
    , v_insert{0}
    , v_split{0}
  {}

  uint32_t operator ^(const Version &rhs){
    return (body ^ rhs.body);
  }
};

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

// こっちは単なる64bit数として捉えた方が良さそう
struct Permutation{
  uint64_t body;

  // index等によるアクセスはmethodとして
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
   * sliceの長さ(byte数)を保持する。
   *
   * また、sliceの長さは0~9なので、それ以外の数字を
   * 使ってlink or valueにどちらが含まれているかを
   * 判別できる。
   *
   * 例えば、key_len[1] == 255なら、lv[1]には
   * pointerが含まれる。
   *
   * 当然、suffixの長さが9以上の場合には8文字分だけ確認を取り、
   * 次のLayerへ移動する
   */
  uint8_t key_len[15] = {};
  Permutation permutation = {};
  uint64_t key_slice[15] = {};
  LinkOrValue lv[15] = {};
  BorderNode *next = nullptr;
  BorderNode *prev = nullptr;
  KeySuffix key_suffixes = {};

  std::pair<ExtractResult, LinkOrValue> extractLinkOrValueFor(Key key){
    // next key sliceがある場合と、ない場合に分けられる
    // ない場合には、current key slice・sizeと合致するものをBorder nodeから探す
    //
    // ある場合には、current key sliceと合致するものをBorder nodeから探す
    // key lenを元に、unstableかlayerか判断する

    auto current = key.getCurrentSlice();

    // hasNextは、必ずしも次のLayerがあるとは限らないぞ…


    // とりあえず、現在のKeyがあるかどうかを調べる
    // 1~7か、ぴったり 8文字か、9文字以上かもしれない
    // 9文字以上でも、同じlayerに治る場合と、そうでない場合がある。


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
