#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm>
#include <memory.h>
#define NDEBUG
#include <assert.h>
#include <vector>
#include <iostream>
using namespace std;
#define EPS 1e-11
#ifndef NDEBUG
    #define debug(x) cerr<<#x<<"=\""<<x<<"\""<<" at line#"<<__LINE__<<endl;
    #define hline() cerr<<"-----------------------------------------"<<endl;
#else
    #define debug(x)
    #define hline()
#endif

//#define OUTPUT
/*
The basic form of Linear Programming is
Max cx
s.t.  x>=0 &&  A*x=b  
*/
template<int N,int M>
class LPSimplex
{
  void printAll(double ma[][N],double vb[],double vc[],int m,int n,int basic[])
  {
    puts("-------ma and vb---------------");
    for(int i=0;i<m;printf("=%lf\n",vb[i++]),assert(vb[i-1]>-EPS))
    for(int j=0;j<n;j++)
      printf("%lf ",ma[i][j]);
    puts("-------------basic-----------");
    for(int i=0;i<n;i++)
      printf("%d ",basic[i]);
    puts("");
    puts("---------------vc-------------");
    for(int i=0;i<n;i++)
      printf("%lf ",vc[i]);
    puts("");
  }
  const static int ITIMES=1000;
  void pivot(double ma[][N],double vb[],double vc[],int m,int n,int basic[],int leave,int enter)
  {
#ifdef OUTPUT
    printf("---pivot %d--->%d\n",leave,enter);
#endif
    assert(basic[leave]!=-1);
    assert(basic[enter]==-1);
    int ind=basic[leave];
    basic[enter]=ind;
    basic[leave]=-1;
    double t=ma[ind][enter];
    assert(fabs(t)>EPS);
    vb[ind]/=t;
    for(int j=0;j<n;j++)ma[ind][j]/=t;
    for(int i=0;i<m;i++)
    {
      if(i==ind)continue;
      t=-ma[i][enter];
      for(int j=0;j<n;j++)
        ma[i][j]+=t*ma[ind][j];
      vb[i]+=t*vb[ind];
    }
    t=-vc[enter];
    for(int j=0;j<n;j++)vc[j]+=t*ma[ind][j];
#ifdef OUTPUT
    printAll(ma,vb,vc,m,n,basic);
#endif
  }
  bool simplex(double ma[][N],double vb[],double vc[],int m,int n,int basic[])
  {
    int *represent=new int[n];
    memset(represent,0xff,sizeof(represent));
    for(int i=0;i<n;i++)
      if(basic[i]!=-1)
        represent[basic[i]]=i;
    for(int t=0;t<ITIMES;t++)
    {
      int enter;
      for(enter=0;enter<n;enter++)
        if(basic[enter]==-1&&vc[enter]>EPS)
          break;
      if(enter==n)break;
      int leave=-1;
      double upper=1e20;
      for(int i=0;i<m;i++)
      {
        int ind=represent[i];
        if(ind==-1)continue;
        if(ma[i][enter]>EPS)
        {
          double tt=vb[i]/ma[i][enter];
          if(leave==-1)
            leave=ind,upper=vb[i]/ma[i][enter];
          else if(tt+EPS<upper||(fabs(tt-upper)<EPS&&leave>ind))
            leave=ind,upper=vb[i]/ma[i][enter];
        }
      }
      if(leave==-1)
      {
        delete [] represent;
        return false;
      }
#ifdef OUTPUT
      printf("leave%d enter%d\n",leave,enter);
#endif
      represent[basic[leave]]=enter;
      pivot(ma,vb,vc,m,n,basic,leave,enter);
    }
    delete [] represent;
    return true;
  }
  /*
     Maximze:   vc[] * X
     s.t.
      ma[][] * X[] = vb[]
      X[] >= 0

      solution: 
  */

  bool initSolution(double ma[][N],double vb[],int m,int n,int basic[])
  {
    for(int i=0;i<n;i++)basic[i]=-1;
    for(int i=n;i<n+m;i++)basic[i]=i-n;
    for(int i=0;i<m;i++)
    {
      for(int j=n;j<n+m;j++)ma[i][j]=0.0;
      ma[i][i+n]=1.0;
      ma[i][n+m]=-1.0;
    }
    if(*min_element(vb,vb+m)>-EPS)return true;
    double *vc=new double[m+n+1];
    memset(vc,0,sizeof(vc));
    vc[n+m]=-1.0;basic[n+m]=-1;
    int leave,enter=n+m;
    leave=(int)(min_element(vb,vb+m)-vb)+n;
    assert(vb[basic[leave]]<-EPS);
    pivot(ma,vb,vc,m,n+m+1,basic,leave,enter);
    bool f=simplex(ma,vb,vc,m,n+m+1,basic);
    assert(f);
    double sum;
    if(basic[n+m]==-1)sum=0;
    else sum=-vb[basic[n+m]];
    if(sum<-EPS)
    {
      delete [] vc;
      return false;
    }
    if(basic[n+m]!=-1)
    {
      int enter;
      int ind=basic[n+m];
      for(enter=0;enter<n+m;enter++)
        if(fabs(ma[ind][enter])>EPS)
          break;
      if(enter<n+m)pivot(ma,vb,vc,m,n+m+1,basic,n+m,enter);
    }
    delete [] vc;
    return true;
  }
  int LP(double ma[][N],double vb[],double vc[],int m,int n,int basic[])
  {
    if(!initSolution(ma,vb,m,n,basic))return -1;
    //return 0;
    for(int i=n;i<n+m;i++)vc[i]=0.0;
    for(int i=0;i<n+m;i++)
    {
      if(basic[i]!=-1&&fabs(vc[i])>EPS)
      {
        double t=-vc[i];
        int ind=basic[i];
        for(int j=0;j<n+m;j++)
          vc[j]+=t*ma[ind][j];
        assert(fabs(vc[i])<EPS);
      }
    }
#ifdef OUTPUT
    puts("------------init solution---------");
    printAll(ma,vb,vc,m,n+m,basic);
#endif
    if(!simplex(ma,vb,vc,m,n+m,basic))return 1;
    return 0;
  }
};



