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
#include <stack>
#include <stdarg.h>
#include <limits.h>
//#define NDEBUG
#include <assert.h>
using namespace std;
#define debug(x) cerr<<#x<<"=\""<<x<<"\""<<" at line#"<<__LINE__<<endl;
#define Rep(i,n) for(int i=0;i<(n);++i)
#define For(i,a,b) for(typeof(a) i=a;i<(b);++i)
#define SZ(vec) ((int)(vec).size())
#define All(vec) (vec).begin(),(vec).end()

class Arena {
  public:
    void *get_mem(unsigned int num) {
      return new char[num];
    };
};
class Base {
  public:
    virtual int foo() {
      return 0;
    };
    void* operator new(size_t num) {
      debug(num);
      return ::new char[num]; 
    };
    Base(int x=-1) : x_(x){
      debug(x);
    };
  private:
    int x_;
};

class Derived : public Base {
  public:
  void* operator new(size_t num, Arena& mem) {
    debug(num);
    return mem.get_mem(num);
  }
  Derived(int x) : Base(x) {
    debug(x);
  }
};
class Simple {
  int a;
};
int main() {
  Arena pool;
  Base* ptr=new Base(100);
  Base* derived=new(pool) Derived(200);
  // placement new
  char buf[sizeof(Simple)];
  Simple* fake_new_ptr=new(buf) Simple();
  return 0;
}
