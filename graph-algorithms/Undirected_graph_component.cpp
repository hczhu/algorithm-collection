#include <stdio.h>
#include <algorithm>
#include <memory.h>
#include <vector>
#define NDEBUG
#include <assert.h>
using namespace std;
//Biconnected components for undirected graphs
// Also works for non simple graph
namespace twoBlockTree
{
//Used to  finding 2-edge connected components
//#define twoEdgeConnectedComponents
#define N 1000
#define M (N*N/2)
int begin[N],edge[2*M];
int etop[N];
int cnt[N+1];
int aa[2*M],bb[2*M];
bool edgeUsed[M];
void forwardStar(int n,int m)
{
  memset(cnt,0,sizeof(int)*(n+1));
  for(int i=0;i<m;i++)cnt[1+aa[i]]++;
  for(int i=1;i<=n;i++)cnt[i]+=cnt[i-1];
  for(int i=0;i<n;i++)
    begin[i]=cnt[i],etop[i]=cnt[i+1];
  for(int i=0;i<m;i++)
    edge[--cnt[aa[i]+1]]=i;
}

int stack[N];
int buffer[N];
int low[N];
int flag[N];
void biconnectedComponents(int n,int m,int end1[],int end2[],vector<vector<int> >& components)
{
  components.clear();
  memset(edgeUsed,false,sizeof(bool)*m);
  for(int i=0;i<m;i++)
    aa[i<<1]=end1[i],bb[i<<1]=end2[i],aa[(i<<1)^1]=end2[i],bb[(i<<1)^1]=end1[i];
  forwardStar(n,m*2);
  memset(flag,0,sizeof(int)*n);
  for(int s=0;s<n;s++)
  {
    if(flag[s])continue;
    if(begin[s]==etop[s])
    {
      components.push_back(vector<int>(1,s));
      continue;
    }
    int top=0,out=0,ff=1;
    stack[top++]=s;
    flag[s]=ff++;
    buffer[out++]=s;
    while(top)
    {
      int v=stack[--top];
      int u;
      while(begin[v]<etop[v]&&(edgeUsed[edge[begin[v]]>>1]||flag[u=bb[edge[begin[v]]]]))
      {
        if(!edgeUsed[edge[begin[v]]>>1])
          edgeUsed[edge[begin[v]]>>1]=true,low[v]=min(flag[u],low[v]);
        begin[v]++;
      }
      if(begin[v]==etop[v])
      {
  #ifdef OUTPUT
        printf("Out stack %d low=%d\n",v,low[v]);
  #endif
#ifndef twoEdgeConnectedComponents
        if(top==0)continue;
        int f=stack[top-1];
        if(f==-1)continue;
        if(low[v]>=flag[f])
        {
          vector<int> com;
          com.push_back(f);
          while(flag[buffer[out-1]]>=flag[v])
            com.push_back(buffer[--out]);
          components.push_back(com);
          #ifndef NDEBUG
          printf("-------------------------\nFind 2-block root=%d\n",f);
          for(int i=0;i<com.size();i++)
            printf("%d ",com[i]);
          puts("");
          #endif
        }
#else
        int f;
        if(top==0||low[v]>flag[f=stack[top-1]])
        {
          vector<int> com;
          while(out&&flag[buffer[out-1]]>=flag[v])
            com.push_back(buffer[--out]);
          components.push_back(com);
          #ifndef OUTPUT
          printf("-------------------------\n\\
              Find 2-edge connected components root=%d\n",v);
          for(int i=0;i<com.size();i++)
            printf("%d ",com[i]);
          puts("");
          #endif
        }
#endif
        else low[f]=min(low[v],low[f]);
        continue;
      }
      edgeUsed[edge[begin[v]]>>1]=true;
      u=bb[edge[begin[v]++]];
      stack[top++]=v;
      stack[top++]=u;
      buffer[out++]=u;
      flag[u]=ff++;
      low[u]=flag[u];
#ifdef OUTPUT
      printf("Visit %d flag %d\n",u,flag[u]);
#endif
    }  
  }
}
};
