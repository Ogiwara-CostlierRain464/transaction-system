#ifndef TRANSACTIONSYSTEM_MASSTREE_H
#define TRANSACTIONSYSTEM_MASSTREE_H

#include <cstdint>

/**
 * C++のbit field, 及びAtomicの保証について調べないとね
 * あとはCastの種類
 */

struct Version{
  union {
    uint32_t body;
    struct {
      bool locked: 1;
      bool inserting: 1;
      bool splitting: 1;
      bool deleted: 1;
      bool is_root: 1;
      bool is_border: 1;
      uint8_t v_insert: 7;
      uint32_t v_split: 18;
      bool unused: 1;
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
    , unused{false}
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
};

union LinkOrValue{
  Node *next_layer = nullptr;
  void *value;
};

struct KeySuffix{
  ; // ?
};

struct uint4_t{
  uint8_t body: 4;
};

union Permutation{
  uint64_t body;
  struct {
    uint4_t key_index[15];
    uint4_t n_keys;
  };
};

struct BorderNode: Node{
  uint8_t n_removed = 0;
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
  // Layerの概念はstructで表現する必要はない。

  Node *root = nullptr;

  Node *findBorder(uint64_t key){
    // is border判定は、やはりvirtual functionで行うしかない？
    // まあ、逆にうまいHackでversionだけ見るという手段もある
  retry:
    auto n = root; auto v = stableVersion(n);

    if(!v.is_root){
      root = root->parent; goto retry;
    }
  descend:
    if(n->version.is_border){
      return n;
    }
    auto border_n = static_cast<InteriorNode *>(n);
    auto n1 = border_n->child[0];
    Version v1 = stableVersion(n1);
    if((n->version ^ v) <= 0b1000'0000'0000'0000'0000'0000'0000'0000){
      n = n1; v = v1; goto descend;
    }
    auto v2 = stableVersion(n);
    if(v2.v_split != v.v_split){
      goto retry;
    }
    v = v2; goto descend;
  }
};

// rootの判断が面倒。もしかしたら、C++ではなくCのようにrootを持たせるべき？
/**
 * まず、基本的な動きはtrie treeと同じ
 * 一つのLayer内で、interiorが増えていく
 *
 * 新しいLayerになっても、結局はrootはborder nodeから始まる
 *
 * まあ、どっかしらに「root is not border」のマーク付けないとな
 * なら、Layerで分けると簡単そう？
 *
 */

#endif //TRANSACTIONSYSTEM_MASSTREE_H
