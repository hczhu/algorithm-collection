#include <stdio.h>
#include <assert.h>
#include <memory.h>
#include <algorithm>
#include <iostream>
using namespace std;
#ifndef NDEBUG
    #define debug(x) cerr<<#x<<"=\""<<x<<"\""<<" at line#"<<__LINE__<<endl;
    #define hline() cerr<<"-----------------------------------------"<<endl;
#else
    #define debug(x)
    #define hline()
#endif

//****************************************************
template<int S,class comparator>
class binaryHeap
{
	int heap[S],hpos[S];
	int hs;
	const comparator *cmpLess;
public:
	binaryHeap():hs(0),cmpLess(NULL){};
	void set_cmp(const comparator& cmp){cmpLess=&cmp;};
	void adjustUp(int key)
	{
		int pos=hpos[key];
		assert(pos>=0&&pos<hs);
		assert(heap[pos]==key);
		while(pos&&(*cmpLess)(key,heap[(pos-1)>>1]))
		{
			heap[pos]=heap[(pos-1)>>1];
			hpos[heap[pos]]=pos;
			pos=(pos-1)>>1;
		}
		heap[pos]=key;
		hpos[key]=pos;
	}
	void adjustDown(int key)
	{
		int pos=hpos[key],mm;
		assert(pos>=0&&pos<hs);
		assert(heap[pos]==key);
		while((pos<<1)+1<hs)
		{
			mm=1+(pos<<1);
			if(mm+1<hs&&(*cmpLess)(heap[mm+1],heap[mm]))mm++;
			if((*cmpLess)(key,heap[mm]))break;
			heap[pos]=heap[mm];
			hpos[heap[pos]]=pos;
			pos=mm;
		}
		heap[pos]=key;
		hpos[key]=pos;
	}
	int popHeap()
	{
		int re=heap[0];
		hs--;
		if(hs)
		{
			heap[0]=heap[hs];
			hpos[heap[0]]=0;
			adjustDown(heap[0]);
		}
		return re;
	}
	void removeHeap(int key)
	{
		int pos=hpos[key];
		assert(pos>=0&&pos<hs);
		assert(heap[pos]==key);
		heap[pos]=heap[hs-1];
		hpos[heap[pos]]=pos;
		hs--;
		if(hs)
		{
			adjustUp(heap[pos]);
			adjustDown(heap[pos],hs);
		}
	}
	void pushHeap(int key)
	{
		heap[hs]=key;
		hpos[key]=hs;
		hs++;
		adjustUp(key);
	}
	int size(){return hs;};
	void clear(){hs=0;};
};
//**************************************************

template<int N,int M,typename flow_type=int,typename cost_type=int>
class min_cost_flow_solver_t
{
public:
cost_type cost[M*2],cap[2*M];
cost_type mindis[N],label[N];
flow_type flow[2*M];

int head[2*M],tail[2*M];
int edge[N+1],star[M*2];
int cnt[N+1];
int pre[N];
int visited[N];

bool operator()(int a,int b)const{return mindis[a]<mindis[b];};

int m,n;

binaryHeap<N,min_cost_flow_solver_t<N,M,flow_type,cost_type> > heap;

min_cost_flow_solver_t():m(0){heap.set_cmp(*this);};
void clear()
{
	m=0;
	n=0;
}
bool Dijkstra(int n,int s,int t,int& ff)
{
	visited[s]=1;
	mindis[s]=0;
	int top=0;
	heap.clear();
	heap.pushHeap(s);
	ff+=2;
	int cnt=0;
	for(;heap.size();)
	{
		int v=heap.popHeap();
		cnt++;
		for(int p=edge[v];p<edge[v+1];p++)
		{
			cnt++;
			int e=star[p];
			assert(flow[e]<=cap[e]);
			if(flow[e]==cap[e])continue;
			int u=tail[e];
			if(visited[u]==1+ff)continue;
			assert(v==head[e]);
			assert(cost[e]+label[v]-label[u]>=0);
			cost_type t=mindis[v]+cost[e]+label[v]-label[u];
			if(ff>visited[u])
			{
				visited[u]=ff;
				mindis[u]=t;
				pre[u]=e;
				heap.pushHeap(u);
			}
			else if(visited[u]==ff&&t<mindis[u])
			{
				mindis[u]=t;
				pre[u]=e;
				heap.adjustUp(u);
			}
		}
		label[v]+=mindis[v];
		visited[v]=ff+1;
	}
	return visited[t]>=ff;
}
void preprocess(int n,int m)
{
	memset(cnt,0,sizeof(int)*(1+n));
	memset(flow,0,sizeof(flow_type)*m);
	memset(label,0,sizeof(cost_type)*m);
	for(int i=0;i<m;i++)cnt[head[i]+1]++;
	for(int i=0;i<n;i++)cnt[i+1]+=cnt[i],edge[i]=cnt[i];
	edge[n]=cnt[n];
	for(int i=0;i<m;i++)star[--cnt[head[i]+1]]=i;
}
int add_one_edge(int _head,int _tail,int _cap,int _cost)
{
	assert(m<2*M);
	if(m==2*M)return -1;
	head[m]=_head;tail[m]=_tail;cap[m]=_cap;cost[m++]=_cost;
	head[m]=_tail;tail[m]=_head;cap[m]=0;cost[m++]=-_cost;
	n=max(n,_head+1);
	n=max(n,_tail+1);
	assert(n<=N);
	return 0;
}
cost_type minCostFlow(int s,int t,flow_type& flowUpper)
{
	assert(s<n);
	assert(t<n);
	debug(n);
	cost_type re=0;
	preprocess(n,m);
	memset(visited,0,sizeof(int)*n);
    int ff=0;
	flow_type sum=0;
	while(flowUpper>sum)
	{
        //printf("sum=%d\n",sum);
        if(!Dijkstra(n,s,t,ff))break;
		flow_type add=flowUpper-sum;
		for(int v=t;v!=s;)
		{
			int e=pre[v];
			add=min(add,cap[e]-flow[e]);
			v=head[e];
		}
		assert(add>0);
		for(int v=t;v!=s;)
		{
			int e=pre[v];
			flow[e]+=add;
			flow[e^1]-=add;
			re+=cost[e]*add;
			v=head[e];
			assert(cost[e]+label[head[e]]-label[tail[e]]==0);
		}
		sum+=add;
	}
	flowUpper=sum;
	return re;
}
};
min_cost_flow_solver_t<100,1000> solver;
int main()
{
	solver.add_one_edge(0,1,1,1);
	solver.add_one_edge(1,2,1,1);
	int up=2;
	assert(2==solver.minCostFlow(0,2,up));
	debug(up);
	assert(up==1);
	return 0;
}
