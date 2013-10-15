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
//#define NDEBUG
#include <cassert>
#include <limits>
//INT_MAX INT_MIN
#include <limits.h>
#include <cstdarg>
#include <iterator>
#include <valarray>
using namespace std;
#define debug(x) cerr<<#x<<"=\""<<x<<"\""<<" at line#"<<__LINE__<<endl;
#define Rep(i,n) for(int i=0;i<(n);++i)
#define For(i,a,b) for(typeof(a) i=a;i<(b);++i)
#define SZ(vec) ((int)(vec).size())
#define All(vec) (vec).begin(),(vec).end()

class Base {
 public:
  void print() {
    debug("Base");
  }
  virtual ~Base() {
    debug("~Base");
  }
 private:
  int v_;
};

class Derived : public Base {
 public:
  virtual void print() {
    debug("Derived");
  }
  ~Derived() {
    debug("~Derived");
  }
 private:
  int v_;
};

class Derived1 : public Derived {
 public:
  virtual void print() {
    debug("Derived1");
  }
  ~Derived1() {
    debug("~Derived1");
  }
};

int main() {
  Derived* ptr = new Derived1();
  ptr->print();
  Base *bptr = ptr;
  bptr->print();
  delete ptr;
  return 0;
}
