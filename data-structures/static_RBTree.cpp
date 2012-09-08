#include <stdio.h>
#define NDEGUB
#include <assert.h>
template <int S,int (*cmp)(int a,int b)>
class RBTree
{
    private:
        int key[S+1],ll[S+1],rr[S+1],pp[S+1],top,root;
#ifdef SIZE
		int size[S+1];
#endif
        bool red[S+1];
        void leftRotate(int x);
        void rightRotate(int x);
        void insertFixup(int x);
        void removeFixup(int x);
        int keyCmp(int a,int b){return a==b?0:cmp(a,b);};
#ifdef OUTPUT
		int DFS(int v,bool f);
#endif
    public:
		RBTree():top(1),root(0)
		{
#ifdef SIZE
			size[0]=0;
#endif
			ll[0]=rr[0]=pp[0]=0;red[0]=false;
		};
		int insert(int kx);
		int find(int k);
		void remove(int x);
        int successor(int x);
		int predecessor(int x);
        int getKey(int x){return key[x];};
		void setKey(int x,int v){key[x]=v;};
		int begin();
		int end();
		int kelement(int k);
		int count_not_greater_than(int k);
		int preInsert(int x,int ky);
		int sucInsert(int x,int ky);
		void clear(){root=0;top=1;};
		//Haven't implemented 	
        void remove(int low,int up);// low and up are iterator, remove all elements between low and up inclusive
		int upperBound(int k);//return the first element greater than k or 0 if not exist
		int lowerBound(int k);//return the first element not less than k or 0 if not exist
#ifdef OUTPUT
		void check(){assert(!red[root]);DFS(root,false);};
		void printTree();
#endif		
};

#ifdef OUTPUT
template <int S,int (*cmp)(int a,int b)>
int RBTree<S,cmp>::DFS(int v,bool f=false)
{
	if(v==0)return 1;
#ifdef SIZE
	if(f)printf("Node%d:key=%d ll=%d rr=%d size=%d red=%s\n",v,key[v],ll[v],rr[v],size[v],red[v]?"red":"black");
#else
	if(f)printf("Node%d:key=%d ll=%d rr=%d red=%s\n",v,key[v],ll[v],rr[v],red[v]?"red":"black");
#endif
	int h=DFS(ll[v],f);
	if(red[v])assert(!red[ll[v]]&&!red[rr[v]]);
	assert(h==DFS(rr[v],f));
	if(!red[v])h++;
#ifdef SIZE
	assert(size[v]=size[ll[v]]+size[rr[v]]+1);
#endif
	return h;
};
template <int S,int (*cmp)(int a,int b)>
void RBTree<S,cmp>::printTree()
{
	puts("-------------------");
	DFS(root,true);
}
#endif
template <int S,int (*cmp)(int a,int b)>
int RBTree<S,cmp>::lowerBound(int k)
{
	if(root==0)return 0;
	int re=root;
	while(re&&cmp(key[re],k)<0)
		re=rr[re];
	if(re==0)return end();
	int v=ll[re];
	while(v)
	{
		if(cmp(key[v],k)<0)v=rr[v];
		else re=v,v=ll[v];
	}
	assert(re&&cmp(key[re],k)>=0);
	return re;
}
template <int S,int (*cmp)(int a,int b)>
int RBTree<S,cmp>::end()
{
	if(root==0)return 0;
	int re=root;
	while(rr[re])re=rr[re];
	return re;
}
template <int S,int (*cmp)(int a,int b)>
int RBTree<S,cmp>::begin()
{
	int re=root;
	while(ll[re])re=ll[re];
	return re;
}
#ifdef SIZE
template <int S,int (*cmp)(int a,int b)>
int RBTree<S,cmp>::kelement(int k)
{
	int v=root;
	if(size[v]<k)return 0;
	for(;;)
	{
		assert(v);
		assert(k);
		if(size[ll[v]]+1==k)return v;
		if(size[ll[v]]>=k)v=ll[v];
		else k-=size[ll[v]]+1,v=rr[v];
	}
	return 0;
}
#endif
template <int S,int (*cmp)(int a,int b)>
void RBTree<S,cmp>::leftRotate(int x)
{
    assert(x);
    int y=pp[x];
    int z=pp[y];
    assert(y&&rr[y]==x);
    rr[y]=ll[x];pp[rr[y]]=y;
    ll[x]=y;pp[y]=x;pp[x]=z;
    if(z==0)root=x;
    else if(ll[z]==y)ll[z]=x;
    else if(rr[z]==y)rr[z]=x;
    else assert(0);
#ifdef SIZE
	size[y]=size[ll[y]]+size[rr[y]]+1;
	size[x]=size[ll[x]]+size[rr[x]]+1;
#endif
}
template <int S,int (*cmp)(int a,int b)>
void RBTree<S,cmp>::rightRotate(int x)
{
    assert(x);
    int y=pp[x];
    int z=pp[y];
    assert(y&&ll[y]==x);
    ll[y]=rr[x];pp[ll[y]]=y;
    rr[x]=y;pp[y]=x;pp[x]=z;
    if(z==0) root=x;
    else if(ll[z]==y)ll[z]=x;
    else if(rr[z]==y)rr[z]=x;
    else assert(0);
#ifdef SIZE
	size[y]=size[ll[y]]+size[rr[y]]+1;
	size[x]=size[ll[x]]+size[rr[x]]+1;
#endif
}
template <int S,int (*cmp)(int a,int b)>
int RBTree<S,cmp>::find(int kx)
{
    int v=root;
    key[0]=kx;
    for(;;)
    {
        int d=keyCmp(kx,key[v]);
        if(d==0)break;
        v=(d<0)?ll[v]:rr[v];
    }
    return v;
}
template <int S,int (*cmp)(int a,int b)>
int RBTree<S,cmp>::insert(int kx)
{
    assert(top<=S);
    const int z=top++;
    ll[z]=rr[z]=pp[z]=0;
#ifdef SIZE	
	size[z]=1;
#endif
    key[z]=kx;red[z]=false;
    if(root==0)
    {
        root=z;
        return z;
    }
    int y=0,x=root;
    while(x)
    {
        y=x;
#ifdef SIZE		
		size[x]++;
#endif
        x=(keyCmp(kx,key[x])<0)?ll[x]:rr[x];
    }
    pp[z]=y;
    if(keyCmp(kx,key[y])<0)ll[y]=z;
    else rr[y]=z;
    red[z]=true;
    insertFixup(z);
    return z;
}
template <int S,int (*cmp)(int a,int b)>
int RBTree<S,cmp>::preInsert(int x,int  ky)
{
	assert(top<=S);
    assert(x);
	const int z=top++;
    ll[z]=rr[z]=pp[z]=0;
#ifdef SIZE	
	size[z]=1;
#endif
    key[z]=ky;red[z]=true;
	if(ll[x])
	{
		int y=predecessor(x);
		assert(y);
		assert(rr[y]==0);
		pp[z]=y;rr[y]=z;
	}
	else pp[z]=x,ll[x]=z;
#ifdef SIZE
	for(int p=pp[z];p;p=pp[p])size[p]++;
#endif
	insertFixup(z);
	return z;
}
template <int S,int (*cmp)(int a,int b)>
int RBTree<S,cmp>::sucInsert(int x,int  ky)
{
	assert(top<=S);
    assert(x);
	const int z=top++;
    ll[z]=rr[z]=pp[z]=0;
#ifdef SIZE	
	size[z]=1;
#endif
    key[z]=ky;red[z]=true;
	if(rr[x])
	{
		int y=successor(x);
		assert(y);
		assert(ll[y]==0);
		pp[z]=y;ll[y]=z;
	}
	else pp[z]=x,rr[x]=z;
#ifdef SIZE
	for(int p=pp[z];p;p=pp[p])size[p]++;
#endif
	insertFixup(z);
	return z;
}
template <int S,int (*cmp)(int a,int b)>
void RBTree<S,cmp>::insertFixup(int z)
{
    while(red[pp[z]])
    {
        assert(z);
        assert(pp[z]);
        assert(pp[pp[z]]);
        if(ll[pp[pp[z]]]==pp[z])
        {
            int y=rr[pp[pp[z]]];
            if(red[y])
            {
                red[pp[y]]=true;
                red[y]=red[pp[z]]=false;
                z=pp[y];
            }
            else
            {
                if(rr[pp[z]]==z)leftRotate(z),z=ll[z];
                assert(rr[pp[pp[z]]]==y);
                assert(z==ll[pp[z]]);
                rightRotate(pp[z]);
                red[pp[z]]=false;
                red[rr[pp[z]]]=true;
            }
        }
        else
        {
            int y=ll[pp[pp[z]]];
            if(red[y])
            {
                red[pp[y]]=true;
                red[y]=red[pp[z]]=false;
                z=pp[y];
            }
            else
            {
                if(ll[pp[z]]==z)rightRotate(z),z=rr[z];
                assert(ll[pp[pp[z]]]==y);
                assert(z==rr[pp[z]]);
                leftRotate(pp[z]);
                red[pp[z]]=false;
                red[ll[pp[z]]]=true;
            }
        }
    }
	red[root]=false;
}
template <int S,int (*cmp)(int a,int b)>
void RBTree<S,cmp>::remove(int x)
{
    assert(x);
    if(ll[x]&&rr[x])
    {
        int y=predecessor(x);
        assert(y&&rr[y]==0);
        int z=ll[y];
        pp[z]=pp[y];
        if(ll[pp[z]]==y)ll[pp[z]]=z;
        else rr[pp[z]]=z;
#ifdef SIZE
		for(int p=pp[y];p;p=pp[p])size[p]--;
#endif
        pp[y]=pp[x];ll[y]=ll[x];rr[y]=rr[x];
#ifdef SIZE
		size[y]=size[x];
#endif
        if(root==x)root=y;
        if(ll[pp[y]]==x)ll[pp[y]]=y;
        else rr[pp[y]]=y;
        pp[ll[y]]=y;pp[rr[y]]=y;
        if(!red[y])
        {
            red[y]=red[x];
            if(z==0)assert(rr[pp[z]]||ll[pp[z]]);
            removeFixup(z);
        }
        else red[y]=red[x];
    }
    else
    {
        int z=ll[x]?ll[x]:rr[x];
        if(root==x)root=z;
        if(root==0)return;
#ifdef SIZE
		for(int p=pp[x];p;p=pp[p])size[p]--;
#endif
        pp[z]=pp[x];
        if(ll[pp[z]]==x)ll[pp[z]]=z;
        else rr[pp[z]]=z;
        if(!red[x])
        {
            if(z==0)assert(ll[pp[z]]||rr[pp[z]]);
            removeFixup(z);
        }
    }
}
template <int S,int (*cmp)(int a,int b)>
void RBTree<S,cmp>::removeFixup(int x)
{
    while(x!=root&&!red[x])
    {
        if(ll[pp[x]]==x)
        {
            int w=rr[pp[x]];
            assert(w);
            if(red[w])leftRotate(w),red[w]=false,w=ll[w],red[w]=true,w=rr[w];
            assert(!red[w]);
            if(!red[ll[w]]&&!red[rr[w]])
            {
                red[w]=true;
                x=pp[x];
                continue;
            }
            if(!red[rr[w]])
            {
                assert(red[ll[w]]);
                rightRotate(ll[w]);
                red[w]=true;
                w=pp[w];
                assert(w);
                red[w]=false;
            }
            assert(w&&red[rr[w]]&&!red[w]);
            leftRotate(w);
            red[w]=red[ll[w]];
            red[ll[w]]=false;
            red[rr[w]]=false;
            x=root;
        }
        else
        {
            int w=ll[pp[x]];
            assert(w);
            if(red[w])rightRotate(w),red[w]=false,w=rr[w],red[w]=true,w=ll[w];
            assert(!red[w]);
            if(!red[rr[w]]&&!red[ll[w]])
            {
                red[w]=true;
                x=pp[x];
                continue;
            }
            if(!red[ll[w]])
            {
                assert(red[rr[w]]);
                leftRotate(rr[w]);
                red[w]=true;
                w=pp[w];
                assert(w);
                red[w]=false;
            }
            assert(w&&red[ll[w]]&&!red[w]);
            rightRotate(w);
            red[w]=red[rr[w]];
            red[ll[w]]=false;
            red[rr[w]]=false;
            x=root;
        }
    }
    red[x]=false;
}
template <int S,int (*cmp)(int a,int b)>
int RBTree<S,cmp>::predecessor(int x)
{
    if(ll[x]==0)
    {
        while(x&&ll[pp[x]]==x)x=pp[x];
        if(x==0)return 0;
        return pp[x];
    }
    x=ll[x];
    while(rr[x])x=rr[x];
    return x;   
}
template <int S,int (*cmp)(int a,int b)>
int RBTree<S,cmp>::successor(int x)
{
    if(rr[x]==0)
    {
        while(x&&rr[pp[x]]==x)x=pp[x];
        if(x==0)return 0;
        return pp[x];
    }
    x=rr[x];
    while(ll[x])x=ll[x];
    return x;
}
