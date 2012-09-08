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
#include <stack>
#include <stdarg.h>
#define NDEBUG
#include <assert.h>
using namespace std;
#ifndef NDEBUG
    #define debug(x) cerr<<#x<<"=\""<<x<<"\""<<" at line#"<<__LINE__<<endl;
    #define hline() cerr<<"-----------------------------------------"<<endl;
	#define Unit_test(fun) class Test_##fun { public: Test_##fun (){ fun ();};}Test_obj_##fun
#else
    #define debug(x) 
    #define assert(x) if(!(x))*((int*)0)=0
    #define hline()
	#define Unit_test(fun)
#endif
typedef long long int llint;
typedef unsigned long long int ullint;
int get_root(int a,vector<int>& root)
{
	return root[a]==-1?a:(root[a]=get_root(root[a],root));
}
void LCA_DFS(int v,const vector<vector<int> >& son,const vector<pair<int,int> >& want,
				const vector<vector<int> >& rlist,vector<int>& ans,
				vector<char>& color,vector<int>& root)
{
	color[v]=1;
	for(int i=0;i<son[v].size();i++)
	{
		int u=son[v][i];
		LCA_DFS(u,son,want,rlist,ans,color,root);
		root[u]=v;
	}
	for(int i=0;i<rlist[v].size();i++)
	{
		int idx=rlist[v][i];
		int u=want[idx].first;
		if(u==v)u=want[idx].second;
		if(color[u]!=2&&v!=u)continue;
		ans[idx]=get_root(u,root);
//		cerr<<v<<" "<<u<<"="<<ans[idx]<<endl;
	}
	color[v]=2;
}
vector<int> offline_LCA(const vector<vector<int> >& son,const vector<pair<int,int> >& want,
		int tree_root)
{
	const int n=son.size();
	vector<vector<int> > rlist(n);
	vector<int> ans(want.size(),-1);
	vector<int> root(n,-1);
	vector<char> color(n,0);
	for(int i=0;i<want.size();i++)
	{
		rlist[want[i].first].push_back(i);
		if(want[i].first!=want[i].second)rlist[want[i].second].push_back(i);
	}
	LCA_DFS(tree_root,son,want,rlist,ans,color,root);
	for(int i=0;i<ans.size();i++)assert(ans[i]!=-1);
	return ans;
}

