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
#include <limits>
//#define NDEBUG
#include <assert.h>
using namespace std;
#ifndef NDEBUG
    #define debug(x) cerr<<#x<<"=\""<<x<<"\""<<" at line#"<<__LINE__<<endl;
    #define hline() cerr<<"-----------------------------------------"<<endl;
#else
    #define debug(x)
    #define hline()
#endif
typedef long long int llint;
template <int Len,typename T=int>
class range_maxer_t {
  T _tree[Len];
  inline int lowbit(int x) {
    return (x^(x-1))&x;
  }
public:
  range_maxer_t() {
    for(int i=0;i<Len;i++) {
      _tree[i]=numeric_limits<T>::min();
    }
  }
  void update(int pos,T v) {
    assert(pos>=0);
    assert(pos<Len);
    while(pos<Len) {
      _tree[pos] =max(_tree[pos],v);
      pos+=lowbit(pos+1);
    }
  }
  T query(int right_bound) {
    assert(right_bound<Len);
    T res=numeric_limits<T>::min();
    while(right_bound>=0) {
      res=max(res,_tree[right_bound]);
      right_bound-=lowbit(right_bound+1);
    }
    return res;
  }
};
