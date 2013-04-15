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
#include <assert.h>
#include <time.h>
using namespace std;
#include "data_structures_common.h"
#include "mem_pool.h"
#include "hash_table.h"
#include "linked_list.h"

class HashFunc
{
  public:
   inline void operator()(int a,int b)
   {
     cerr<<a+b<<endl;
   };
};
/*
template <typename T>
struct A_t
{
  T a;
  struct B_t
  {
    B_t(){debug("B_t");};
  };
};
*/

template<typename T1,typename T2=int>
struct A_t
{
  T1 a;
  T2 b;
  A_t(){puts("A_t constructor");};
};
template<class T>
void foo(T t)
{
  typename T::B_t  b;
  cerr<<t.a<<endl;
//  typeof(t.a) b=t.a;
//  cerr<<b<<endl;
}

int main()
{
//  A_t<int> a;
//  A_t<int>::B_t b;
  //a.a=100;
  //foo(a);  
  A_t<int> a;
  return 0;
}
