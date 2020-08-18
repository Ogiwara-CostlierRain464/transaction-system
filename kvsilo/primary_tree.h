#ifndef TRANSACTIONSYSTEM_PRIMARY_TREE_H
#define TRANSACTIONSYSTEM_PRIMARY_TREE_H

#include <cstdio>
#include "record.h"

namespace KVSilo{

using Key = size_t;

class PrimaryTree{
public:
  static constexpr size_t order = 4;

  struct Node{
    void **pointers;
    Key *keys;
    Node *parent;
    bool isLeaf;
    size_t numKeys;
    Node *next;

    explicit Node()
    : pointers(new void*[order])
    , keys(new Key[order - 1])
    , parent(nullptr)
    , isLeaf(false)
    , numKeys(0)
    , next(nullptr){}
  };

  Node *queue = nullptr;
  Node *root = nullptr;

  explicit PrimaryTree(){
    // 予めデータを用意しておく
    root = insert(1,1);
    root = insert(2,2);
    root = insert(3,3);
    root = insert(4,4);
    root = insert(5,5);
    root = insert(6,6);
    root = insert(7,7);
    root = insert(8,8);
  }

  /**
   * Enqueue to master queue.
   * Not thread-safe.
   * @param new_node
   */
  void enqueue(Node *new_node);
  /**
   * Dequeue to master queue.
   * Not thread-safe.
   * @return Top Node
   */
  Node *dequeue();
  /**
   * Prints the bottom row of keys of the tree.
   * Not thread-safe.
   */
  void printLeaves();
  /**
   * Give the length in edges of the path from any node to the root.
   * @param child
   * @return length of edges to root.
   */
  size_t pathToRoot(Node *child);
  /**
   * Prints the B+ tree.
   */
  void printTree();
  /**
   * Finds the record under a given key and prints.
   * @param key
   */
  void findAndPrint(Key key);
  /**
   * Finds and prints the keys, pointers, and values within a range
   * of keys between key_start and key_end, including both bounds.
   * @param key_start
   * @param key_end
   */
  void findAndPrintRange(Key key_start, Key key_end);
  /**
   * Finds keys and their pointers, if present, in the range
   * specified by key_start and key_end, inclusive.
   * @param key_start
   * @param key_end
   * @param[out] outKeys
   * @param[out] outPointers
   * @return
   */
  size_t findRange(Key key_start, Key key_end, Key *out_keys, void* *out_pointers);
  /**
   * Traces the path from the root to a leaf, searching by key.
   * @param key
   * @return
   */
  Node *findLeaf(Key key);
  /**
   * Finds and returns the record to which a key refers.
   * @param key
   * @param[out] out_leaf
   * @return
   */
  Record *find(Key key, Node **out_leaf);
  /**
   * Finds the appropriate place to
   * split a node that is too big into two.
   * @param length
   * @return
   */
  size_t cut(size_t length);


  /**
   * Creates a new record to hold the value
   * to which a key refers.
   * @param value
   * @return created record.
   */
  Record *makeRecord(int value);
  /**
   * Creates a new general node, which can be adapted
   * to serve as either a leaf or an internal node.
   * @return created node/
   */
  Node *makeNode();
  /**
   * Creates a new leaf by creating a node
   * and then adapting it appropriately.
   * @return
   */
  Node *makeLeaf();
  /**
   * Helpers function used in insert_into_parent,
   * to find the index of the parent's pointer to
   * the node to the left of the key to be inserted.
   * @param parent
   * @param left
   * @return
   */
  size_t getLeftIndex(Node *parent, Node *left);
  /**
   * Inserts a new pointer to a record and its corresponding
   * key into a leaf.
   * @param leaf
   * @param key
   * @param ptr
   * @return altered leaf.
   */
  Node *insertIntoLeaf(Node *leaf, Key key, Record *ptr);
  /**
   * Inserts a new key and pointer to a new record into a leaf so as to exceed
   * the tree's order, causing the leaf to be split in half.
   * @param leaf
   * @param key
   * @param ptr
   * @return
   */
  Node *insertInfoLeafAfterSplitting(Node *leaf, Key key, Record *ptr);
  /**
   * Inserts a new key and pointer to a node into a node
   * into which there can fit without violating the
   * B+ tree properties.
   * @param n
   * @param left_index
   * @param key
   * @param right
   * @return
   */
  Node *insertIntoNode(Node *n, size_t left_index, Key key, Node *right);
  /**
   * Inserts a new key and pointer to a node into a node,
   * causing the node's size to exceed the order, and causing
   * the node to split into two.
   * @param old_node
   * @param left_index
   * @param key
   * @param right
   * @return
   */
  Node *insertIntoNodeAfterSplitting(Node *old_node, size_t left_index, Key key, Node *right);
  /**
   * Inserts a new node (leaf or internal node) into the B+ tree.
   * @param left
   * @param key
   * @param right
   * @return root of the tree
   */
  Node *insertIntoParent(Node *left, Key key, Node *right);
  /**
   * Creates a new root for two subtrees and inserts the
   * appropriate key into the new root.
   * @param left
   * @param key
   * @param right
   * @return
   */
  Node *insertIntoNewRoot(Node *left, Key key, Node *right);
  /**
   * First insertion: start a new tree.
   * @param key
   * @param ptr
   * @return
   */
  Node *startNewTree(Key key, Record *ptr);
  /**
   *
   * @param key
   * @param value
   * @return
   */
  Node *insert(Key key, int value);

  int read(Key key);
  void write(Key key, int value);
};

}

#endif //TRANSACTIONSYSTEM_PRIMARY_TREE_H
