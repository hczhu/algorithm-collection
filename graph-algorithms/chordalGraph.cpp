#include <stdio.h>
#include <algorithm>
#include <vector>
#include <list>
#include <assert.h>
using namespace std;
struct Bucket
{
	bool mark;
	list<int> vertex;
	Bucket():mark(false){};
};
vector<int> lexBFS(vector< vector<int> >& edge)
{
	typedef list<Bucket>::iterator Bit;
	vector<int> res;
	const int n=edge.size();
	list<Bucket> qq;
	list< Bit > mq;
	qq.push_back(Bucket());
	vector< Bit > vb(n,qq.begin());
	vector< list<int>::iterator > pos;
	for(int i=0;i<n;i++)
	{
		(*(qq.begin())).vertex.push_back(i);
		pos.push_back(--(qq.begin()->vertex.end()));
	}
	for(int i=0;i<n;i++)
	{
		list<int>& lv=qq.begin()->vertex;
		assert(!lv.empty());
		int v=*lv.begin();
		lv.erase(lv.begin());
		vb[v]=qq.end();
		if(lv.empty())qq.erase(qq.begin());
		res.push_back(v);
		for(int i=0;i<edge[v].size();i++)
		{
			int u=edge[v][i];
			if(vb[u]==qq.end())continue;
			Bit uit=vb[u];
			uit->vertex.erase(pos[u]);
			if(!uit->mark)
			{
				uit->mark=true;
				mq.push_back(uit);
				qq.insert(uit,Bucket());
			}
			uit--;
			uit->vertex.push_back(u);
			vb[u]=uit;
			pos[u]=--(uit->vertex.end());
		}
		for(list<Bit>::iterator it=mq.begin();it!=mq.end();it++)
		{
			(*it)->mark=false;
			if((*it)->vertex.empty())qq.erase(*it);
		}
		mq.clear();
	}
	return res;
}
void countingSort(vector<int>& vv,int m)
{
	vector<int> cnt(m,0);
	vector<int> tmp=vv;
	const int n=vv.size();
	for(int i=0;i<n;i++)
		cnt[vv[i]]++;
	for(int i=1;i<m;i++)
		cnt[i]+=cnt[i-1];
	for(int i=0;i<n;i++)
		vv[--cnt[tmp[i]]]=tmp[i];
}
bool chordalGraph(vector<int>& perfect,vector< vector<int> >& edge)
{
	const int n=perfect.size();
	vector<int> order(n);
	for(int i=0;i<n;i++)order[perfect[i]]=i;
	vector< vector<int> > want(n);
	for(int i=n-1;i>=0;i--)
	{
		int v=perfect[i];
		int u,value=-1;
		for(int j=0;j<edge[v].size();j++)
		{
			if(order[edge[v][j]]<i&&order[edge[v][j]]>value)
				value=order[edge[v][j]],u=edge[v][j];
		}
		if(value==-1)continue;
		for(int j=0;j<edge[v].size();j++)
		{
			if(u==edge[v][j]||order[edge[v][j]]>=i)continue;
			want[u].push_back(edge[v][j]);
		}
	}
	for(int i=0;i<n;i++)
	{
		countingSort(want[i],n);
		countingSort(edge[i],n);
		for(int p=0,q=0;p<want[i].size();p++)
		{
			for(;edge[i][q]!=want[i][p]&&q<edge[i].size();q++);
			if(q==edge[i].size())return false;
		}
	}
	return true;
}
int main()
{
	int n,m;
	while(scanf("%d %d",&n,&m)!=EOF&&n>0)
	{
		vector< vector<int> > edge(n);
		for(int i=0;i<m;i++)
		{
			int a,b;
			scanf("%d %d",&a,&b);
			a--;b--;
			edge[a].push_back(b);
			edge[b].push_back(a);
		}
		vector<int> perfect=lexBFS(edge);
		bool f=chordalGraph(perfect,edge);
		printf("%s\n",f?"Perfect":"Imperfect");
		if(f)
		{
			for(int i=0;i<n;i++)
				printf("%d ",perfect[i]+1);
			puts("");
		}
	}
	return 0;

}

