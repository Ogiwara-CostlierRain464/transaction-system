#include <iostream>
#include <string>
#include <cmath>

constexpr size_t N = 3;
constexpr size_t INF = 99999;
using Key = size_t;

struct Node{
  size_t size;
  Node **p_body;
  Key *k_body;
  Node *parent;
  // Used for `Value Node`.
  char *value;

  explicit Node(size_t size_ = N-1){
    size = size_;
    p_body = new Node*[size+1];
    k_body = new Key[size];

    for(size_t i = 0; i < size+1; ++i){
      p_body[i] = nullptr;
    }
    for(size_t i = 0; i < size ; ++i){
      k_body[i] = INF;
    }
    parent = nullptr;
  }

  Node *p(size_t index){
    return p_body[index - 1];
  }

  void set_p(size_t index, Node *p){
    p_body[index - 1] = p;
  }

  Key k(size_t index){
    return k_body[index - 1];
  }

  void set_k(size_t index, Key key){
    k_body[index - 1] = key;
  }

  void insert_before_P1(Node *P, Key K){
    for(size_t i = size - 1; i >= 1; --i){
      set_k(i + 1, k(i));
    }
    for(size_t i = size - 1; i >= 1; --i){
      set_p(i + 1, p(i));
    }
    set_p(1, P);
    set_k(1, K);
  }

  size_t highest_key_index_less_than_or_equal_to_arg(Key K){
    size_t prev = 1;
    for(size_t i = 1; i <= size; ++i){
      if(K >= k(i) and k(i) > prev){
        prev = i;
      }
    }
    return prev;
  }

  bool is_not_full(){
    return k(size) == INF;
  }

  bool has_less_than_n_pointers(){
    return p(size+1) == nullptr;
  }

  Key smallest_key(){
    return k(1);
  }

  bool is_leaf(){
    return p(1)->value != nullptr;
  }

  void erase_p_and_k(){
    for(size_t i = 0; i < size+1; ++i){
      p_body[i] = nullptr;
    }
    for(size_t i = 0; i < size ; ++i){
      k_body[i] = INF;
    }
  }

  void erase_partly(){
    Node *tmp = p(N);
    erase_p_and_k();
    set_p(N, tmp);
  }

  Node *clone(){
    Node *copy = new Node();
    memcpy(copy->p_body, p_body, size+1);
    memcpy(copy->k_body, k_body, size);
    copy->parent = parent;
    copy->size = size;
    return copy;
  }

  void add_size(){ // for 1
    Node **new_p_body = new Node*[size+1+1];
    Key *new_k_body = new Key[size+1];

    memcpy(new_p_body, p_body, size+1);
    memcpy(new_k_body, k_body, size);

    delete[] p_body;
    delete[] k_body;

    new_p_body[size+1] = nullptr;
    new_k_body[size] = INF;

    size += 1;
  }

  std::string to_string() {
    std::string res;
    for (size_t i = 1; i <= size; ++i) {
      res += "(" + std::to_string(k(i)) + ")";
    }
    return res;
  }
};

struct Tree{
  Node *root = nullptr;

  bool is_empty(){
    return root == nullptr;
  }
};

Tree tree;

Node *search_leaf(Node *node, Key contain){
  if(node->is_leaf()){
    return node;
  }
  if(0 < contain and contain < node->k(1)){
    return search_leaf(node->p(1), contain);
  }
  for(size_t i = 1 ; i <= node->size - 1; ++i){
    if(node->k(i) <= contain and contain < node->k(i+1)){
      return search_leaf(node->p(i+1), contain);
    }
  }
  if(node->k(node->size) < contain){
    return search_leaf(node->p(node->size + 1), contain);
  }
  assert(false);
}

void insert_in_leaf(Node *L, Key K, Node *P){
  if(K < L->k(1)){
    L->insert_before_P1(P, K);
  }else{
    size_t index = L->highest_key_index_less_than_or_equal_to_arg(K);
    for(size_t i = L->size - 1; i >= index + 1; --i){
      L->set_k(i + 1, L->k(i));
    }
    for(size_t i = L->size - 1; i >= index + 1; --i){
      L->set_p(i + 1, L->p(i));
    }
    L->set_p(index + 1, P);
    L->set_k(index + 1, K);
  }
}

void insert_in_parent(Node *n, Key K_, Node* N_){
  if(n == tree.root){
    Node *R = new Node;
    R->set_p(1, n);
    n->parent = R;
    R->set_k(1, K_);
    R->set_p(2, N_);
    N_->parent = R;
    tree.root = R;
    return;
  }
  Node *P = n->parent;
  if(P->has_less_than_n_pointers()){
    size_t index = P->highest_key_index_less_than_or_equal_to_arg(K_);
    for(size_t ki = P->size-1; ki >= index+1; --ki){
      P->set_k(ki+1, P->k(ki));
    }
    for(size_t pi = P->size; pi >= index+2; --pi){  //index + 2 ?
      P->set_p(pi+1, P->p(pi));
    }
    P->set_k(index+1, K_);
    P->set_p(index+2, N_);
    N_->parent = P;
  }else{
    Node *T = P->clone();
    T->add_size();

    size_t index = T->highest_key_index_less_than_or_equal_to_arg(K_);
    for(size_t ki = index+1; ki <= T->size - 1; ++ki){
      T->set_k(ki + 1, T->k(ki));
    }
    for(size_t pi = index+2; pi <= T->size - 1; ++pi){
      T->set_p(pi + 1, T->p(pi));
    }
    T->set_k(index + 1, K_);
    T->set_p(index + 2, N_);

    N_->parent = T;
    P->erase_p_and_k();
    Node *P_ = new Node;
    for(size_t i = 1; i <= ceil((N+1)/2)-1; ++i){
      P->set_p(i, T->p(i));
      P->set_k(i, T->k(i));
    }
    P->set_p(ceil((N+1)/2), T->p(ceil((N+1)/2)));
    Key K__ = T->k(ceil((N + 1) / 2));
    size_t offset = ceil((N+1)/2);
    for(size_t i = ceil((N + 1) / 2)+1; i <= N; ++i){
      P_->set_p(i - offset, T->p(i));
      P_->set_k(i - offset, T->k(i));
    }
    P_->set_p(N+1 - offset, T->p(N+1));
    insert_in_parent(P, K__, P_);
  }
}

void insert(Key K, Node *P){
  Node *L;
  if(tree.is_empty()){
    L = new Node;
    tree.root = L;
  }else{
    L = search_leaf(tree.root, K);
  }

  if(L->is_not_full()){
    insert_in_leaf(L, K, P);
  }else{
    Node *L_ = new Node;
    Node *T = L->clone();
    T->add_size();
    insert_in_leaf(T, K, P);
    L_->set_p(N, L->p(N));
    L->set_p(N, L_);
    L->erase_partly();
    for(size_t i = 1; i <= ceil(N / 2); ++i){
      L->set_p(i, T->p(i));
      L->set_k(i, T->k(i));
    }
    for(size_t i = ceil(N / 2) + 1; i <= N; ++i){
      size_t offset = ceil(N / 2);
      L_->set_p(i - offset, T->p(i));
      L_->set_k(i - offset, T->k(i));
    }
    Key K_ = L_->smallest_key();
    insert_in_parent(L, K_, L_);
  }
}



int main(){
  // rule
  // - no OOP?
  // - int key, void pointer
  Node dog;
  dog.value = "dog";
  Node cat;
  cat.value = "cat";
  Node wolf;
  wolf.value = "wolf";
  Node god;
  god.value = "god";

  insert(3, &dog);
  insert(2, &cat);
  insert(5, &wolf);
  insert(4, &dog);

  //printf("%s",  tree.root->to_string());
}