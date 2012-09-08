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
#include <sys/time.h>
using namespace std;
//#ifdef ZDEBUG
#define OUTFOX_RETURN(x,y) fprintf x;return y;
#define OUTFOX_LOG(x) fprintf x;
#define LP_ERROR stderr
#define LP_WARNING stderr
//#endif

// Note that duplicate strings will be discarded.
//将Char影射为一个int的模板类。
template<typename T>
class Convertor
{
	static const int Hsize=(sizeof(T)>=4||(1<<16)<(1<<(sizeof(T)*8)))?(1<<16):(1<<(8*sizeof(T)));
	int hash[Hsize];
	std::map<T,int> mem;
public:
	int cnt;
	Convertor():cnt(0){memset(hash,0xff,sizeof(hash));};
	//未建立影射的T现建立影射再返回
	inline int GetCharIndex(T chr)
	{
		int ch=(int)chr;
		if(ch>=0&&ch<(int)sizeof(hash))
		{
			if(-1==hash[ch])hash[ch]=cnt++;
			return hash[ch];
		}
		else
		{
			if(mem.count(ch)==0)mem[ch]=cnt++;
			return mem[ch];
		}
	}
	inline int getCharIndex(T ch)const
	{
		if(ch>=0&&ch<(int)sizeof(hash))
		{
			return hash[(int)ch];
		}
		else
		{
			return mem.count(ch)?mem.find(ch)->second:-1;
		}
	}
    int dump(FILE* fout)const
    {
        if(!fout)return -1;
        size_t mem_size=mem.size();
        if(1!=fwrite(&cnt,sizeof(cnt),1,fout)||
            Hsize!=fwrite(hash,sizeof(hash[0]),Hsize,fout)||
            1!=fwrite(&mem_size,sizeof(mem_size),1,fout))
        {
            return -1;
        }
        for(typename std::map<T,int>::iterator itr=mem.begin();itr!=mem.end();itr++)
        {
            if(1!=fwrite(&(itr->first),sizeof(itr->first),1,fout)||
                1!=fwrite(&(itr->second),sizeof(itr->second),1,fout))return -1;
        }
        return 0;
    }
    int load(FILE* fin)
    {
        if(!fin)return -1;
        size_t mem_size=0;
        if(1!=fread(&cnt,sizeof(cnt),1,fin)||
            Hsize!=fread(hash,sizeof(hash[0]),Hsize,fin)||
            1!=fread(&mem_size,sizeof(mem_size),1,fin))
        {
            return -1;
        }
        mem.clear();
        for(size_t i=0;i<mem_size;i++)
        {
            T a;
            int b;
            if(1!=fread(&a,sizeof(a),1,fin)||1!=fread(&b,sizeof(b),1,fin))return -1;
            assert(0==mem.count(a));
            assert(b<cnt&&b>=0);
            mem[a]=b;
        }
        return 0;
    }
};
//特化类，避免编译warning和不必要的比较开销
template<>
class Convertor<unsigned short>
{
	static const int Hsize=(1<<16);
	int hash[Hsize];
	std::map<unsigned short,int> mem;
public:
	int cnt;
	Convertor():cnt(0){memset(hash,0xff,sizeof(hash));};
	inline int GetCharIndex(unsigned short ch)
	{
		if(-1==hash[ch])hash[ch]=cnt++;
		return hash[ch];
	}
	inline int getCharIndex(unsigned short ch)const
	{
		// The Char is unsigned short for this application
		return hash[ch];
	}
    int dump(FILE* fout)
    {
        if(!fout)return -1;
        size_t mem_size=mem.size();
        if(1!=fwrite(&cnt,sizeof(cnt),1,fout)||
            Hsize!=(int)fwrite(hash,sizeof(hash[0]),Hsize,fout)||
            1!=fwrite(&mem_size,sizeof(mem_size),1,fout))
        {
            return -1;
        }
        for(std::map<unsigned short,int>::const_iterator itr=mem.begin();itr!=mem.end();itr++)
        {
            if(1!=fwrite(&(itr->first),sizeof(itr->first),1,fout)||
                1!=fwrite(&(itr->second),sizeof(itr->second),1,fout))return -1;
        }
        return 0;
    }
    int load(FILE* fin)
    {
        if(!fin)return -1;
        size_t mem_size=0;
        if(1!=fread(&cnt,sizeof(cnt),1,fin)||
            Hsize!=(int)fread(hash,sizeof(hash[0]),Hsize,fin)||
            1!=fread(&mem_size,sizeof(mem_size),1,fin))
        {
            return -1;
        }
        mem.clear();
        for(size_t i=0;i<mem_size;i++)
        {
            unsigned short a;
            int b;
            if(1!=fread(&a,sizeof(a),1,fin)||1!=fread(&b,sizeof(b),1,fin))return -1;
            assert(0==mem.count(a));
            assert(b<cnt&&b>=0);
            mem[a]=b;
        }
        return 0;
    }

};

//#define DEBUG_OUTPUT
//The constructor of Info will be used
template<typename Char,typename Info>
class ACAuto
{
	public:
	typedef unsigned int uint;
	void removeCell(int pos)
	{
		if(pos<0||pos>=(int)m_array.size())
		{
			//OUTFOX_LOG((LP_ERROR,"Illegal empry cell position %d\n",pos));
      assert(0);
			return ;
		}
		assert(m_array[pos].check<0);
		assert(m_array[pos].base<0);
		int pre=-m_array[pos].check;
		int next=-m_array[pos].base;
		m_array[pre].base=-next;
		assert(m_array[pre].base<0);
		if(next<(int)m_array.size())m_array[next].check=-pre;
		m_array[pos].check=0;
	//	printf("remove %d\n",pos);
	//	print();
		assert(m_array[1].base<0);
	}
	void enlarge(uint size)
	{
		if(m_array.size()>=size)
		{
			//OUTFOX_LOG((LP_ERROR,"Unnecessary enlarge m_array"));
      assert(0);
			return;
		}
		int tail=m_array.size()-1,head=1;
		for(;m_array[tail].check>=0&&-m_array[head].base<(int)m_array.size();)
		{
			assert(m_array[head].base<0);
			tail--;
			head=-m_array[head].base;
		}
		int pre=(m_array[tail].check<0)?tail:head;
		assert(pre==1||m_array[pre].check<0);
		assert(-m_array[pre].base==(int)m_array.size());
		const int old_size=(int)m_array.size();
		m_array.resize(size);
		for(int i=old_size;i<(int)m_array.size();i++)
		{
			m_array[pre].base=-i;
			m_array[i].check=-pre;
			pre=i;
		}
		m_array.back().base=-size;
	//	puts("enlarge");
	//	print();
		assert(m_array[1].base<0);
	}
	Convertor<Char> m_convertor;
	inline int getCharIndex(Char ch)const{return m_convertor.getCharIndex(ch);};
	inline int GetCharIndex(Char ch){return m_convertor.GetCharIndex(ch);};

	//If check<0, then -check points ot the previous empty cell
	//If base<0, then -base points to the next empty cell
	struct AC_node
	{
		int check,base,fail,dep,match_link;
		bool match;
		Info info;
		AC_node():check(-1),match(false){};
	};
	
	const AC_node* m_pArray;
	bool m_inited;
	int m_last_cell;
	size_t m_size;
	std::vector<AC_node> m_array;
	std::vector<std::vector<uint> > m_vstr;
	std::vector<Info> m_vinfo;
	std::vector<std::vector<int> > m_edge;

    int dump(FILE* fout)
    {
        if(!m_inited)return -2;
        if(m_convertor.dump(fout))return -1;
        assert(m_size==m_array.size());
        if(1!=fwrite(&m_size,sizeof(m_size),1,fout)||
            1!=fwrite(&m_last_cell,sizeof(m_last_cell),1,fout))
            return -1;
        if(m_size!=fwrite(m_pArray,sizeof(m_pArray[0]),m_size,fout))return -1;
        return 0;
    }
    int load(FILE* fin)
    {
        if(m_convertor.load(fin))return -3;
        if(1!=fread(&m_size,sizeof(m_size),1,fin)||
            1!=fread(&m_last_cell,sizeof(m_last_cell),1,fin))
            return -1;
        //printf("m_size=%llu m_array.size()=%d\n",(unsigned long long)m_size,(int)m_array.size());
        try
        {
            m_array.resize(m_size);
        }
        catch(...)
        {
            return -2;
        }
        m_pArray=&m_array[0];
        AC_node* tmp_pointer=&m_array[0];
        if(m_size!=fread(tmp_pointer,sizeof(m_pArray[0]),m_size,fin))return -1;
        m_inited=true;
        return 0;
    }
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
		assert(parent<(int)m_array.size());
#ifdef DEBUG_OUTPUT
		std::cerr<<"constructing dep "<<dep<<" size="<<vidx.size()<<std::endl;
		for(int i=0;i<(int)vidx.size();i++)std::cerr<<vidx[i]<<" ";
		std::cerr<<std::endl;
#endif
		if(vidx.empty())
		{
			m_array[parent].base=0;
			return true;
		}
		for(int i=0;i<(int)vidx.size();i++)
		{
			if((int)m_vstr[vidx[i]].size()<=dep)
			{
				//OUTFOX_RETURN((LP_ERROR,"Illeqal dep=%d idx=%d",dep,vidx[i]),false);
        assert(0);
			}
		}
		std::sort(vidx.begin(),vidx.end(),comparator(dep,this));
		std::vector<int> have;
		have.push_back(m_vstr[vidx[0]][dep]);
		for(int i=1;i<(int)vidx.size();i++)
		{
			if(m_vstr[vidx[i-1]][dep]!=m_vstr[vidx[i]][dep])
			{
				have.push_back(m_vstr[vidx[i]][dep]);
			}
		}
		//while(m_last_cell<m_array.size()&&m_array[m_last_cell].check!=-1)m_last_cell++;
		int base;
		for(int cell=-m_array[1].base;;cell=-m_array[cell].base)
		{
			assert(cell!=1);
			base=std::max(cell-have[0],0);
			bool ok=false;
			if(base+m_convertor.cnt>(int)m_array.size())enlarge(base+m_convertor.cnt);
			ok=true;
			for(int i=0;i<(int)have.size();i++)
			{
				if(m_array[base+have[i]].check>=0)
				{
					ok=false;
					break;
				}
			}
			if(ok)break;
		}
		m_array[parent].base=base;
		assert(m_array[parent].check>=0);
		assert(base>=0);
#ifdef DEBUG_OUTPUT
		std::cerr<<"Find base "<<base<<" for node "<<parent<<" with children ";
		for(int i=0;i<(int)have.size();i++)std::cerr<<have[i]<<" ";std::cerr<<std::endl;
#endif
		if((int)m_edge.size()<=parent)m_edge.resize(parent+1);

		for(int i=0;i<(int)have.size();i++)
		{
			removeCell(base+have[i]);
			m_array[base+have[i]].check=parent;
			m_array[base+have[i]].dep=dep+1;
			m_edge[parent].push_back(have[i]);
#ifdef DEBUG_OUTPUT
			printf("%d--->%d\n",parent,have[i]);
#endif
		}
		for(int i=0,p=0;i<(int)have.size();i++)
		{
			std::vector<int> son;
			while(p<(int)vidx.size()&&(int)m_vstr[vidx[p]][dep]==have[i])
			{
				if((int)m_vstr[vidx[p]].size()==dep+1)
				{
					if(m_array[base+have[i]].match)
					{
						//OUTFOX_LOG((LP_WARNING,"Duplicated strings appeared %d %d\n",vidx[p],vidx[p-1]));
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
		m_array[0].fail=m_array[0].match_link=0;
		for(int i=0;i<(int)m_edge[0].size();i++)
		{
			qq.push(m_edge[0][i]+m_array[0].base);
			m_array[m_edge[0][i]+m_array[0].base].fail=0;
			m_array[m_edge[0][i]+m_array[0].base].match_link=0;
		}
		while(qq.size())
		{
			const int parent=qq.front();qq.pop();
		//	std::cerr<<"parent="<<parent<<std::endl;
			const int base=m_array[parent].base;
			// leaf node
			if((int)m_edge.size()<=parent) continue;
			std::vector<int>& have=m_edge[parent];
#ifdef DEBUG_OUTPUT
			printf("have[%d] size=%d\n",parent,have.size());
#endif
			for(int i=0;i<(int)have.size();i++)
			{
				if(have[i]+base>=(int)m_array.size()||m_array[base+have[i]].check!=parent)
				{
					//OUTFOX_RETURN((LP_ERROR,"Internal error in construct_link\n"),false);
          assert(0);
				}
			}
			for(int i=0;i<(int)have.size();i++)
			{
				qq.push(base+have[i]);
			}
			for(int i=0;i<(int)have.size();i++)
			{
#ifdef DEBUG_OUTPUT
				printf("find fail %d--->%d\n",parent,base+have[i]);
#endif
				const int son=base+have[i];
				int fail=m_array[parent].fail;
				int next=0;
				while(fail>0)
				{
					next=(m_array[fail].base)+have[i];
					if(m_array[next].check==fail)break;
					fail=m_array[fail].fail;
				}
				if(0==fail)
				{
					next=(m_array[0].base)+have[i];
					m_array[son].fail=(m_array[next].check==fail)?next:0;
				}
				else 
				{
					m_array[son].fail=next;
				}
				int pre=m_array[son].fail;
				if(m_array[pre].match)m_array[son].match_link=pre;
				else m_array[son].match_link=m_array[pre].match_link;
#ifdef DEBUG_OUTPUT
				printf("node[%d].fail=%d\n",base+have[i],m_array[base+have[i]].fail);
#endif
			}
		}
		return true;
	}
public:
	void print()
	{
		std::cerr<<"size="<<m_array.size()<<std::endl;
		int pre=-1;
		for(int i=0;i<(int)m_array.size();i++)
		{
			if(m_array[i].check>=0)continue;
			
			std::cerr<<"cell #"<<i<<": "<<"dep="<<m_array[i].dep<<" check="<<m_array[i].check
				<<" base="<<m_array[i].base
				<<" fail="<<m_array[i].fail
					<<" match="<<m_array[i].match<<" info="<<m_array[i].info<<std::endl;
			if(pre!=-1)
			{
				assert(-m_array[pre].base==i);
			}
			pre=i;
		}
		std::cerr<<"---------------------"<<std::endl;
		for(int i=0;i<(int)m_array.size();i++)
		{
			if(m_array[i].check<0)continue;
			
			std::cerr<<"cell #"<<i<<": "<<"dep="<<m_array[i].dep<<" check="<<m_array[i].check
				<<" base="<<m_array[i].base
				<<" fail="<<m_array[i].fail
					<<" match="<<m_array[i].match<<" match_link="<<m_array[i].match_link<<" info="<<m_array[i].info<<std::endl;
		}
	}
	ACAuto(uint size=2):m_pArray(NULL),m_inited(false),m_last_cell(0),m_array(std::max((uint)2,size))
	{
		size=std::max((uint)2,size);
		m_array[0].base=m_array[0].check=m_array[0].dep=m_array[0].fail=0;
		m_last_cell=2;
		for(int i=1;i<(int)m_array.size();i++)
		{
			m_array[i].base=-(i+1);
			m_array[i].check=-(i-1);
		}
		m_array[1].check=-1;
	};
    ~ACAuto(){};
	bool insertString(const Char* ptr,int length,const Info& info)
	{
		m_vstr.push_back(std::vector<uint>(length));
		for(int i=0;i<length;i++)m_vstr.back()[i]=GetCharIndex(ptr[i]);
#ifdef DEBUG_OUTPUT
		for(int i=0;i<length;i++)std::cerr<<m_vstr.back()[i];
		std::cerr<<std::endl;
#endif
		m_vinfo.push_back(info);
		return true;
	}
    // Check wether the AC automaton has been constructed
    inline bool IsInitialized() const
    {
		return (NULL != m_pArray && m_inited);
    }

	bool construct()
	try
	{
		if(m_vstr.empty())return true;
		std::vector<int> all(m_vstr.size());
		for(int i=0;i<(int)m_vstr.size();i++)all[i]=i;
#ifdef DEBUG_OUTPUT
		std::cerr<<"Constructing "<<all.size()<<" words"<<std::endl;
#endif
		if(!construct_helper(all,0,0))
		{
			return false;
		}
		//std::cerr<<"Construct DA ended"<<std::endl;
#ifdef DEBUG_OUTPUT
		std::cerr<<"Finished constructing auto"<<std::endl;
#endif
		if(!construct_link())
		{
			return false;
		}
		m_pArray=&(m_array[0]);
		m_inited=true;
		m_size=m_array.size();
		// release vector's memory by the swap trick.
		std::vector<Info>().swap(m_vinfo);
		std::vector<std::vector<uint> >().swap(m_vstr);
		std::vector<std::vector<int> >().swap(m_edge);
		return true;
	}
	catch(std::bad_alloc)
	{
		//OUTFOX_RETURN((LP_ERROR,"Construct ACAuto of %zu words failed due to memoey alloc\n",m_vstr.size()),false);
      assert(0);
	}
	catch(...)
	{
		//OUTFOX_RETURN((LP_ERROR,"Construct ACAuto of %zu words failed\n",m_vstr.size()),false);
      assert(0);
	}
	inline int goNext(int st,Char ch)const
	{
		if(st<0||st>=(int)m_size)
		{
			//OUTFOX_RETURN((LP_ERROR, "Illegal state:%d\n",st),0);
      assert(0);
		}
		const int idx=getCharIndex(ch);
		if(idx<0)return -1;
		int pos=m_pArray[st].base+idx;
		return (m_pArray[pos].check==st)?pos:-1;
	}
	inline int getNext(int st,Char ch)const
	{
		if(st<0||st>=(int)m_size)
		{
			//OUTFOX_RETURN((LP_ERROR, "Illegal state:%d\n",st),0);
      assert(0);
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
	inline int getDep(int st)const
	{
		if(st<0||st>=(int)m_size)
		{
			//OUTFOX_RETURN((LP_ERROR, "Illegal state:%d\n",st),0);
      assert(0);
		}
		return m_pArray[st].dep;
	}
	inline int getFail(int st)
	{
		if(st<0||st>=(int)m_size)
		{
			//OUTFOX_RETURN((LP_ERROR, "Illegal state:%d\n",st),0);
      assert(0);
		}
		return m_pArray[st].fail;
	}
	inline int getMatchlink(int st)const
	{
		if(st<0||st>=(int)m_size)
		{
			//OUTFOX_RETURN((LP_ERROR, "Illegal state:%d\n",st),0);
      assert(0);
		}
		return m_pArray[st].match_link;
	}
	const Info& getInfo(int st)const
	{
		if(st<0||st>=(int)m_size)
		{
			//OUTFOX_RETURN((LP_ERROR, "Illegal state:%d\n",st),m_pArray[0].info);
      assert(0);
		}
		return m_pArray[st].info;
	}
	Info& getInfo_ref(int st)
	{
		if(st<0||st>=(int)m_size)
		{
			//OUTFOX_RETURN((LP_ERROR, "Illegal state:%d\n",st),m_pArray[0].info);
      assert(0);
		}
		return m_pArray[st].info;
	}
	void print_status()
	{
		std::cerr<<"array capacity="<<m_array.capacity()<<std::endl;
		std::cerr<<"array size="<<m_array.size()<<std::endl;
		int used=0;
		for(int i=0;i<(int)m_array.size();i++)
		{
			if(m_array[i].check>=0)used++;
		}
		std::cerr<<"Used cell="<<used<<std::endl;

	};
	bool isMatched(int st)const
	{
		return m_pArray[st].match;
	};
	size_t cell_size()
	{
		return m_array.size();
	};
	size_t used_cell()
	{
		size_t used=0;
		for(int i=0;i<(int)m_array.size();i++)
			if(m_array[i].check>=0)used++;
		return used;
	};
	size_t used_mem()
	{
		return m_array.capacity()*sizeof(AC_node);
	};
};


inline double get_wall_time()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + 1e-6 * tv.tv_usec;
}

int main()
{
	unsigned int seed=time(NULL);
//	seed=1280222720;
	srand(seed);
	cerr<<seed<<endl;
	ACAuto<short,int> tree(1);
//	ACAuto<char,int> tree(1);
//	tree.insertString("010",3,1);
//	tree.insertString("10",2,2);
//	tree.insertString("01",2,3);
//	assert(tree.construct());
//	tree.print();	
	short buf[100];
	set<vector<short> > mem;
	vector<vector<short> > all;
	const int wlen=20;
	int base=6;
	for(int i=0;i<1000000;i++)
	{
		int len=rand()%wlen+1;
		for(int j=0;j<len;j++)
		{
			int x=rand()%(base);
			buf[j]=x;
		}
		if(mem.count(vector<short>(buf,buf+len)))continue;
		mem.insert(vector<short>(buf,buf+len));
		tree.insertString(buf,len,all.size());
		all.push_back(vector<short>(buf,buf+len));
		//for(int j=0;j<len;j++)printf("%d",buf[j]);puts("");
	}
	double start=get_wall_time();
	if(!tree.construct())
	{
		cerr<<"Error"<<endl;
		return 0;
	}
	cerr<<"constructing time="<<get_wall_time()-start<<endl;
//	tree.print();
	tree.print_status();
//	tree.print();
	int st=0;
	vector<short> seq;
	for(int i=0;i<1000000;i++)
	{
		short x=rand()%(base+1);
//		printf("%d",x);
		st=tree.getNext(st,x);
		seq.push_back(x);
		int cnt=0;
		//for(int s=st;s;0s=tree.getFail(s))
		int pre=10000;
		vector<int> match;
		for(int s=st;s;s=tree.getFail(s))
		{
			if(tree.isMatched(s))
			{
				int pos=tree.getInfo(s);
				int len=tree.getDep(s);
				cnt++;
//				cerr<<i<<" "<<s<<" "<<len<<" "<<pos<<" "<<all[pos].size()<<endl;
				assert(len==all[pos].size());
				for(int j=0;j<len;j++)assert(seq[i-j]==all[pos][len-1-j]);
				assert(len<pre);
				pre=len;
				match.push_back(s);
			}
		}
		reverse(match.begin(),match.end());
		for(int s=tree.isMatched(st)?st:tree.getMatchlink(st);s;s=tree.getMatchlink(s))
		{
			assert(match.size());
			assert(s==match.back());
			match.pop_back();

		}
		assert(match.empty());
		for(int len=1;len<=i+1&&len<=wlen;len++)
		{
			vector<short> pat=vector<short>(seq.begin()+i-len+1,seq.begin()+i+1);
			if(mem.count(pat))cnt--;
		}
		if(cnt)
		{
			cerr<<cnt<<endl;
			assert(cnt==0);
		}
	}

	return 0;
}

