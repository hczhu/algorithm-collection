/*
Author: Hongcheng Zhu (zhuhcheng@gmail.com)
Date:  September 19 2009

SMAWK algorithm
Find row minimal for a totally monotone matrix A[][] in linear time.
Matrix A[][] is totally monotone iff for any 0<=i1<i2<n, 0<=j1<j2<m, 
A[i2][j1]<[i2][j2] implies A[i1][j1]<A[i1][j2].
The algorithm first delete some columns to reduce the number of column to not greater than the number of rows,
using the following two facts:
1. If A[a][a]<A[a][a+1], then for any 0<=i<=a A[i][a]<A[i][a+1], so the elements A[i][a+1]s are all "dead".
2. If A[a][a]>=A[a][a+1], then for any a<=i<n A[i][a]>=A[i][a+1], so the elements A[i][a]s are all "dead".
Then it partitions the row into two parts even rows and odd rows, and recursively calculates the row minima of even rows.
*/
#include <vector>
#include <iostream>
//#define NDEBUG
#include <cassert>
using namespace std;

template<typename T>
void SMAWK(int n,int inc,vector<int> col,int row_minima[],T (*lookup)(int,int))
{
  const int row_size=(n+inc-1)/inc;
  vector<int> sub_col;
  for(int i=0;i<col.size();i++)
  {
    while(sub_col.size()&&lookup(inc*(sub_col.size()-1),sub_col.back())
                >=lookup(inc*(sub_col.size()-1),col[i]))
      sub_col.pop_back();
    if(sub_col.size()<row_size)sub_col.push_back(col[i]);
  }
  col=sub_col;
  assert(col.size()<=row_size);
  assert(col.size());
  if(row_size==1)
  {
    row_minima[0]=col[0];
    return;
  }
  SMAWK(n,inc<<1,col,row_minima,lookup);
  for(int i=inc,c=0;i<n;i+=2*inc)
  {
    int pre=row_minima[i-inc];
    int next=(i+inc<n)?row_minima[i+inc]:col.back();
    while(c<col.size()&&col[c]<pre)c++;
    assert(c<col.size()&&col[c]>=pre);
    assert(col[c]<=next);
    int& res=row_minima[i];
    res=col[c];
    while(c<col.size()&&col[c]<=next)
    {
      if(lookup(i,col[c])<=lookup(i,res))res=col[c];
      c++;
    }
    c--;
  }
}
template<typename T>
void SMAWK(int n,int m,int row_minima[],T (*lookup)(int,int))
{
  vector<int> col(m);
  for(int i=0;i<m;i++)col[i]=i;
//  hline();
//  for(int i=0;i<n;i++,cerr<<endl)
//  for(int j=0;j<m;j++)cerr<<lookup(i,j)<<" ";
//  hline();
  SMAWK(n,1,col,row_minima,lookup);
//  for(int i=0;i<n;i++)
//  {
//    for(int j=0;j<m;j++)
//      assert(lookup(i,row_minima[i])<=lookup(i,j));
//  }
}

