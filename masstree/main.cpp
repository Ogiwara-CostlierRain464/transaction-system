#include <iostream>
#include <string>
#include "sample.h"

using namespace std;

std::string ver_string(int a,int b, int c){
  string s;
  s += (to_string(a) + "." + to_string(b) + "." + to_string(c));
  return s;
}

void check(){
  const char *true_cxx =
#ifdef __clang__
  "clang";
#else
  "g++";
#endif

  std::string true_cxx_ver =
#ifdef __clang__
    "clang";
#else
  ver_string(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#endif

  printf("Compiler: %s, %s\n", true_cxx, true_cxx_ver.c_str());
}

int main(){
  check();

  auto tree = sample2();
  auto b = tree->findBorder(0);


  printf("Hello");
}
