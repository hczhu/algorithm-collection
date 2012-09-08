/*
 *An implementation of the algorithm in the paper
 "A new flexible algorithm for the longest common subsequence problem"
 *The time complexity is O(min(pm,p(n-p))) where m and n (m<=n) are lengths of two strings , and p
  is the length of LCS.
 *
 */
#include <stdio.h>
#include <assert.h>
#include <memory.h>
#include <algorithm>
using namespace std;
namespace LCS
{
#define N 20010
#define S 100
int rowMatch[N+2],colMatch[N+2];
int nextPos1[N+2][S],nextPos2[N+2][S];
void preprocess(int str1[],int n,int str2[],int m,int s=S)
{
	for(int i=0;i<s;i++)
		nextPos1[n+1][i]=nextPos1[n][i]=n,nextPos2[m+1][i]=nextPos2[m][i]=m;
	for(int i=n-1;i>=0;i--)
	{
		memcpy(nextPos1[i],nextPos1[i+1],sizeof(int)*s);
		nextPos1[i][str1[i]]=i;
	}
	for(int i=m-1;i>=0;i--)
	{
		memcpy(nextPos2[i],nextPos2[i+1],sizeof(int)*s);
		nextPos2[i][str2[i]]=i;
	}
}
int longestCommonSubsequence(int str1[],int n,int str2[],int m)
{
	if(n>m)swap(str1,str2),swap(n,m);
	const int s=1+max(*max_element(str1,str1+n),*max_element(str2,str2+m));
	preprocess(str1,n,str2,m,s);
	colMatch[0]=rowMatch[0]=-1;
	for(int i=1;i<=n+1;i++)
		rowMatch[i]=m,colMatch[i]=n;
	int low=1;
	for(int i=0;i<n;i++)
	{
		int r,c;
		if(colMatch[low]==i)c=nextPos2[rowMatch[low++]+1][str1[i]];
		else c=nextPos2[i][str1[i]];
		for(int k=low;c<m;k++)
		{
			int tmp=rowMatch[k];
			if(tmp>=c)
			{
				rowMatch[k]=c;
				c=nextPos2[tmp+1][str1[i]];
			}
		}
		if(rowMatch[low]==i)r=nextPos1[colMatch[low++]+1][str2[i]];
		else r=nextPos1[i+1][str2[i]];
		for(int k=low;r<n;k++)
		{
			int tmp=colMatch[k];
			if(tmp>=r)
			{
				colMatch[k]=r;
				r=nextPos1[tmp+1][str2[i]];
			}
		}
	}
	while(rowMatch[low]<m)low++;
	return low-1;
}
};
