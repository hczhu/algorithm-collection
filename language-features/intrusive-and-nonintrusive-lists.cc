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

class Link {
  public:
    Link(Link* next = NULL) : next_(next) {}
    Link*& Next() {
      return next_;
    }
  protected:
    Link *next_;
};

class IntrusiveList {
  public:
    void AddToFront(Link* node) {
      node->Next() = head_.Next();
      head_.Next() = node;
    }
    Link* Start() {
      return head_.Next();
    }
  private:
    Link head_;
};
class Node : public Link {
  public:
    Node(int v = 0) : data_(v) {}
  private:
    int data_;
};

template<class T>
class NonIntrusiveList {

  class InnerNode : public Link {
    public:
      InnerNode(const T* user_data) : user_data_(user_data) {}
      const T* user_data_;
  };
  IntrusiveList list_;
  public:
    void AddToFront(const T& new_data) {
      list_.AddToFront(new InnerNode(&new_data));
    }
};
#define CHECK_DERIVE(x, y) do { y* _tmp_y = (x*)(0); _tmp_y = NULL;} while(0);
int main() {
  CHECK_DERIVE(Node, Link);
  const int n=10000000;
  double start = 1.0 * clock() / CLOCKS_PER_SEC;
  IntrusiveList intrusive_list;
  Rep(i, n) {
    intrusive_list.AddToFront(new Node(i));
  }
  cerr << "Intrusive Time = " << 1.0 *clock() / CLOCKS_PER_SEC - start << " s" << endl;
  start = 1.0 * clock() / CLOCKS_PER_SEC;
  NonIntrusiveList<int> nonintrusive_list;
  Rep(i, n) {
    nonintrusive_list.AddToFront(*(new int(i)));
  }
  cerr << "Non-intrusive Time = " << 1.0 *clock() / CLOCKS_PER_SEC - start << " s" << endl;
  return 0;
}
