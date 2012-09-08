#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <assert.h>
using namespace std;
#define N 200012
//Size of alphabets
#define M 54
#define NDEBUG
//Notice !!!
//No real leaf nodes exist
//next[i]=a means the leaf enode -a+2
struct
{
	int next[M];
	int begin,end;
	int suffixLink;
	int parent;
}snode[N+1];
//leaf nodes to reduce memory usage
struct
{
	int suffixIndex;
	int parent,begin,end;
}enode[1+N];
char str[N+1];
void clearSnode(int ind)
{
	memset(snode[ind].next,0xff,sizeof(snode[ind].next));
	snode[ind].suffixLink=-1;
}
void addInternalNode(int top,int head,int len,int ind,char* str)
{
	int son=snode[head].next[ind];
	assert(son!=-1);
	assert(len>0);
	int b,e;
	if(son<=-2)
	{
		son=-son-2;
		b=enode[son].begin;
		e=enode[son].end;
		enode[son].parent=top;
		enode[son].begin+=len;
		son=-son-2;
	}
	else
	{
		b=snode[son].begin;
		e=snode[son].end;
		snode[son].begin+=len;
		snode[son].parent=top;
	}
	clearSnode(top);
	snode[top].parent=head;
	snode[head].next[ind]=top;
	snode[top].next[str[b+len]]=son;
	snode[top].begin=b;
	snode[top].end=b+len-1;
}
int suffixRescan(int prefix,int prefixPos,int headPos,char* str,int& top)
{
	int son;
	while(prefixPos<headPos)
	{
		son=snode[prefix].next[str[prefixPos]];
		assert(son!=-1);
#ifdef OUTPUT 
		if(son>=0)
		{
			for(int i=0;i<=snode[son].end-snode[son].begin&&i+prefixPos<headPos;i++)
				assert(str[i+snode[son].begin]==str[prefixPos+i]);
		}
		else
		{
			son=-son-2;
			for(int i=0;i<=enode[son].end-enode[son].begin&&i+prefixPos<headPos;i++)
				assert(str[i+enode[son].begin]==str[prefixPos+i]);
			son=-son-2;
		}
#endif
		int len=(son<=-2?enode[-son-2].end-enode[-son-2].begin+1:
				1+snode[son].end-snode[son].begin);
		if(prefixPos+len>headPos)
		{
			addInternalNode(top,prefix,headPos-prefixPos,str[prefixPos],str);
			prefix=top++;
			break;
		}
		assert(son>=0);
		prefixPos+=snode[son].end-snode[son].begin+1;
		prefix=son;
	}
	return prefix;
}
void suffixScan(int& head,int& headPos,char* str,int& top)
{
	for(;;)
	{
		assert(head>=0);
		int son=snode[head].next[str[headPos]];
		if(son==-1)break;
		int len;
		if(son>=0)
		{
			assert(str[headPos]==str[snode[son].begin]);
			for(len=snode[son].begin;len<=snode[son].end&&str[len]==str[headPos];len++,headPos++);
			assert(len>snode[son].begin);
			if(len<=snode[son].end)
			{
				addInternalNode(top,head,len-snode[son].begin,str[snode[son].begin],str);
				head=top++;
				break;
			}
		}
		else
		{
			son=-son-2;
			assert(str[headPos]==str[enode[son].begin]);
			for(len=enode[son].begin;len<=enode[son].end&&str[len]==str[headPos];len++,headPos++);
			assert(len>enode[son].begin);
			if(len<=enode[son].end)
			{
				addInternalNode(top,head,len-enode[son].begin,str[enode[son].begin],str);
				head=top++;
				break;
			}
			assert(0);
		}	
		head=son;
	}
}
//Used for debug
void printSnode(int ind)
{
	puts("--------------------------");
	printf("snode%d parent%d %d->%d\n",ind,snode[ind].parent,snode[ind].begin,snode[ind].end);
	for(int i=snode[ind].begin;i<=snode[ind].end;i++)
		printf("%c",str[i]+'a');
	puts("");
	int cc=0;
	for(int i=0;i<M;i++)
		if(snode[ind].next[i]!=-1)
		{
			cc++;
			if(snode[ind].next[i]>=0)
				printf("son %d\n",snode[ind].next[i]);
			else 
			{
				int nd=-snode[ind].next[i]-2;
				printf("end %d %d %d\n",enode[nd].suffixIndex,enode[nd].begin,enode[nd].end);
			}
		}
	assert(ind==0||cc!=1);
	assert(ind==0||snode[snode[ind].parent].next[str[snode[ind].begin]]==ind);
}
int constructSuffixTree(char* str,const int n)
{
	clearSnode(0);
	snode[0].parent=snode[0].begin=snode[0].end=snode[0].suffixLink=0;
	snode[0].end=-1;
	int prefix,prefixPos,phead,head=0,top=1,headPos=0,etop=0;
	for(int i=0;i<n;i++)
	{
		if(headPos<i)headPos=i;
		phead=snode[head].parent;
		if(snode[head].suffixLink==-1)
		{
			prefix=snode[phead].suffixLink;
			prefixPos=headPos-(snode[head].end-snode[head].begin+1);
			if(prefixPos<i)prefixPos=i;
			snode[head].suffixLink=suffixRescan(prefix,prefixPos,headPos,str,top);
		}
		head=snode[head].suffixLink;
		suffixScan(head,headPos,str,top);
		assert(snode[head].next[str[headPos]]==-1);

		enode[etop].suffixIndex=i;
		enode[etop].parent=head;
		enode[etop].begin=headPos;
		enode[etop].end=n-1;
		snode[head].next[str[headPos]]=-etop-2;
		etop++;
#ifdef OUTPUT
		puts("**********************************\n");
		printf("add %d\n",i);
		for(int i=0;i<top;i++)
			printSnode(i);
#endif
	}
#ifdef OUTPUT
	puts("**********************************\n");
	for(int i=0;i<top;i++)
		printSnode(i);
#endif
	assert(top<=n);
	assert(etop<=n);
	return top;
}
