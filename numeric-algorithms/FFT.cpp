#include <assert.h>
#include <cmath>
#include <vector>
#include <complex>
#include <algorithm>
#include <memory.h>
#include <iostream>
using namespace std;

#define two(x)  (1<<(x))
#define low(x)  (((x)^((x)-1))&(x))
void getReverse(int n,int rev[])
{
  assert(low(n)==n);
  rev[0]=0;
  rev[n-1]=n-1;
  int m;
  for(m=0;two(m)<n;m++);
  int last=m-1,pre=0;
  n--;
  for(int i=1;i<n;i++)
  {
    pre=((two(last)-1)&pre)^two(last);
    if(last<m-1)last=m-1;
    else for(last--;pre&two(last);last--);
    rev[i]=pre;
  }
}
typedef complex<double> dcomp;
void DFFT(dcomp input[],int n,int rev[]=NULL,bool invert=false)
{
  const double PI=acos(-1.0);
  assert(low(n)==n);
  dcomp w(cos(2*PI/n),sin(2*PI/n));
  if(invert)w.imag()=-w.imag();
  bool allocate=false;
  if(rev==NULL)
  {
    allocate=true;
    rev=new int[n];
    getReverse(n,rev);
  }
  for(int i=0;i<n;i++)if(rev[i]>i)swap(input[rev[i]],input[i]);
  dcomp wns;
  for(int s=1;s<n;s<<=1)
  {
    wns=w;
    for(int ns=n/s;ns>2;ns>>=1)wns*=wns;
    for(int b=0,ss=s<<1;b<n;b+=ss)
    {
      dcomp ww(1.0);
      for(int j=0;j<s;j++)
      {
        dcomp even=input[b+j];
        dcomp odd=ww*input[b+j+s];
        input[b+j]=even+odd;
        input[b+j+s]=even-odd;
        ww*=wns;
      }
    }
  }
  if(allocate)delete [] rev;
}

//Array buffer[] must be large enough
template<typename T>
void polynomialMultiply(T xx[],T yy[],int n,T res[],T buffer[])
{
  memset(res,0,sizeof(T)*(2*n-1));
  if(n<=100)
  {
    for(int i=0;i<n;i++)
    {
      if(0==yy[i])continue;
      for(int j=0;j<n;j++)
        res[i+j]+=xx[j]*yy[i];
    }
    return;
  }
  const int a=n/2;
  const int hsize=2*(n-a)-1,lsize=2*a-1;
  polynomialMultiply(xx,yy,a,res,buffer);
  polynomialMultiply(xx+a,yy+a,n-a,res+2*a,buffer);
  for(int i=0;i<a;i++)xx[i+a]+=xx[i],yy[i+a]+=yy[i];
  polynomialMultiply(xx+a,yy+a,n-a,buffer,buffer+hsize);
  for(int i=0;i<a;i++)xx[i+a]-=xx[i],yy[i+a]-=yy[i];
  
  for(int i=0;i<lsize;i++)buffer[i]-=res[i];
  for(int i=0;i<hsize;i++)buffer[i]-=res[i+2*a];
  for(int i=0;i<hsize;i++)res[i+a]+=buffer[i];
}
