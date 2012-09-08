#include <stdio.h>
#include <memory.h>
#include <algorithm>
//#define NDEBUG
#include <vector>
#include <assert.h>
#include <limits>
#include <numeric>
using namespace std;
template<typename T>
struct trait_t {};
template<> struct trait_t<int>{const static int inf=1000000000;};
template<> struct trait_t<long long int>{const static long long int inf=1000000000000000000LL;};
template<int N,typename Cost,Cost INF=numeric_limits<Cost>::max()>
struct bipartite_maximum_weighted_matching
{
Cost llabel[N],rlabel[N];
Cost (*weight)[N];
int rpred[N];
Cost delta[N];
int lpred[N];
int rmatch[N],lmatch[N];
int qq[2*N];
bool lset[N];
void augment(int s,int v,int u)
{
  rmatch[u]=v;
  int a,b;
  while(v!=s)
  {
    a=lmatch[v];lmatch[v]=u;
    b=lpred[v];
    assert(rmatch[a]==v);
    rmatch[a]=b;
    v=b;u=a;
  }
  lmatch[v]=u;
}
bool BFS(const int s,int n,int m)
{
  int head=0,tail=0;
  memset(lset,false,sizeof(bool)*n);
  for(int i=0;i<m;i++)delta[i]=INF;
  qq[tail++]=s;lset[s]=true;
  for(;;)
  {
    while(head<tail)
    {
      int v=qq[head++];
      for(int u=0;u<m;u++)
      {
        Cost t=llabel[v]+rlabel[u]-weight[v][u];
        assert(t>=0);
        if(t<delta[u])delta[u]=t,rpred[u]=v;
        if(t)continue;
        if(rmatch[u]==-1)
        {
          augment(s,v,u);
          return true;
        }
        if(lset[rmatch[u]])continue;
        lset[rmatch[u]]=true;
        lpred[rmatch[u]]=v;
        qq[tail++]=rmatch[u];
      }
    }
    Cost del=INF;
    for(int i=0;i<m;i++)
      if(delta[i]&&del>delta[i])del=delta[i];
    assert(del>0&&del<INF);
    for(int i=0;i<n;i++)
      if(lset[i])llabel[i]-=del;
    for(int i=0;i<m;i++)
    {
      assert(delta[i]<INF&&delta[i]>=0);
      if(delta[i]==0)rlabel[i]+=del;
      else
      {
        delta[i]-=del;
        if(delta[i]==0)qq[tail++]=i;
      }
    }
    for(int i=head;i<tail;i++)
    {
      if(rmatch[qq[i]]==-1)
      {
        augment(s,rpred[qq[i]],qq[i]);
        return true;
      }
      int v=rmatch[qq[i]];
      lset[v]=true;
      lpred[v]=rpred[qq[i]];
      qq[i]=v;
    }
  }
  return false;
}
Cost match(int n,int m,Cost cost[][N])
{
  while(n>m) {
    assert(m<N);
    for(int i=0;i<n;i++)cost[i][m]=0;
    m++;
  }
  weight=cost;
  memset(rmatch,0xff,sizeof(int)*m);
  memset(lmatch,0xff,sizeof(int)*n);
  memset(rlabel,0,sizeof(rlabel[0])*m);
  for(int i=0;i<n;i++)
    llabel[i]=*max_element(weight[i],weight[i]+m);
  for(int i=0;i<n;i++)
    BFS(i,n,m);
  Cost re=0;
  for(int i=0;i<n;i++)
    re+=weight[i][lmatch[i]];
  return re;
}
};
//Maximum Cardinality bipartite matching
template<int N>
class bipartite_match_t
{
public:
int rset[N];
int rmatch[N];
int ff;
bool augment(int v,const vector<vector<int> >& edge)
{
	for(int i=0;i<edge[v].size();i++)
	{
		int u=edge[v][i];
		if(ff==rset[u])continue;
		rset[u]=ff;
		if(rmatch[u]==-1||augment(rmatch[u],edge))
		{
			rmatch[u]=v;
			return true;
		}
	}
	return false;
}
int maxMatch(int n,int m,const vector<vector<int> >& edge)
{
	memset(rset,0,sizeof(rset[0])*m);
	memset(rmatch,0xff,sizeof(int)*m);
	int res=0;
	ff=1;
	for(int i=0;i<n;i++)
	{
		if(!augment(i,edge))continue;
		res++;
		ff++;
		//memset(rset,false,sizeof(bool)*m);
	}
	return res;
};
};
