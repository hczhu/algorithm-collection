#ifndef _AC_H_
#define _AC_H_
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <stack>
#include <map>
#include <deque>
#include <algorithm>
#include <cassert>
#include <string.h>
#include <queue>
#include <vector>
#include <set>
#include "common_header.h"

using namespace std;

#define ZDEBUG
#ifdef ZDEBUG
#define OUTFOX_RETURN(x,y) fprintf x;return y;
#define OUTFOX_LOG(x) fprintf x;
#define LP_ERROR stderr
#define LP_WARNING stderr
#endif


//#define DEBUG_OUTPUT
//The constructor of Info will be used
template<typename Char,typename Info>
class ACAuto
{
  typedef unsigned int uint;
  int GetCharIndex(Char ch)
  {
    if(ch>=0&&ch<sizeof(m_convertor.hash))
    {
      if(-1==m_convertor.hash[ch])m_convertor.hash[ch]=m_convertor.cnt++;
      return m_convertor.hash[ch];
    }
    else
    {
      if(m_convertor.mem.count(ch)==0)m_convertor.mem[ch]=m_convertor.cnt++;
      return m_convertor.mem[ch];
    }
  }
  int getCharIndex(Char ch)
  {
    if(ch>=0&&ch<sizeof(m_convertor.hash))
    {
      return m_convertor.hash[ch];
    }
    else
    {
      return m_convertor.mem.count(ch)?m_convertor.mem[ch]:-1;
    }
  }
  struct Convertor
  {
    int hash[1<<16];
    std::map<Char,int> mem;
    int cnt;
    Convertor():cnt(0){memset(hash,0xff,sizeof(hash));};
  }m_convertor;
  //If check<0, then -check points ot the previous empty cell
  //If base<0, then -base points to the next empty cell
  struct AC_node
  {
    int check,base,fail,dep;
    bool match;
    Info info;
    AC_node():check(-1),match(false){};
  };
  
  std::vector<AC_node> m_array;
  const AC_node* m_pArray;
  bool m_inited;
  int m_last_cell;
  std::vector<std::vector<uint> > m_vstr;
  std::vector<Info> m_vinfo;
  std::vector<std::vector<int> > m_edge;

  struct comparator
  {
    int pos;
    ACAuto *_ptr;
    comparator(int dep,ACAuto* ptr):pos(dep),_ptr(ptr){};
    bool operator()(int a,int b)
    {
      return _ptr->m_vstr[a][pos]<_ptr->m_vstr[b][pos];
    };
  };
  bool construct_helper(std::vector<int>& vidx,int parent,int dep)
  {
    assert(parent<m_array.size());
#ifdef DEBUG_OUTPUT
    std::cerr<<"constructing dep "<<dep<<" size="<<vidx.size()<<std::endl;
    for(int i=0;i<vidx.size();i++)std::cerr<<vidx[i]<<" ";
    std::cerr<<std::endl;
#endif
    if(vidx.empty())
    {
      m_array[parent].base=0;
      return true;
    }
    for(int i=0;i<vidx.size();i++)
    {
      if(m_vstr[vidx[i]].size()<=dep)
      {
        OUTFOX_RETURN((LP_ERROR,"Illeqal dep=%d idx=%d",dep,vidx[i]),false);
      }
    }
    std::sort(vidx.begin(),vidx.end(),comparator(dep,this));
    std::vector<int> have;
    have.push_back(m_vstr[vidx[0]][dep]);
    for(int i=1;i<vidx.size();i++)
    {
      if(m_vstr[vidx[i-1]][dep]!=m_vstr[vidx[i]][dep])
      {
        have.push_back(m_vstr[vidx[i]][dep]);
      }
    }
    while(m_last_cell<m_array.size()&&m_array[m_last_cell].check!=-1)m_last_cell++;
    //m_last_cell=
    bool ok=false;
    int base;
    for(base=std::max(0,m_last_cell-have.back());;base++)
    {
      if(base+have.back()>=m_array.size())m_array.resize(base+have.back()+1);
      ok=true;
      for(int i=0;i<have.size();i++)
      {
        if(m_array[base+have[i]].check!=-1)
        {
          ok=false;
          break;
        }
      }
      if(ok)break;
    }
    m_array[parent].base=base;
#ifdef DEBUG_OUTPUT
    std::cerr<<"Find base "<<base<<" for node "<<parent<<" with children ";
    for(int i=0;i<have.size();i++)std::cerr<<have[i]<<" ";std::cerr<<std::endl;
#endif
    if(base+m_convertor.cnt>m_array.size())m_array.resize(base+m_convertor.cnt);
    if(m_edge.size()<=parent)m_edge.resize(parent+1);

    for(int i=0;i<have.size();i++)
    {
      m_array[base+have[i]].check=parent;
      m_array[base+have[i]].dep=dep+1;
      m_edge[parent].push_back(have[i]);
    }
    for(int i=0,p=0;i<have.size();i++)
    {
      std::vector<int> son;
      while(p<vidx.size()&&m_vstr[vidx[p]][dep]==have[i])
      {
        if(m_vstr[vidx[p]].size()==dep+1)
        {
          if(m_array[base+have[i]].match)
          {
            OUTFOX_LOG((LP_WARNING,"Duplicated strings appeared %d %d\n",vidx[p],vidx[p-1]));
          }
          m_array[base+have[i]].match=true;
          m_array[base+have[i]].info=m_vinfo[vidx[p]];
        }
        else 
        {
          son.push_back(vidx[p]);
        }
        p++;
      }
      if(!construct_helper(son,base+have[i],dep+1))return false;
    }  
    return true;
  }
  bool construct_link()
  {
    std::queue<int> qq;
    m_array[0].fail=0;
    for(int i=0;i<m_edge[0].size();i++)
    {
      qq.push(m_edge[0][i]+m_array[0].base);
      m_array[m_edge[0][i]+m_array[0].base].fail=0;
    }
    while(qq.size())
    {
      int parent=qq.front();qq.pop();
    //  std::cerr<<"parent="<<parent<<std::endl;
      const int base=m_array[parent].base;
      std::vector<int>& have=m_edge[parent];
      for(int i=0;i<have.size();i++)
      {
        if(have[i]+base>=m_array.size()||m_array[base+have[i]].check!=parent)
        {
          OUTFOX_RETURN((LP_ERROR,"Internal error in construct_link\n"),false);
        }
      }
      for(int i=0;i<have.size();i++)
      {
        qq.push(base+have[i]);
      }
      if(parent==0)
      {
        for(int i=0;i<have.size();i++)
          m_array[base+have[i]].fail=0;
        continue;
      }
      for(int i=0;i<have.size();i++)
      {
        int fail=m_array[parent].fail;
        while(fail>0)
        {
          int next=(m_array[fail].base)+have[i];
          if(m_array[next].check==fail)break;
          fail=m_array[fail].fail;
        }
        int next=(m_array[fail].base)+have[i];
        m_array[base+have[i]].fail=(m_array[next].check==fail)?next:fail;
//        printf("node[%d].fail=%d\n",base+have[i],m_array[base+have[i]].fail);
      }
    }
    return true;
  }
public:
  void print()
  {
    std::cerr<<"size="<<m_array.size()<<std::endl;
    for(int i=0;i<m_array.size();i++)
    {
      if(m_array[i].check==-1)continue;
      std::cerr<<"cell #"<<i<<": "<<"dep="<<m_array[i].dep<<" check="<<m_array[i].check
        <<" base="<<m_array[i].base
        <<" fail="<<m_array[i].fail
          <<" match="<<m_array[i].match<<" info="<<m_array[i].info<<std::endl;
    }
  }
  ACAuto(uint size=1000):m_pArray(NULL),m_inited(false),m_last_cell(0)
  {
    size=std::max((uint)2,size);
    m_array.resize(size);
    m_edge.resize(size);
    //m_array[0] is reserved as the root node
    m_array[0].base=m_array[0].check=m_array[0].dep=m_array[0].fail=0;
    //m_array[1] is reserved as the free cell list header
    m_array[1].base=m_array[1].check=m_array[1].dep=m_array[1].fail=0;
    m_last_cell=2;
    for(int i=1;i<m_array.size();i++)
    {
      m_array[i].base=-(i+1);
      m_array[i].check=-(i-1);
    }
  };
    ~ACAuto(){};
  void insertString(const Char* ptr,int length,const Info& info)
  {
    m_vstr.push_back(std::vector<uint>(length));
    for(int i=0;i<length;i++)m_vstr.back()[i]=GetCharIndex(ptr[i]);
#ifdef DEBUG_OUTPUT
    for(int i=0;i<length;i++)std::cerr<<m_vstr.back()[i];
    std::cerr<<std::endl;
#endif
    m_vinfo.push_back(info);
  }
    // Check wether the AC automaton has been constructed
    inline bool IsInitialized() const
    {
    return (NULL != m_pArray && m_inited);
    }

  bool construct()
  try
  {
    std::vector<int> all(m_vstr.size());
    for(int i=0;i<m_vstr.size();i++)all[i]=i;
    std::cerr<<"Constructing "<<all.size()<<" words"<<std::endl;
    if(!construct_helper(all,0,0))
    {
      return false;
    }
    //std::cerr<<"Construct DA ended"<<std::endl;
    std::cerr<<"Finished constructing auto"<<std::endl;
    if(!construct_link())
    {
      return false;
    }
    m_pArray=&(m_array[0]);
    m_inited=true;
    // release vector's memory by the swap trick.
    std::vector<Info>().swap(m_vinfo);
    std::vector<std::vector<uint> >().swap(m_vstr);
    std::vector<std::vector<int> >().swap(m_edge);
    return true;
  }
  catch(std::bad_alloc)
  {
    OUTFOX_RETURN((LP_ERROR,"Construct ACAuto of %u words failed due to memoey alloc\n",m_vstr.size()),false);
  }
  catch(...)
  {
    OUTFOX_RETURN((LP_ERROR,"Construct ACAuto of %u words failed\n",m_vstr.size()),false);
  }
  int get_fail(int st)
  {
    if(st<0||st>=m_array.size())
    {
      OUTFOX_RETURN((LP_ERROR, "Illegal state:%d\n",st),0);
    }
    return m_pArray[st].fail;
  }
  int getNext(int st,Char ch)
  {
    if(st<0||st>=m_array.size())
    {
      OUTFOX_RETURN((LP_ERROR, "Illegal state:%d\n",st),0);
    }
    const int idx=getCharIndex(ch);
    if(idx<0)return 0;
    while(st)
    {
      int pos=m_pArray[st].base+idx;
      if(m_pArray[pos].check==st)return pos;
      st=m_pArray[st].fail;
    }
    if(st)return st;
    return (m_pArray[m_pArray[0].base+idx].check==0)?m_pArray[0].base+idx:0;
  }
  int getDep(int st)
  {
    if(st<0||st>=m_array.size())
    {
      OUTFOX_RETURN((LP_ERROR, "Illegal state:%d\n",st),0);
    }
    return m_pArray[st].dep;
  }
  int getFail(int st)
  {
    if(st<0||st>=m_array.size())
    {
      OUTFOX_RETURN((LP_ERROR, "Illegal state:%d\n",st),0);
    }
    return m_pArray[st].fail;
  }
  const Info& getInfo(int st)
  {
    if(st<0||st>=m_array.size())
    {
      OUTFOX_RETURN((LP_ERROR, "Illegal state:%d\n",st),m_pArray[0].info);
    }
    return m_pArray[st].info;
  }
  void print_status()
  {
    std::cerr<<"array capacity="<<m_array.capacity()<<std::endl;
    std::cerr<<"array size="<<m_array.size()<<std::endl;
    int used=0;
    for(int i=0;i<m_array.size();i++)
    {
      if(m_array[i].check!=-1)used++;
    }
    std::cerr<<"Used cell="<<used<<std::endl;

  };
  bool isMatched(int st)
  {
    return m_pArray[st].match;
  };
};
int main()
{
  srand(23123);
  ACAuto<short,int> tree(10000);
  short buf[100];
  set<vector<short> > mem;
  vector<vector<short> > all;
  for(int i=0;i<300000;i++)
  {
    int len=rand()%10+1;
    for(int j=0;j<len;j++)
    {
      int x=rand()%(1<<16);
      buf[j]=x;
    }
    if(mem.count(vector<short>(buf,buf+len)))continue;
    mem.insert(vector<short>(buf,buf+len));
    tree.insertString(buf,len,all.size());
    all.push_back(vector<short>(buf,buf+len));
  }
  const double start=get_wall_time();
  if(!tree.construct())
  {
    cerr<<"Error"<<endl;
    return 0;
  }
  cerr<<get_wall_time()-start<<endl;
  tree.print_status();
  int st=0;
  vector<short> seq;
  for(int i=0;i<100000;i++)
  {
    short x=rand()%(1<<16);
    st=tree.getNext(st,x);
    seq.push_back(x);
    int cnt=0;
    //for(int s=st;s;s=tree.getFail(s))
    for(int s=st;s;s=tree.getFail(s))
    {
      if(tree.isMatched(s))
      {
        int pos=tree.getInfo(s);
        int len=tree.getDep(s);
        cnt++;
//        cerr<<i<<" "<<s<<" "<<len<<" "<<pos<<" "<<all[pos].size()<<endl;
        assert(len==all[pos].size());
        for(int j=0;j<len;j++)assert(seq[i-j]==all[pos][len-1-j]);
      }
    }
    for(int len=1;len<=i+1&&len<=10;len++)
    {
      vector<short> pat=vector<short>(seq.begin()+i-len+1,seq.begin()+i+1);
      if(mem.count(pat))cnt--;
    }
    assert(cnt==0);
  }
  return 0;
}

#endif
