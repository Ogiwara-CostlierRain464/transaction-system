#ifndef TRANSACTIONSYSTEM_MASSTREE_H
#define TRANSACTIONSYSTEM_MASSTREE_H

#include <cstdint>
#include <cstddef>

using Key = uint64_t;

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
  /**
   * 0x0123の2桁目について、0と2の間のnodeに誘導したい時には
   *
   * [0x00FF] (Node) [0x02FF]
   *
   * のようにkeyを設計すれば、findChildの実装(≤)によって、何桁目を
   * 比較しているかを明示的にする必要がなくなる。
   */
  uint64_t key_slice[15] = {};
  Node *child[16] = {};

  Node* findChild(Key key){
    for(size_t i = 0; i < 15; ++i){
      if(key <= key_slice[i]){
        return child[i];
      }
    }
    return child[15];
  }
};

union LinkOrValue{
  Node *next_layer = nullptr;
  void *value;
};

struct KeySuffix{
  ; // ?
};

// こっちは単なる64bit数として捉えた方が良さそう
struct Permutation{
  uint64_t body;

  // index等によるアクセスはmethodとして
};

struct BorderNode: Node{
  static constexpr uint8_t nextLayerKeyLen = 255;

  uint8_t n_removed = 0;
  /**
   * border nodeの各key_sliceの中の
   * sliceの長さを保持する。
   *
   * また、sliceの長さは1~10なので、それ以外の数字を
   * 使ってlink or valueにどちらが含まれているかを
   * 判別できる。
   *
   * 例えば、key_len[1] == 255なら、lv[1]には
   * pointerが含まれる。
   */
  uint8_t key_len[15] = {};
  Permutation permutation = {};
  uint64_t key_slice[15] = {};
  LinkOrValue lv[15] = {};
  BorderNode *next = nullptr;
  BorderNode *prev = nullptr;
  KeySuffix key_suffixes = {};
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

class Masstree{
public:
  Node *root = nullptr;

  BorderNode *findBorder(Key key){
  retry:
    auto n = root; auto v = stableVersion(n);

    if(!v.is_root){
      root = root->parent; goto retry;
    }
  descend:
    if(n->version.is_border){
      return reinterpret_cast<BorderNode *>(n);
    }
    auto interior_n = reinterpret_cast<InteriorNode *>(n);
    auto n1 = interior_n->findChild(key);
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
};

#endif //TRANSACTIONSYSTEM_MASSTREE_H
