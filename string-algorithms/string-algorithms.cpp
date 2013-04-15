#include <algorithm>
#include <stdlib.h>
#include <assert.h>
#include <memory.h>
#include <stdio.h>
//#define NDEBUG
#include <assert.h>
#include <string>
#include <vector>
#include <iostream>
#include <queue>
using namespace std;
//Linear time kmp algorithm. It returns all matched positions
vector<int> KMPmatch(const vector<int>& text,const vector<int>& pattern)
{
  const int n=text.size();
  const int m=pattern.size();
  assert(m);
  vector<int> fail(m);
  fail[0]=-1;
  for(int i=1,k=-1;i<pattern.size();i++)
  {
    while(k!=-1&&pattern[k+1]!=pattern[i])k=fail[k];
    if(pattern[k+1]==pattern[i])k++;
    fail[i]=k;
  }
  vector<int> ret;
  for(int i=0,k=-1;i<n;i++)
  {
    while(k!=-1&&text[i]!=pattern[k+1])k=fail[k];
    if(pattern[k+1]==text[i])k++;
    if(k==m-1)ret.push_back(i-m+1),k=fail[k];
  }
  return ret;
}


//extend[i] = the largest k such that pattern.substr(i,k)=pattern.substr(0,k)
void extended_kmp(int pattern[],int n,int extend[])
{
  extend[0]=1;
  // reach=Max{ extend[j]+j-1 | 0< j <i }
  // idx = arg_max{ extend[j]+j-1 | 0< j <i}
  int reach=0,idx=0;
  for(int i=1;i<n;i++)
  {
    int p=0;
    if(reach>=i)p=min(reach+1-i,extend[i-idx]);
    while(p+i<n&&pattern[p]==pattern[p+i])p++;
    extend[i]=p;
    if(p+i-1>reach)reach=p+i-1,idx=i;
  }
  extend[0]=n;
}
// text[i]= Max{ k | pattern.substr(0,k)=text.substr(i,k) }
// match is supposed to be filled by extended_kmp(pattern,n,match)
void longest_common_prefix(int pattern[],int n,int text[],int m,int extend[]=NULL)
{
  bool allo=false;
  if(extend==NULL)
  {
    allo=true;
    extend=new int[n];
    extended_kmp(pattern,n,extend);
  }
  int idx=-1,reach=-1;
  for(int i=0;i<m;i++)
  {
    int p=i;
    if(reach>=i)p=min(reach+1,extend[i-idx]+i);
    while(p-i<n&&pattern[p-i]==text[p])p++;
    text[i]=p-i;
    if(reach<p-1)reach=p-1,idx=i;
  }
  if(allo)delete [] extend;
}
//*********************************************************************
// AC automaton
//We don't need to worry the situation 
//that a added string is a substring of another string
template<int ALPHA,int M>
class StringsAutomaton
{
  int dep[M];
  int next[M][ALPHA],fail[M];
//Are there some strings ending at node #i
  bool end[M];
  int qq[M];
//Number of strings in the subtree rooted at node #i
  int count[M];
// Charaters are mapped to integers.
  int hash[1000];
  int top;
public:
  void setHash(char hp[])
    {
        if(hp)
        {
            for(int i=0;hp[i];i++)
                hash[hp[i]]=i;
        }
        else
        {
            for(int i=0;i<26;i++)
                hash['a'+i]=hash['A'+i]=i;
        }
    };
    StringsAutomaton(char hp[]=NULL)
    {
        top=2;
        clearNode(1);
        setHash(hp);
    }
  void clearNode(int r)
  {
    memset(next[r],0,sizeof(int)*ALPHA);
    end[r]=false;
    count[r]=0;
    dep[r]=0;
  }
  void removeAll()
  {
    top=2;
    clearNode(1);
  }
  void constructAll(const vector<string>& all,char hp[]=NULL)
  {
    if(hp)setHash(hp);
    top=2;
    clearNode(1);
    for(int i=0;i<all.size();i++)
      addTrie(all[i].c_str(),all[i].length());
  }
  inline int getNext(int r,char ch)
  {
    return next[r][hash[ch]];
  };
  inline int getCount(int r){return count[r];};
  inline bool getEnd(int r){return end[r];};
  inline int getDep(int r){return dep[r];};
  bool addTrie(const char* pp,int len)
  {
      int r=1;
      for(int i=0;pp[i];i++)
      {
          assert(r);
      count[r]++;
          if(next[r][hash[pp[i]]]==0)
          {
            assert(top<M);
        clearNode(top);
              dep[top]=dep[r]+1;
        next[r][hash[pp[i]]]=top++;
          }
          r=next[r][hash[pp[i]]];
      }
    if(end[r])return false;
    end[r]=true;
    count[r]++;
    return true;
  }
  void constructAutomaton()
  {
       int head=0,tail=0;
       fail[1]=1;
      for(int i=0;i<ALPHA;i++)
      {
          if(next[1][i])
          {
               qq[tail++]=next[1][i];
              fail[next[1][i]]=1;
          }
      }
      while(head<tail)
      {
          int v=qq[head++];
          for(int i=0;i<ALPHA;i++)
          {
              if(next[v][i])
              {
                  qq[tail++]=next[v][i];
                  int p=fail[v];
                  while(p!=1&&0==next[p][i])p=fail[p];
                  if(next[p][i])p=next[p][i];
                  fail[next[v][i]]=p;
              }
          }
      }
      for(int i=0;i<ALPHA;i++)
          if(0==next[1][i])next[1][i]=1;
      for(int i=0;i<tail;i++)
      {
          int v=qq[i];
      assert(fail[v]);
      //Process substring issue
      if(!end[v])end[v]=end[fail[v]];
          for(int i=0;i<ALPHA;i++)
          {
              if(next[v][i])continue;
              assert(fail[v]);
              next[v][i]=next[fail[v]][i];
              assert(next[v][i]);
          }
      }
  }
};
//********************************************
// extend[i] = Max{ k | str.substr(i-k,2*k+1) is a palindrome}
void palindrom_extend(int str[],int n,int extend[])
{
  // reach = Max{extend[j]+j |  j<i }
  int idx=0,reach=0;
  extend[0]=0;
  for(int i=1;i<n;i++)
  {
    int len=0;
    if(reach>i)len=min(reach-i,extend[2*idx-i]);
    while(i-len-1>=0&&i+len+1<n&&str[i-len-1]==str[i+len+1])len++;
    extend[i]=len;
    if(reach<extend[i]+i)
    {
      reach=extend[i]+i;
      idx=i;
    }
  }
}
// A more general AC automaton.
template<typename Char>
class AC_automaton_t
{
public:
  struct edge_t
  {
    Char ch;
    int idx;
    edge_t(){};
    edge_t(Char a,int b):ch(a),idx(b){};
    bool operator<(const edge_t& other)const
    {
      return ch<other.ch||(ch==other.ch&&idx<other.idx);
    };
  };
  struct node_t
  {
    int dep,count,sum,failed;
    vector<edge_t> edge;
    node_t():dep(0),count(0),sum(0),failed(0){/*edge.reserve(10)*/;};
    node_t(int a,int b=0,int c=0):dep(a),count(b),sum(c),failed(0){edge.reserve(10);};
    void sort(){std::sort(edge.begin(),edge.end());};
    void print()const{cerr<<"dep: "<<dep<<" cout: "<<count<<" sum: "<<sum
            <<" failed: "<<failed<<" edges:";
          for(int i=0;i<edge.size();i++)cerr<<" ("<<edge[i].ch<<","<<edge[i].idx<<")";
          cerr<<endl;};
  };
  vector<node_t> nodes;
public:
  AC_automaton_t(int size=1):nodes(size){};
  void resize(int size){nodes.resize(size);};
  void clear(){for(int i=0;i<nodes.size();i++)nodes[i].clear();};
  int find_next_sorted(const node_t& node,Char ch)const
  {
    edge_t out(ch,-1);
    int pos=(int)(lower_bound(node.edge.begin(),node.edge.end(),out)-node.edge.begin());
    if(pos<node.edge.size()&&node.edge[pos].ch==ch)return node.edge[pos].idx;
    return -1;
  };
  int find_next(const node_t& node,Char ch)const
  {
    for(int i=0;i<node.edge.size();i++)
      if(ch==node.edge[i].ch)return node.edge[i].idx;
    return -1;
  }
  void add_str(const Char str[],int len,int sum=0)
  {
    int pos=0;
    for(int i=0;i<len;i++)
    {
      nodes[pos].count++;
      int idx=find_next(nodes[pos],str[i]);
      if(idx==-1)
      {
        idx=nodes.size();
        nodes.push_back(node_t(nodes[pos].dep+1));
        nodes[pos].edge.push_back(edge_t(str[i],idx));
      }
      pos=idx;
    }
    nodes[pos].sum+=sum;
  };
  int get_dep(int st)const {return nodes[st].dep;};
  int get_count(int st)const {return nodes[st].count;};
  int get_sum(int st)const {return nodes[st].sum;};
  int get_failed(int st)const {return nodes[st].failed;};
  const vector<edge_t>& get_edges(int st)const {return nodes[st].edge;};
  int get_next(int st,Char ch)const {return find_next(nodes[st],ch);};
  int get_next_sorted(int st,Char ch)const {return find_next_sorted(nodes[st],ch);};

  int go_next(int st,Char ch)const 
  {
    int res;
    while(st!=-1&&(res=find_next_sorted(nodes[st],ch))==-1)
        st=nodes[st].failed;
    if(st!=-1)return res;
    return 0;
  };
  void construct_automaton()
  {
    for(int i=0;i<nodes.size();i++)nodes[i].sort();
    queue<int> qq;
    qq.push(0);
    nodes[0].failed=-1;
    int cnt=0;
    while(qq.size())
    {
      int pos=qq.front();qq.pop();
      const vector<edge_t>& edge=nodes[pos].edge;
      const int start=nodes[pos].failed;
      for(int i=0;i<edge.size();i++)
      {
        Char ch=edge[i].ch;
        int idx=edge[i].idx;
        nodes[idx].failed=0;
        for(int st=start;st!=-1;st=nodes[st].failed)
        {
          int next;
          if((next=find_next_sorted(nodes[st],ch))!=-1)
          {
            nodes[idx].failed=next;
            assert(next>=0&&next<nodes.size());
            break;
          }
        }
        qq.push(idx);
        // sum all matched suffix 
        assert(nodes[idx].failed>=0&&nodes[idx].failed<nodes.size());
        nodes[idx].sum+=nodes[nodes[idx].failed].sum;
      }
    };
  };
  void print_automaton()const
  {
    cerr<<"Nodes number="<<nodes.size()<<endl;
    for(int i=0;i<nodes.size();i++)nodes[i].print();
  };
  int size()const
  {
    return nodes.size();
  };
};

//Linear time sort alphabetically strings buf+offset[0],buf+offset[1],...
// sort[i] is the string index (0-based) of the rank i string
void sort_strings(const char buf[],const int offset[],int n,int sorted[])
{

}
