#include <cstdarg>
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
#include <cassert>
#include <iterator>
#include <limits>
#include <valarray>
using namespace std;
//** Start of Macro and typedef 
#ifndef NDEBUG
    #define debug(x) cerr<<#x<<"=\""<<x<<"\""<<" at line#"<<__LINE__<<endl;
    #define hline() cerr<<"-----------------------------------------"<<endl;
#else
    #define debug(x)
    #define hline()
#endif
#define Throw(x) fprintf(stderr,"%s at [line:%d] [file:%s]\n",x,__LINE__,__FILE__);throw x;
typedef long long int llint;
#define Low(x) ((((x)^((x)-1))&x))
#define Two(x)  ((1)<<(x))
#define PB(x) push_back((x))
#define SORT(x) sort(x.begin(),x.end())
#define GOOD(grid,r,c) (((r)>=0&&(r)<grid.size())&&((c)>=0&&(c)<grid[0].length()))
#define BCNT(x) __builtin_popcount(x)
#define Min(out, ... ) do{typeof(out) X[]={__VA_ARGS__};out=*min_element(X,X+sizeof(X)/sizeof(X[0]));}while(0)
#define Max(out, ... ) do{typeof(out) X[]={__VA_ARGS__};out=*max_element(X,X+sizeof(X)/sizeof(X[0]));}while(0)
#define uMin(out, ... ) do{typeof(out) X[]={out,__VA_ARGS__};out=*min_element(X,X+sizeof(X)/sizeof(X[0]));}while(0)
#define uMax(out, ... ) do{typeof(out) X[]={out,__VA_ARGS__};out=*max_element(X,X+sizeof(X)/sizeof(X[0]));}while(0)

#define Back(str) (str)[(str).length()-1]
#define SZ(vec) ((int)(vec).size())
#define All(vec) (vec).begin(),(vec).end()
#define Rep(i,n) for(int i=0;i<(n);++i)
#define For(i,a,b) for(typeof(a) i=a;i<b;++i)
#define Bit(x,b) (1&((x)>>(b)))
#define Foreach(itr,con) typedef typeof(con) _type_of_ ## con; for(_type_of_ ## con::iterator itr=con.begin();itr!=con.end();++itr)
#define Make_vector(vec,...) do{typedef typeof(vec) _T;const _T::value_type _tmp[]={ __VA_ARGS__ };vec=_T(_tmp,_tmp+sizeof(_tmp)/sizeof(_tmp[0]));}while(0)
#define Union(out,...) do{typeof(out) _v; Make_vector(_v,__VA_ARGS__);out=interval_union(_v);}while(0)
static const int bitindex_map[32]={};
#define BitIndex(x) (((unsigned int)(x)*0x077CB531U)>>27)
//***************End of Macro and typedef**************

const int dir[][2]={{-1,0},{0,1},{1,0},{0,-1},{1,1},{1,-1},{-1,1},{-1,-1}};
const char dname[]="NWSE";
//const char dname[]="URDL";
// Tool functions
template<typename T>
inline void updateMax(T& a,const T& b){a=max(a,b);}
template<typename T>
inline void updateMin(T& a,const T& b){a=min(a,b);}
template<typename T>
inline vector<T> erase(vector<T> table,int ind)
{
	assert(ind<table.size());
	table.erase(table.begin()+ind);
	return table;
}
template<typename T>
vector<T> unique(vector<T> table)
{
	SORT(table);
  vector<T> res;
  unique_copy(table.begin(),table.end(),back_inserter(res));
  return res;
	//return vector<T>(table.begin(),unique(table.begin(),table.end()));
}
template<typename A,typename B>
ostream& operator<<(ostream& out,const pair<A,B>& pp)
{
	out<<"("<<pp.first<<","<<pp.second<<")";
	return out;
}
template<typename A,typename B>
istream& operator<<(istream& in,pair<A,B>& pp)
{
	cerr<<"A pair wanted"<<endl;
	in>>pp.first>>pp.second;
	return in;
}

template<typename T>
ostream& operator<<(ostream& out,const vector<T>& vect)
{
  copy(vect.begin(),vect.end(),ostream_iterator<T>(out," "));
  return out;
	out<<"length = "<<vect.size()<<endl;
	for(int i=0;i<vect.size();i++)out<<vect[i]<<" ";
	out<<endl;
	return out;
}
ostream& operator<<(ostream& out,const vector<string>& vect)
{
	out<<vect.size()<<" X "<<vect[0].length()<<endl;
	for(int i=0;i<vect.size();i++)out<<vect[i]<<endl;
	return out;
}
template<typename T>
istream& operator>>(istream& in,vector<T>& vect)
{
	vect.clear();
	int n;
	cerr<<"A integer of length wanted"<<endl;
	in>>n;
	vect.resize(n);
	cerr<<n<<" elements wanted"<<endl;
	for(int i=0;i<n;i++)in>>vect[i];
	return in;
}

template<typename T>
ostream& operator<<(ostream& out,const vector<vector<T> >& vect)
{
	out<<"row number="<<vect.size()<<endl;
	for(int i=0;i<vect.size();i++)
	{
		out<<"row #"<<i<<":";
		for(int j=0;j<vect[i].size();j++)
			out<<" "<<vect[i][j];
		out<<endl;
	}
	return out;
}
template<typename T>
istream& operator>>(istream& in,vector<vector<T> >& vect)
{
	vect.clear();
	int n,m;
	cerr<<"Two integers wanted"<<endl;
	in>>n>>m;
	vect.resize(n);
	cerr<<"A matrix "<<n<<" X "<<m<<" wanted"<<endl;
	for(int i=0;i<n;i++)
	{
		vect[i].resize(m);
		for(int j=0;j<m;j++)in>>vect[i][j];
	}
	return in;
}



// Convert anything to string
template<class T>
string convert(T vv)
{
	ostringstream re;
	re<<vv;
	return re.str();
}
template<typename T>
T convert(const string& ss)
{
	istringstream is(ss);
	T tmp;
	is>>tmp;
	return tmp;
}
//convert vector to string
template<class T>
string convert(vector<T> vv)
{
	ostringstream re;
	for(int i=0;i<vv.size();i++)
	{
		if(i)re<<" ";
		re<<vv[i];
	}
	return re.str();
}

//Be carefull for cut into strings!!!!
template<class T>
vector<T> parse(const string& ss,const char* cut=" ")
{
	vector<T> re;
	for(int j=0;j<ss.size();j++)
	{
		string s;
		while(j<ss.size()&&NULL==strchr(cut,ss[j]))
			s+=ss[j++];
		if(!s.empty())
		{
			T tmp;
			istringstream is(s);
			is>>tmp;
			re.push_back(tmp);
		}
	}
	return re;
}
template<>
vector<string> parse(const string& ss,const char* cut)
{
	vector<string> re;
	for(int j=0;j<ss.size();j++)
	{
		string s;
		while(j<ss.size()&&NULL==strchr(cut,ss[j]))
			s+=ss[j++];
		if(!s.empty())
		{
			re.push_back(s);
		}
	}
	return re;
}
/************ bitwise functions  ************/
int countBit(int n)
{
  return __builtin_popcount(n);
	int re=0;
	while(n)re++,n^=Low(n);
	return re;
}
// Most significant bit
int MSB(int n)
{
	if(n==0)return 0;
	while(Low(n)!=n)n^=Low(n);
	return n;
}
void initHash(int n,int hash[])
{
	for(int i=0;i<n;i++)
		hash[Two(i)]=i;
}
void initBcnt(int n,int bcnt[])
{
	bcnt[0]=0;
	for(int i=1;i<Two(n);i++)
		bcnt[i]=bcnt[i^Low(i)]+1;
}
int print(int aa[]) {
	return 0;
}

template<typename T>
vector<pair<T,T> >  interval_union(vector<pair<T,T> > all) {
	vector<pair<T,T> > res;
	if(all.empty())return res;
	sort(all.begin(),all.end());
	res.push_back(all[0]);
	for(int i=1;i<all.size();i++) {
		if(all[i].first<=res.back().second) {
			res.back().second=max(res.back().second,all[i].second);
		}
		else {
			res.push_back(all[i]);
		}
	}
	return res;
}
template<typename T>
class CPoint : public complex<T> {
  public:
    CPoint(const T& x=T(),const T& y=T()):complex<T>(x,y){};
    T& x() {
      return complex<T>::real();
    }
    T& y() {
      return complex<T>::imag();
    }
};
template<typename T>
vector<T> set_diff(vector<T>& a,vector<T>& b) {
  vector<T> c;
  sort(a.begin(),a.end());
  sort(b.begin(),b.end());
  set_difference(a.begin(),a.end(),b.begin(),b.end(),inserter(c,c.begin()));
  return c;
}
template<typename T>
ostream& operator<<(ostream& out,const valarray<T>& val) {
  for(int i=0;i<val.size();i++) {
    cout<<val[i]<<" ";
  }
  return out;
}
vector<string> transpose(const vector<string>& grid) {
  vector<string> res((grid[0].size()),string((grid.size()),' '));
  for(int i=0;i<grid.size();i++)for(int j=0;j<grid[i].size();j++) {
    res[j][i]=grid[i][j];
  }
  return res;
}
double nextGaussian()
{
    double x,y,r;
    for(;;)
    {
        x=2.0*rand()/(RAND_MAX+1.0)-1.0;
        y=2.0*rand()/(RAND_MAX+1.0)-1.0;
        r=x*x+y*y;
        if(r>=1.0)continue;
        break;
    }
    double re=sqrt(-2.0*log(r)/(r))*x;
    return re;
}
template<typename T>
bool next_permutation(T ptr[],int n) {
  for(int i=n-1;i;i--) if(ptr[i]>ptr[i-1]) {
    int b=i;
    while(b+1<n&&ptr[b+1]>ptr[i-1])b++;
    swap(ptr[i-1],ptr[b]);
    reverse(ptr+i,ptr+n);
    return true;
  }
  return false;
}
//Generate permutation group of a cube.
vector<vector<int> > generateCubePermutation() {
  vector<int> org(6);
  for(int i=0;i<6;i++)org[i]=i;
  vector<vector<int> > res;
  set<vector<int> > mem;
  const int rot[][4]={
    {2,3,4,5},{0,2,1,4},{0,3,1,5}
  };
  for(int a=0;a<4;a++)for(int b=0;b<4;b++)for(int c=0;c<4;c++) {
    //a=0;b=1;c=0;
    vector<int> per=org;
    int times[3]={a,b,c};
    int tmp[4];
    for(int i=0;i<3;i++) {
      for(int j=0;j<4;j++) {
        tmp[j]=per[rot[i][(j+times[i])%4]];
      }
      for(int j=0;j<4;j++) {
        per[rot[i][j]]=tmp[j];
      }
    }
    if(mem.count(per)==0) {
      res.push_back(per);
      mem.insert(per);
      //debug(per[0]);
    }
  }
  //debug(mem.size());
  assert(mem.size()==24);
  return res;
}
int main()
{
  valarray<int> v1(1,10),v2(5,10);
  valarray<int> vv=v1+v2*v2+v2*10-100;
  debug(vv);
  v1[slice(1,2,3)]=-100;
  debug(v1);
  v1=-1000;
  debug(v1);
	typedef pair<int,int> ppt;
	vector<ppt> un;
	Union(un,ppt(1,2),ppt(0,1),ppt(3,4));
	assert(un.size()==2);
	assert(un[0]==ppt(0,2));
	assert(un[1]==ppt(3,4));
	int t;
	Max(t,1,2,3,4,5);
	debug(t);
	assert(t==5);
	double tt;
	Max(tt,1.1,2,3,4,-1.1);
	debug(tt);
	assert(tt==4);
	debug(BCNT(1023));
	unsigned int magic=0x077CB531U;
	for(unsigned int i=(1U)<<31;i>0;i>>=1) 
		cout<<((magic&i)?1:0);
	cout<<endl;
	//for(int i=0;i<10;i++)assert(BitIndex((1<<i))==i);
  vector<int> a,b;
  Make_vector(a,1,-1,0,-2,-1,-3,4,5);
  Make_vector(b,0,-1,-3,4,100,102);
  vector<int> c=set_diff(a,b);
  debug(c);
  assert(c.size()==4);
  assert(c[0]==-2);
  assert(c[1]==-1);
  assert(c[2]==1);
  assert(c[3]==5);
  debug(numeric_limits<int>::max());
  Make_vector(a,1,2,3,4,5);
  set<int> mem(All(a));
  Foreach(itr,mem)cerr<<*itr<<" ";cerr<<endl;
  Foreach(itr,mem)cerr<<*itr<<" ";cerr<<endl;
	return 0;
}

