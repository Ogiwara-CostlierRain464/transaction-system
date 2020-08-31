#include <iostream>
#include "sample.h"

int main(){
  auto tree = sample2();
  auto b = static_cast<BorderNode*>(tree->findBorder(0));


  printf("Hello");
}