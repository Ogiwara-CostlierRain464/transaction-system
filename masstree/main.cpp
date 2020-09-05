#include "sample.h"
#include "verify.h"

struct S {
  // will usually occupy 2 bytes:
  // 3 bits: value of b1
  // 2 bits: unused
  // 6 bits: value of b2
  // 2 bits: value of b3
  // 3 bits: unused
  //
  unsigned char b1 : 3, : 2, b2 : 6, b3 : 2;
};

int main(){
  verify_union_size();
  auto tree = sample2();
  auto b = tree->findBorder(0);
}
