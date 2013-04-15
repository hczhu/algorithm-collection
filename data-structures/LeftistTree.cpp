#include <algorithm>
#include <assert.h>
template<int S>
class LeftistTree
{
  int ll[S+1],rr[S+1],size[S+1],key[S+1];
  int top;
  int (*kcmp)(int,int);
public:
  LeftistTree(int (*cmp)(int,int)):top(0),kcmp(cmp){size[0]=ll[0]=rr[0]=0;};
  int makeTree(int kk)
  {
    top++;
    assert(top<=S);
    ll[top]=rr[top]=0;
    size[top]=1;
    key[top]=kk;
    return top;
  };
  int pop(int& root)
  {
    assert(root);
    root=merge(ll[root],rr[root]);
    return root;
  };
  int push(int& root,int kk)
  {
    if(0==root)root=makeTree(kk);
    else
    {
      int node=makeTree(kk);
      root=merge(node,root);
    }
    return root;
  };
  int merge(int ra,int rb)
  {
    assert(size[ll[ra]]>=size[rr[ra]]);
    assert(size[ll[rb]]>=size[rr[rb]]);
    if(ra==0)return rb;
    if(rb==0)return ra;
    if(kcmp(key[ra],key[rb])>0)std::swap(ra,rb);
    int son=merge(rr[ra],rb);
    rr[ra]=son;
    if(size[ll[ra]]<size[rr[ra]])swap(ll[ra],rr[ra]);
    size[ra]=size[ll[ra]]+size[rr[ra]]+1;
    return ra;
  };
  int getSize(int root){return size[root];};
  void clear(int (*cmp)(int,int)=NULL){top=0;if(cmp)kcmp=cmp;};
  int getKey(int ind){return key[ind];};
};
