
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <iostream>
#include <cmath>
#include <vector>
#include <queue>
#include <set>
#include <sstream>
#include <map>
#include <cstring>
#include <complex>
#include <numeric>
#include <functional>
#include <cassert>
#include <limits>
#include <limits.h>
#include <cstdarg>
#include <iterator>
#include <valarray>
#include <thread>
#include <memory>
using namespace std;
#define debug(x) cerr<<#x<<"=\""<<x<<"\""<<" at line#"<<__LINE__<<endl;

class A {
public:
  void constFoo() const {
  }
  void nonconstFoo() {
  }
};

class D {
 public:
  void operator()(A* a) {}
};

int main() {
  A a;
  const A* ptr1 = &a;
  // ptr1->nonconstFoo(); Won't compile.
  const unique_ptr<A, D> ptr2(&a, D());
  ptr2->nonconstFoo();
  using Aptr = A*;
  const Aptr ptr3 = &a;
  ptr3->nonconstFoo();  //interesting part.
  vector<unique_ptr<A>> vec;
  vec.emplace_back(new A());
  vec.emplace_back(new A());
  vec.emplace_back(new A());
  sort(vec.begin(), vec.end());
  return 0;
}
