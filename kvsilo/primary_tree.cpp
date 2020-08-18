#include <iostream>
#include "primary_tree.h"

void KVSilo::PrimaryTree::enqueue(Node *new_node) {
  Node *c;
  if(queue == nullptr){
    queue = new_node;
    queue->next = nullptr;
  }else{
    c = queue;
    while(c->next != nullptr){
      c = c->next;
    }
    c->next = new_node;
    new_node->next = nullptr;
  }
}

KVSilo::PrimaryTree::Node *KVSilo::PrimaryTree::dequeue() {
  Node *n = queue;
  queue = queue->next;
  n->next = nullptr;
  return n;
}

void KVSilo::PrimaryTree::printLeaves() {
  if(root == nullptr){
    return;
  }
  Node *c = root;
  while (!c->isLeaf)
    c = static_cast<Node *>(c->pointers[0]);
  while (true){
    for(size_t i = 0; i < c->numKeys; ++i){
      printf("%zu", c->keys[i]);
    }
    if(c->pointers[order - 1] != nullptr){
      printf(" | ");
      c = static_cast<Node *>(c->pointers[order - 1]);
    }else{
      break;
    }
  }
  printf("\n");
}

size_t KVSilo::PrimaryTree::pathToRoot(KVSilo::PrimaryTree::Node *child) {
  size_t len = 0;
  Node *c = child;
  while(c != root){
    c = c->parent;
    ++len;
  }
  return len;
}

void KVSilo::PrimaryTree::printTree() {
  Node *n = nullptr;
  size_t rank = 0;
  size_t new_rank = 0;

  if(root == nullptr){
    printf("Empty tree.\n");
    return;
  }
  queue = nullptr;
  enqueue(root);
  while(queue != nullptr){
    n = dequeue();
    if(n->parent != nullptr && n == n->parent->pointers[0]){
      new_rank = pathToRoot(n);
      if(new_rank != rank){
        rank = new_rank;
        printf("\n");
      }
    }
    for(size_t i = 0; i < n->numKeys; ++i){
      printf("%zu", n->keys[i]);
    }
    if(!n->isLeaf){
      for(size_t i = 0; i <= n->numKeys; ++i){
        enqueue(static_cast<Node *>(n->pointers[i]));
      }
    }
    printf("| ");
  }
  printf("\n");
}

void KVSilo::PrimaryTree::findAndPrint(Key key) {
  Node *leaf = nullptr;
  Record *r = find(key, nullptr);
  if(r == nullptr){
    printf("Record not found under key %zu.\n", key);
  }else{
    printf("Record at %p -- key %zu, value %d.\n",
      r, key, r->value.load(std::memory_order_relaxed));
  }
}

void KVSilo::PrimaryTree::findAndPrintRange(Key key_start, Key key_end) {
  size_t array_size = key_end - key_start + 1;
  Key out_keys[array_size];
  void* out_pointers[array_size];
  int num_found = findRange(key_start, key_end, out_keys, out_pointers);

  if(!num_found){
    printf("None found.\n");
  }else{
    for(size_t i = 0; i < num_found; ++i){
      printf("Key: %zu Location: %p Value: %d\n",
        out_keys[i],
        out_pointers[i],
        static_cast<Record*>(out_pointers[i])->value.load(std::memory_order_relaxed)
        );
    }
  }
}

size_t KVSilo::PrimaryTree::findRange(Key key_start, Key key_end,
                                      Key *out_keys, void **out_pointers) {
  size_t i, num_found;
  num_found = 0;
  Node *n = findLeaf(key_start);
  if(n == nullptr){
    return 0;
  }
  for(i = 0; i < n->numKeys && n->keys[i] < key_start; ++i);
  if(i == n->numKeys) return 0;
  while(n != nullptr){
    for(; i < n->numKeys && n->keys[i] <= key_end; ++i){
      out_keys[num_found] = n->keys[i];
      out_pointers[num_found] = n->pointers[i];
      ++num_found;
    }
    n = static_cast<Node *>(n->pointers[order - 1]);
    i = 0;
  }
  return num_found;
}

KVSilo::PrimaryTree::Node *KVSilo::PrimaryTree::findLeaf(Key key) {
  if(root == nullptr){
    return root;
  }
  size_t i = 0;
  Node *c = root;
  while(!c->isLeaf){
    i = 0;
    while(i < c->numKeys){
      if(key >= c->keys[i]) ++i;
      else break;
    }
    c = static_cast<Node *>(c->pointers[i]);
  }
  return c;
}

KVSilo::Record *KVSilo::PrimaryTree::find(Key key, Node **out_leaf) {
  if(root == nullptr){
    if(out_leaf != nullptr){
      *out_leaf = nullptr;
    }
    return nullptr;
  }

  size_t i = 0;
  Node *leaf = nullptr;

  leaf = findLeaf(key);

  /**
   * If root != nullptr, leaf must have a value, even
   * if it does not contain the desired key.
   */
   for(i = 0; i < leaf->numKeys; ++i){
     if(leaf->keys[i] == key) break;
   }
   if(out_leaf != nullptr){
     *out_leaf = leaf;
   }
   if(i == leaf->numKeys){
     return nullptr;
   }else{
     return static_cast<Record *>(leaf->pointers[i]);
   }
}

size_t KVSilo::PrimaryTree::cut(size_t length) {
  if(length % 2 == 0){
    return length/2;
  }else{
    return length/2 + 1;
  }
}

KVSilo::Record *KVSilo::PrimaryTree::makeRecord(int value){
  auto *new_record = new Record(value);
  return new_record;
}

KVSilo::PrimaryTree::Node *KVSilo::PrimaryTree::makeNode() {
  Node *new_node = new Node();
  return new_node;
}

KVSilo::PrimaryTree::Node *KVSilo::PrimaryTree::makeLeaf() {
  Node *leaf = makeNode();
  leaf->isLeaf = true;
  return leaf;
}

size_t KVSilo::PrimaryTree::getLeftIndex(Node *parent, KVSilo::PrimaryTree::Node *left) {
  size_t left_index = 0;
  while(left_index <= parent->numKeys &&
    parent->pointers[left_index] != left){
    ++left_index;
  }
  return left_index;
}

KVSilo::PrimaryTree::Node *KVSilo::PrimaryTree::insertIntoLeaf(Node *leaf, Key key, Record *ptr) {
  size_t insertion_point;
  insertion_point = 0;
  while(insertion_point < leaf->numKeys && leaf->keys[insertion_point] < key){
    ++insertion_point;
  }
  for(size_t i = leaf->numKeys; i > insertion_point; --i){ // move to right 1 step.
    leaf->keys[i] = leaf->keys[i - 1];
    leaf->pointers[i] = leaf->pointers[i - 1];
  }
  leaf->keys[insertion_point] = key;
  leaf->pointers[insertion_point] = ptr;
  leaf->numKeys++;
  return leaf;
}

KVSilo::PrimaryTree::Node *KVSilo::PrimaryTree::insertInfoLeafAfterSplitting(Node *leaf, Key key,Record *ptr) {
  Key temp_keys[order];
  void* temp_pointers[order];

  Node *new_leaf = makeLeaf();
  size_t insertion_index = 0;
  while(insertion_index < order - 1 && leaf->keys[insertion_index] < key){
    ++insertion_index;
  }

  for(size_t i = 0, j = 0; i < leaf->numKeys; ++i, ++j){
    if(j == insertion_index) ++j;
    temp_keys[j] = leaf->keys[i];
    temp_pointers[j] = leaf->pointers[i];
  }

  temp_keys[insertion_index] = key;
  temp_pointers[insertion_index] = ptr;

  leaf->numKeys = 0;

  size_t split = cut(order - 1);

  for(size_t i = 0; i < split; ++i){ // copy back
    leaf->pointers[i] = temp_pointers[i];
    leaf->keys[i] = temp_keys[i];
    leaf->numKeys++;
  }
  for(size_t i = split, j = 0; i < order; ++i, ++j){
    new_leaf->pointers[j] = temp_pointers[i];
    new_leaf->keys[j] = temp_keys[i];
    new_leaf->numKeys++;
  }

  new_leaf->pointers[order - 1] = leaf->pointers[order - 1];
  leaf->pointers[order - 1] = new_leaf;

  for(size_t i = leaf->numKeys; i < order - 1; ++i){
    leaf->pointers[i] = nullptr;
  }
  for(size_t i = new_leaf->numKeys; i < order - 1; ++i){
    new_leaf->pointers[i] = nullptr;
  }

  new_leaf->parent = leaf->parent;
  size_t new_key = new_leaf->keys[0];

  return insertIntoParent(leaf, new_key, new_leaf);
}

KVSilo::PrimaryTree::Node *KVSilo::PrimaryTree::insertIntoNode(Node *n, size_t left_index, Key key, Node *right) {
  for(size_t i = n->numKeys; i > left_index; --i){ // move right for 1 step
    n->pointers[i + 1] = n->pointers[i];
    n->keys[i] = n->keys[i - 1];
  }
  n->pointers[left_index + 1] = right;
  n->keys[left_index] = key;
  n->numKeys++;
  return root;
}

KVSilo::PrimaryTree::Node *KVSilo::PrimaryTree::insertIntoNodeAfterSplitting(Node *old_node, size_t left_index,Key key, Node *right) {
  /* First create a temporary set of keys and pointers
   * to hold everything in order, including
   * the new key and pointer, inserted in their
   * correct places.
   * Then create a new node and copy half of the
   * keys and pointers to the old node and
   * the other half to the new.
   */
  size_t i,j;
  void* temp_pointers[order + 1];
  Key temp_keys[order];

  for (i = 0, j = 0; i < old_node->numKeys + 1; ++i, ++j) {
    if (j == left_index + 1) j++;
    temp_pointers[j] = old_node->pointers[i];
  }

  for (i = 0, j = 0; i < old_node->numKeys; i++, j++) {
    if (j == left_index) j++;
    temp_keys[j] = old_node->keys[i];
  }

  temp_pointers[left_index + 1] = right;
  temp_keys[left_index] = key;

  /* Create the new node and copy
   * half the keys and pointers to the
   * old and half to the new.
   */
  size_t split = cut(order);
  Node *new_node = makeNode();
  old_node->numKeys = 0;

  for (i = 0; i < split - 1; i++) {
    old_node->pointers[i] = temp_pointers[i];
    old_node->keys[i] = temp_keys[i];
    old_node->numKeys++;
  }
  old_node->pointers[i] = temp_pointers[i];
  size_t k_prime = temp_keys[split - 1];
  for (++i, j = 0; i < order; i++, j++) {
    new_node->pointers[j] = temp_pointers[i];
    new_node->keys[j] = temp_keys[i];
    new_node->numKeys++;
  }
  new_node->pointers[j] = temp_pointers[i];
  new_node->parent = old_node->parent;
  Node *child;
  for (i = 0; i <= new_node->numKeys; i++) {
    child = static_cast<Node *>(new_node->pointers[i]);
    child->parent = new_node;
  }

  /* Insert a new key into the parent of the two
   * nodes resulting from the split, with
   * the old node to the left and the new to the right.
   */
  return insertIntoParent(old_node, k_prime, new_node);
}

KVSilo::PrimaryTree::Node *KVSilo::PrimaryTree::insertIntoParent(Node *left, Key key,Node *right) {
  Node *parent = left->parent;
  size_t left_index;

  // Case: new root.
  if(parent == nullptr)
    return insertIntoNewRoot(left, key, right);

  // Case: leaf or node.
  // find the parent's pointer to the left node.
  left_index = getLeftIndex(parent, left);

  // Simple case: the new key fits into the node.
  if(parent->numKeys < order - 1)
    return insertIntoNode(parent, left_index, key, right);

  // Harder case: split a node in order to
  // preserve the B+ tree properties.

  return insertIntoNodeAfterSplitting(parent, left_index, key, right);
}

KVSilo::PrimaryTree::Node *KVSilo::PrimaryTree::insertIntoNewRoot(Node *left, Key key, Node *right) {
  Node *node = makeNode();
  node->keys[0] = key;
  node->pointers[0] = left;
  node->pointers[1] = right;
  node->numKeys++;
  node->parent = nullptr;
  left->parent = node;
  right->parent = node;
  return node;
}

KVSilo::PrimaryTree::Node *KVSilo::PrimaryTree::startNewTree(Key key, Record *ptr) {
  Node *node = makeLeaf();
  node->keys[0] = key;
  node->pointers[0] = ptr;
  node->pointers[order - 1] = nullptr;
  node->parent = nullptr;
  node->numKeys++;
  return node;
}

KVSilo::PrimaryTree::Node *KVSilo::PrimaryTree::insert(KVSilo::Key key, int value) {
  Record *record_ptr = nullptr;
  Node *leaf = nullptr;

  record_ptr = find(key, nullptr);
  if(record_ptr != nullptr){
    record_ptr->value = value;
    return root;
  }

  record_ptr = makeRecord(value);
  if(root == nullptr){
    return startNewTree(key, record_ptr);
  }

  leaf = findLeaf(key);

  if(leaf->numKeys < order - 1){
    leaf = insertIntoLeaf(leaf, key, record_ptr);
    return root;
  }
  return insertInfoLeafAfterSplitting(leaf, key, record_ptr);
}

int KVSilo::PrimaryTree::read(Key key) {
  Record *r = find(key, nullptr);
  assert(r);
  return r->value;
}

void KVSilo::PrimaryTree::write(Key key, int value) {
  Record *r = find(key, nullptr);
  assert(r);
  r->value = value;
}










