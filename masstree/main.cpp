#include "sample.h"
#include "verify.h"

int main(){
  verify_struct_property();
  auto tree = sample2();
  auto b = tree->findBorder(0);
  printf("0x%08llx", b->key_slice[0]);
}
