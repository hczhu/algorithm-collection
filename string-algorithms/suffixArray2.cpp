#include <memory.h>
#include <algorithm>
#include <assert.h>
using namespace std;
namespace SuffixArray
{
#define N 200000
int array[N+1];
int mycount[1+N];
int buf[N+1];
int rank[N+1],suffix[N+1],tmp[N+1];
int height[N+1];
void countingSort(int n,int* array,int* rank)
{
    int i;
    const int pmax=(*max_element(rank,rank+n));
    assert(pmax<=N);
    memset(mycount,0,sizeof(int)*(pmax+1));
    for(i=0;i<n;i++)
        mycount[rank[i]]++;
    for(i=1;i<=pmax;i++)
        mycount[i]+=mycount[i-1];
    for(i=n-1;i>=0;i--)
        buf[--mycount[rank[array[i]]]]=array[i];
    memcpy(array,buf,sizeof(int)*n);
}
void getRank(int n,int* suffix,int* rank,int t)
{
    int i;
    buf[suffix[0]]=0;
    for(i=1;i<n;i++)
    {
        if(rank[suffix[i]]==rank[suffix[i-1]]
                &&rank[suffix[i]+t]==rank[suffix[i-1]+t])
            buf[suffix[i]]=buf[suffix[i-1]];
        else buf[suffix[i]]=buf[suffix[i-1]]+1;
    }
    memcpy(rank,buf,sizeof(int)*n);
}
void sortSuffix(int* num,int n)
{
	memcpy(array,num,sizeof(int)*n);
    array[n++]=(*max_element(array,array+n))+1;
    memcpy(rank,array,sizeof(int)*n);
    int i,t;
    for(i=0;i<n;i++)suffix[i]=i;
    countingSort(n,suffix,rank);
    getRank(n,suffix,rank,0);
    for(i=1,t=1;t<n;i++,t<<=1)
    {
        for(i=0;i<n;i++)tmp[i]=(i+t)<n?rank[i+t]:n-1;
        countingSort(n,suffix,tmp);
        countingSort(n,suffix,rank);
        getRank(n,suffix,rank,t);
    }
}
void LCP(int n)
{
    int i,j,pre;
    for(i=0;i<n;i++)rank[suffix[i]]=i;
    for(i=pre=0;i<n;i++)
    {
        j=suffix[rank[i]+1];
        if(pre)pre--;
        while(array[i+pre]==array[j+pre])
            pre++;
        height[rank[i]]=pre;
    }
}
#define L 20
int rmq[L][N];
int hash[1<<L];
void RMQpreprocess(int n)
{
	for(int i=0;i<n;i++)
		rmq[0][i]=height[i];
	for(int i=1;(1<<i)<=n;i++)
	for(int j=0;j+(1<<i)<=n;j++)
		rmq[i][j]=min(rmq[i-1][j],rmq[i-1][j+(1<<(i-1))]);
	hash[0]=-1;
	for(int i=1,t=1;i<=n;i++)
	{
		hash[i]=hash[i-1];
		if(i==t)t<<=1,hash[i]++;
	}
}
int queryLCP(int a,int b)
{
	a=rank[a];
	b=rank[b];
	assert(a-b);
	if(a>b)swap(a,b);
	b--;
	int len=b+1-a;
	assert(len>=(1<<hash[len]));
	int re=min(rmq[hash[len]][a],rmq[hash[len]][b-(1<<(hash[len]))+1]);
	return re;
}
};
