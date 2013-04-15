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
using namespace std;
struct node_t
{
  vector<double> vec;
  double ans;
  int order;
};

#define D
#ifdef A
#include "parallel_framework.h"
  parallel_framework_t<node_t> para;
#endif
#ifdef B
#include "parallel_framework_semaphore.h"
  parallel_framework_semaphore_t<node_t> para;
#endif
#ifdef C
#include "parallel_framework_lock.h"
  parallel_framework_lock_t<node_t> para;
#endif
#ifdef D
#include "parallel_framework_buffer.h"
  parallel_framework_buffer_t<node_t> para;
#endif
int generator(node_t& node,uint32_t idx)
{
  static int order=0;
  node.order=++order;
  int len=rand()%1000+1;
  for(int i=0;i<len;i++)
  {
    node.vec.push_back(1.0*rand()/1000.0);
  }
  return order<10000?0:1;
}
int processor(node_t& node,uint32_t idx)
{
  node.ans=0.0;
  for(int i=0;i<node.vec.size();i++)
  {
    node.ans+=sqrt(node.vec[i]);
  }
  return 0;
}
int consumer(node_t& node,int ret)
{
  static int order=0;
  assert(ret==0);
  order++;
  assert(node.order==order);
  return 0;
}
int main()
{
  para.create(2,100);
  para.run(generator,processor,consumer);
  return 0;
}
