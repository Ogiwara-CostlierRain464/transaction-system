#ifndef TRANSACTIONSYSTEM_MASSTREE_H
#define TRANSACTIONSYSTEM_MASSTREE_H

#include <cstdint>

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

struct InteriorNode{
  // atomicにしないん？
  Version version = {};
  InteriorNode *parent = nullptr;
  uint8_t n_keys = 0;
  uint64_t key_slice[15] = {};
  void *child[16] = {};
};


struct Layer{
  // Debug用に、Layer numberの記憶しておくとよいかも
  void *root = nullptr;
};


union LinkOrValue{
  Layer *next_layer = nullptr;
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

struct BorderNode{
  Version version = {};
  InteriorNode *parent = nullptr;
  uint8_t n_removed = 0;
  uint8_t key_len[15] = {};
  Permutation permutation = {};
  uint64_t key_slice[15] = {};
  LinkOrValue lv[15] = {};
  BorderNode *next = nullptr;
  BorderNode *prev = nullptr;
  KeySuffix key_suffixes = {};
};

Version stableVersion(void *node){
  auto n = static_cast<InteriorNode *>(node);
  Version v = n->version;
  while(v.inserting or v.splitting)
    v = n->version;
  return v;
}

void lock(void *node){
  auto n = static_cast<InteriorNode *>(node);
  //while(n != nullptr and n->version.locked)
}

void unlock(void *node){
  auto n = static_cast<InteriorNode *>(node);

  if(n->version.inserting)
    ++n->version.v_insert;
  else if (n->version.splitting)
    ++n->version.v_split;
  //
}

bool isBorder(void *node){
  auto n = static_cast<InteriorNode *>(node);
  return n->version.is_border;
}

InteriorNode *lockedParent(void *node){
  auto n = static_cast<InteriorNode *>(node);
  retry: InteriorNode *p = n->parent; lock(p);
  if(p != n->parent){
    unlock(p); goto retry;
  }

  return p;
}

class Masstree{
public:
  // Layerの概念はstructで表現する必要はない。

  Layer *rootLayer = nullptr;

  void *findBorder(uint64_t key){
    // is border判定は、やはりvirtual functionで行うしかない？
    // まあ、逆にうまいHackでversionだけ見るという手段もある
    void *root = rootLayer->root;
  retry:
    void *node = root;
    auto n = static_cast<InteriorNode *>(node);
    auto v = stableVersion(n);

    if(!v.is_root){
      root = static_cast<InteriorNode *>(root)->parent; goto retry;
    }
  descend:
    if(isBorder(n)){
      return n;
    }
    void *n1 = n->child[0];
    Version v1 = stableVersion(n1);
    if((n->version ^ v) <= 0b1000'0000'0000'0000'0000'0000'0000'0000){
      n = static_cast<InteriorNode *>(n1); v = v1; goto descend;
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
