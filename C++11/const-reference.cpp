
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

vector<int> Foo() {
  return vector<int>{1, 2, 3};
}
struct B {
  const string& aa;
  B(const string& a)  : aa(a)
  { cout << "Constructing B @" << this << endl; }
  ~B() { cout << "Destructing B @" << this << endl;}
};

struct A {
  const B& b;
  A(const B& bb) : b(bb)
  { cout << "Constructing A @" << this << endl; }
  ~A() { cout << "Destructing A @" << this << endl;}
};

void foo(const B& b) {
  B bb("adad");
  cout << b.aa << endl;
}

int main() {
  A a(B("afdsafasfd"));
  cout << a.b.aa << endl;
  cout << &a.b << endl;
  foo(B("12345"));
  return 0;
}
