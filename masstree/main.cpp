#include "sample.h"
#include "verify.h"

int main(){
  verify_struct_property();
  auto tree = sample3();
  auto b = tree->findBorder(0x01234567);
  printf("%llx", b->key_slice[0]);
}
