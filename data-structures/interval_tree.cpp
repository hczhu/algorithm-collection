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
#include <cmath>
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

// Use the fractional cascading technique
template<int N,int L=(int)(ceil(log(1.0*N)/log(2.0))),typename T_key=int,typename T_value=int>
class interval_tree_t
{
public:
//  typedef int T_value;
//  typedef int T_key;
private:
  //static const int L=17;
  //static const int N=100000;
  static const int B=5;
//  const int L=(int)log(1.0*N);
private:
  const T_key* _key;
  const T_value* _value; 
  int sorted_index[L][N];
  T_value prefix_sum[L][N];
  int right_sum[L][N];
  int n;
  void build_tree_helper(int level,int low,int high);
public:
  interval_tree_t():_key(NULL),_value(NULL),n(0){};
  void build_tree(int size,const T_key key[],const T_value value[]);
  
  // Return the sum of _value[i] such that _key[i]<=key_upper and i<=end_index
  T_value query(int end_index,const T_key& key_upper)const;

  // Return the sum of _value[i] such that _key[i]<=key_upper and start_index<=i<=end_index
  T_value query(int start_index,int end_index,const T_key& key_upper)const
  {
    T_value res=query(end_index,key_upper);
    if(start_index)res-=query(start_index-1,key_upper);
    return res;
  };
  
  int bsearch_key(const T_key& key_upper)const;
};

template<int L,int N,typename T_key,typename T_value>
int interval_tree_t<L,N,T_key,T_value>::bsearch_key(const T_key& key_upper)const
{
  const int* sindex=sorted_index[0];
  if(_key[sindex[0]]>key_upper)return -1;
  int low=0,high=n-1;
  while(low<high)
  {
    int mid=1+((low+high)>>1);
    if(_key[sindex[mid]]<=key_upper)low=mid;
    else high=mid-1;
  }
  return low;
}
// Return sum
template<int L,int N,typename T_key,typename T_value>
T_value interval_tree_t<L,N,T_key,T_value>::query(int end_index,const T_key& key_upper)const
{
  int pos=bsearch_key(key_upper);
  if(pos<0)return (T_value)0;
  int level=0,low=0,high=n-1;
  T_value res=(T_value)0;
  for(;pos>=0;)
  {
    //const T_value *psum=prefix_sum[level]+low;
    const int* rsum=right_sum[level]+low;
    int n=high+1-low;
    assert(pos>=0&&pos<n);
    if(end_index==high)
    {
      res+=prefix_sum[level][pos+low];
      break;
    }
    if(n<=B)
    {
      const int *sindex=sorted_index[level]+low;
      for(int i=0;i<=pos;i++)
        if(sindex[i]<=end_index)res+=_value[sindex[i]];
      break;
    }
    int mid=(low+high)>>1;
    int right_link=rsum[pos]-1;
    int left_link=pos-rsum[pos];

    assert(left_link>=-1);
    assert(right_link>=-1);

    if(end_index<=mid)
    {
      high=mid;
      pos=left_link;
    }
    else
    {
      if(left_link!=-1)res+=prefix_sum[level+1][low+left_link];
      low=mid+1;
      pos=right_link;
    }
    level++;
/*    hline();
    debug(level);
    debug(low);
    debug(high);
    debug(pos);*/
  }
  return res;
}

template<int L,int N,typename T_key,typename T_value>
void interval_tree_t<L,N,T_key,T_value>::build_tree_helper(int level,int low,int high)
{
  assert(level<L);
  int *sindex=sorted_index[level]+low;
  T_value *psum=prefix_sum[level]+low;
  int* rsum=right_sum[level]+low;
  int n=high+1-low;
  if(n<=B)
  {
    for(int i=low;i<=high;i++)sindex[i-low]=i;
    for(int i=0;i<n;i++)
    for(int j=i;j&&_key[sindex[j]]<_key[sindex[j-1]];j--)
      swap(sindex[j],sindex[j-1]);

    psum[0]=_value[sindex[0]];
    for(int i=1;i<n;i++)psum[i]=psum[i-1]+_value[sindex[i]];
    return;
  }
  //merge
  int mid=(low+high)>>1;
  build_tree_helper(level+1,low,mid);
  build_tree_helper(level+1,mid+1,high);
  int lsize=mid+1-low;
  int rsize=n-lsize;
  int *lindex=sorted_index[level+1]+low;
  int *rindex=sorted_index[level+1]+mid+1;
  for(int ltop=0,rtop=0,top=0;ltop<lsize||rtop<rsize;)
  {
    if(rtop==rsize)sindex[top++]=lindex[ltop++];
    else if(ltop==lsize)sindex[top++]=rindex[rtop++];
    else if(_key[lindex[ltop]]<_key[rindex[rtop]])sindex[top++]=lindex[ltop++];
    else sindex[top++]=rindex[rtop++];
  }
  psum[0]=_value[sindex[0]];
  rsum[0]=(sindex[0]>mid)?1:0;
  for(int i=1;i<n;i++)
  {
    psum[i]=psum[i-1]+_value[sindex[i]];
    rsum[i]=rsum[i-1]+(sindex[i]>mid?1:0);
  }
/*  hline();
  debug(level);
  debug(low);
  debug(high);
  for(int i=0;i<n;i++)cerr<<sindex[i]<<" ";cerr<<endl;
  for(int i=0;i<n;i++)cerr<<psum[i]<<" ";cerr<<endl;
  for(int i=0;i<n;i++)cerr<<rsum[i]<<" ";cerr<<endl;*/
}
template<int L,int N,typename T_key,typename T_value>
void interval_tree_t<L,N,T_key,T_value>::build_tree(int size,const T_key key[],const T_value value[])
{
  n=size;
  _key=key;
  _value=value;
//  memcpy(_key,key,sizeof(T_key)*n);
//  memcpy(_value,value,sizeof(T_value)*n);
  build_tree_helper(0,0,n-1);
}
const int key[]={   2, 1, 4, 3, 6, 8, 7, 5,10, 9,16,11,14,13,12,15};
const int value[]={-1, 0, 1, 2, 3,-1, 3, 4, 4, 3,-9, 5, 8, 2, 1, 9};
interval_tree_t<16> tree;
int main()
{
  tree.build_tree(16,key,value);
  assert(tree.query(3,6,6)==5);
  assert(tree.query(3,10,9)==14);
  assert(tree.query(11,11)==23);
  return 0;
}
