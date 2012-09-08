#include <stdio.h>
#include <algorithm>
#include <memory.h>
#include <assert.h>
using namespace std;
//suffix array and LCP O(N)
//height[i] means the LCP of suffix[i] and suffix[i+1]
#define N 10000
namespace LinearSuffixArray
{
int suf_buf[N+10];
int sort_cnt[N+10];
int suf_rank[N+10];
int suf_sort[N+10];
int* cmp_pp;
int array[N+1];
int height[N+1];
int suffix[N+1];
bool suffix_cmp(int a,int b)
{
	int i;
	if ( a==b )return false; 
	for ( i=0;;i++ )
		if (cmp_pp[a+i]!=cmp_pp[b+i] )
			return cmp_pp[a+i]<cmp_pp[b+i];
	return false;
}
void count_pass(int* rank,int* pre_sa,int* next_sa,int n,int s,int kk)
{
	int i;
	memset(sort_cnt,0,sizeof(int)*(kk+1));
	for ( i=0;i<n;i++ )
		sort_cnt[rank[pre_sa[i]+s]]++;
	for ( i=1;i<=kk;i++ )
		sort_cnt[i]+=sort_cnt[i-1];
	for ( i=n-1;i>=0;i-- )
		next_sa[--sort_cnt[rank[pre_sa[i]+s]]]=pre_sa[i];
}
void sort_triple(int* rank,int* sa,int n,int kk)
{
	count_pass(rank,sa,suf_buf,n,2,kk);
	count_pass(rank,suf_buf,sa,n,1,kk);
	count_pass(rank,sa,suf_buf,n,0,kk);
	memcpy(sa,suf_buf,sizeof(int)*n);	
}
inline bool triple_equal(int a,int b,int* rank)
{
	return rank[a]==rank[b]&&(rank[a+1]==rank[b+1])
		&&rank[a+2]==rank[b+2];
}
inline bool suf_less(int* rank,int* srank,int a,int b)
{
	if ( b%3==1 )
		return rank[a]<rank[b]||
			(rank[a]==rank[b]&&srank[a+1]<srank[b+1]);
	else 
		return rank[a]<rank[b]||
			(rank[a]==rank[b]&&rank[a+1]<rank[b+1])
			||(rank[a]==rank[b]&&rank[a+1]==rank[b+1]&&srank[a+2]<srank[b+2]);
}
//kk is the maximum element in the rank[]
void sort_suffix(int* rank,int* sa,int n,const int kk)
{
	int rr,n0,i,add,n1,n2,top,h0,h1;
	if ( n<3 )
	{
		for ( i=0;i<n;i++ )sa[i]=i;
		cmp_pp=rank;
		sort(sa,sa+n,suffix_cmp);
		return ;
	}
	rank[n]=rank[n+1]=rank[n+2]=kk+1;
	n0=(n+3)/3;n1=(n+2)/3;n2=(n+1)/3;
	int* tsa=new int[n1+n2+3],*trank=new int[n1+n2+3];
	for ( top=0,i=1;i<=n;i+=3 )tsa[top++]=i;
	
	//assert(top==n1);
	
	for ( i=2;i<=n;i+=3 )tsa[top++]=i;
	
	//assert(top==n1+n2);
	
	sort_triple(rank,tsa,top,kk+1);
	
	//get triple rank in trank
	add=(tsa[0]%3==1 )?0:n1;
	trank[add+tsa[0]/3]=0;
	for (rr=0,i=1;i<top;i++ )
	{
		if ( !triple_equal(tsa[i-1],tsa[i],rank) ) rr++;
		add=(tsa[i]%3==1 )?0:n1;
		trank[add+tsa[i]/3]=rr;
	}
	sort_suffix(trank,tsa,top,rr); 
	//get 1-2 rank in suf_buf
	for ( i=0;i<top;i++ ) 
	{
		if (tsa[i]<n1)tsa[i]=3*tsa[i]+1;
		else tsa[i]=3*(tsa[i]-n1)+2;
		suf_buf[tsa[i]]=i;
	}
	while ( tsa[top-1]>=n )top--;
	
	//sort pos0 in trank
	for ( n0=i=0;i<n;i+=3 )
		trank[n0++]=i;
	//assert(n0+top==n);
	count_pass(suf_buf,trank,sa,n0,1,top);
	count_pass(rank,sa,trank,n0,0,kk);
	
	//merge trank and tsa to sa
	for ( n1=h1=h0=0;h0<n0&&h1<top; )
	{
		if ( suf_less(rank,suf_buf,trank[h0],tsa[h1]) )
			sa[n1++]=trank[h0++];
		else sa[n1++]=tsa[h1++];
	}
	while ( h0<n0 ) sa[n1++]=trank[h0++];
	while ( h1<top ) sa[n1++]=tsa[h1++];
	//assert(n1==n);	
	delete [] tsa;
	delete [] trank;
}
};
