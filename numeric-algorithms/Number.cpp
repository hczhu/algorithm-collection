#include <assert.h>
#include <memory.h>
#include <algorithm>
#include <stdlib.h>
#include <vector>
#include <iostream>
using namespace std;
#define debug(x) cerr<<#x<<"=\""<<x<<"\""<<" at line#"<<__LINE__<<endl;

//The most common functions
typedef long long int llint;
typedef unsigned long long int ullint;


//floor(A/B)
llint intFloor(llint A,llint B) {
  if(B<0) A=-A,B=-B;
  return A>0?A/B:(A-B+1)/B;
}
template<typename T>
llint _lcm(const vector<T>& number) {
  llint res=1;
  for(int i=0;i<number.size();i++) {
    res*=number[i]/__gcd(static_cast<llint>(number[i]),res);
  }
  return res;
}
template<typename T>
T __gcd(T n,T m,T& a,T& b)
{
  T a1=0,b1=1;
  a=1,b=0;
  // a*n+b*m=n';  a1*n+b1*m=m';
  //(n',m') keeps changing.
  while(m)
  {
    T c=n/m;
    T r=n-m*c;
    T t;
    t=a;a=a1;a1=t-c*a1;
    t=b;b=b1;b1=t-c*b1;
    n=m;m=r;
  }
  return n;
}
// update [lower,upper] by a*x<=b
void update_interval(llint a,llint b,llint& lower,llint& upper)
{
  if(a==0){if(0<=b)return;upper=-1,lower=0;return;}
  if(a>0)
  {
    if(b>=0)upper=min(upper,b/a);
    else upper=min(upper,(b-a+1)/a);
  }
  else
  {
    a=-a;b=-b;
    if(b>=0)lower=max(lower,(b+a-1)/a);
    else lower=max(lower,b/a);
  }
}
void generateInverse(llint p,llint inv[],llint n) {
  inv[1]=1;
  n=min(n,p-1);
  for(llint i=2;i<=n;i++) {
    inv[i]=(p-(inv[p%i]*(p/i)%p))%p;
  }
}
llint getReverse(llint a,llint p,llint rev[]=NULL)
{
  if(rev!=NULL)return rev[a];
  llint x,y;
  assert(1==std::__gcd(a,p));
  __gcd(a,p,x,y);
  return ((x%p)+p)%p;
}

llint modPower(llint a,llint n,llint m) {
  llint re=1,t=a;
  while(n)
  {
    if(n&1)re=re*t%m;
    t=t*t%m;
    n>>=1;
  }
  return re;
}
llint getReverseByEuler(llint a,llint p) {
  return modPower(a,p-2,p);
}
inline int mod_mult(llint a,llint b,llint mod)
{
  return a*b%mod;
}
template<typename T>
vector<pair<T,int> > factorize(T n)
{
  vector<pair<T,int> > res;
  for(T t=2;t*t<=n;t++)
  {
    if(0==(n%t))
    {
      int cnt=1;
      n/=t;
      while(0==(n%t))n/=t,cnt++;
      res.push_back(make_pair(t,cnt));
    }
  }
  if(n>1)res.push_back(make_pair(n,1));
  return res;
}
//Linear priem numbers sieve. The array factor[] store a prime factor of a composite number.
int primeSieve(int factor[],int prime[],int n)
{
  int top=0;
  memset(factor,0,sizeof(int)*(n+1));
  //make 1 not prime
  factor[1]=1;
  for(int i=2;i<=n;i++)
  {
    if(0==factor[i])prime[top++]=i;
    for(int j=0;j<top&&prime[j]*i<=n;j++)
    {
      factor[prime[j]*i]=prime[j];
      if(0==(i%prime[j]))break;
    }
  }
  return top;
}
//End of most common functions
//******************************************************************************

// Some advanced functions
// return bino(n,m)%p, p must be a prime.
// Using the Lemma: bino(n,m)=bino(n/p,m/p)*bino(n%p,m%p) mod p
llint binomial_mod(llint n,llint m,llint p)
{
  llint res=1;
  vector<llint> up,down;
  while(n||m)
  {
    llint a=n%p;n/=p;
    llint b=m%p;m/=p;
    if(a<b)return 0;
    if(a>b&&b)up.push_back(a),down.push_back(b),down.push_back(a-b);
  }
  sort(up.begin(),up.end());
  reverse(up.begin(),up.end());
  sort(down.begin(),down.end());
  reverse(down.begin(),down.end());
  llint fact=1;
  for(int i=1;i<p&&(up.size()+down.size());i++)
  {
    fact*=i;fact%=p;
    llint rfact=getReverse(fact,p);
    while(up.size()&&up.back()==i)
      res*=fact,res%=p,up.pop_back();
    while(down.size()&&down.back()==i)
      res*=rfact,res%=p,down.pop_back();
  }
  return res;
}
// All factors p in n! are removed.
// p must be prime.
// degr is the number of factor p in n!
llint factorial_mod(llint n,llint p,llint& deg)
{
  deg=0;
  vector<llint> table(p);
  table[0]=1;
  for(int i=1;i<p;i++)
    table[i]=table[i-1]*i,table[i]%=p;
  llint res=1;
  while(n)
  {
    llint r=n%p;
    n/=p;
    if(n&1)res=p-res;
    res*=table[r];res%=p;
    deg+=n;
  }
  res%=p;
  return res;
}

template<typename T>
vector<llint> Chinese_remainder(const vector<T>& mod)
{
  const int n=mod.size();
  llint all=1;
  for(int i=0;i<n;i++)all*=mod[i];
  vector<llint> res(n);
  for(int i=0;i<n;i++)
  {
    if(__gcd((llint)mod[i],all/mod[i])>1)
      return vector<llint>();
    llint k=getReverse(all/mod[i],mod[i]);
    res[i]=all/mod[i]*k;
    assert(1==(res[i]%mod[i]));
  }
  return res;
}

//solve a*x=b mod p. x0+d,x0+2*d ... are also solutions
bool solveModule(llint a,llint b,llint p,llint& x0,llint& d)
{
  llint x,y,g;
  a%=p;b%=p;
  g=__gcd(a,p,x,y);
  if(b%g)return false;
  d=p/g;
  x0=((((x%d)+d)%d)*(b/g))%p;
  return true;
}
//Solve module equation group
//x==aa[i] mod mm[i]

template<typename T>
llint solveModuleEquations(vector<T> aa,vector<T> mm) {
  const int n=aa.size();
  llint gg,xx,yy;
  for(int i=1;i<n;i++)
  {
    gg=__gcd(mm[0],mm[i],xx,yy);
    if((aa[i]-aa[0])%gg)return -1;
    xx*=(aa[i]-aa[0])/gg;
    mm[i]/=gg;
    xx=((xx%mm[i])+mm[i])%mm[i];
    aa[0]=mm[0]*xx+aa[0];
    mm[0]*=mm[i];aa[0]%=mm[0];
  }
  return aa[0];
}

//Solve module equation group
//x==aa[i] mod mm[i]
llint solveModuleEquations(int n,llint aa[],llint mm[])
{
  llint gg,xx,yy;
  for(int i=1;i<n;i++)
  {
    gg=__gcd(mm[0],mm[i],xx,yy);
    if((aa[i]-aa[0])%gg)return -1;
    xx*=(aa[i]-aa[0])/gg;
    mm[i]/=gg;
    xx=((xx%mm[i])+mm[i])%mm[i];
    aa[0]=mm[0]*xx+aa[0];
    mm[0]*=mm[i];aa[0]%=mm[0];
  }
  return aa[0];
}

//Solve module equation aa[0]*x0+aa[1]*x1+..==b mod p
//The equation has n unknowns
template<int N>
class moduleEquation
{
  llint *aa;
  llint ans[N];
  llint gg[N+1];
  bool solveModule(llint a,llint b,llint p,llint& x0,llint& d)
  {
    llint x,y,g;
    a%=p;b%=p;
    b=(b?p-b:b);
    g=__gcd(a,p,x,y);
    if(b%g)return false;
    d=p/g;
    x0=((((x%d)+d)%d)*(b/g))%p;
    return true;
  }
public:
  bool solve(int n,llint b,llint p,llint aaa[])
  {
    aa=aaa;
    llint d;
    gg[0]=p;b%=p;
    b=(b?p-b:b);
    if(*std::max_element(aa,aa+n)==0)return b==0;
    for(int i=1;i<=n;i++)
      gg[i]=__gcd(gg[i-1],aa[i-1],d,d);
    bool f=true;
    for(int i=n-1;i>=0;i--)
    {
      if(aa[i]==0)continue;
      if(!solveModule(aa[i],b,gg[i],ans[i],d))
      {
        f=false;
        break;
      }
      b=(b+ans[i]*aa[i])%p;
    }
    return f;
  }
};
//Mtrix mult and power and number factor
template<int S,typename T>
class Matrix
{
  T tmp[2*S][2*S],table[S<<1][S<<1];
  T power[S<<1][S<<1];
public:
  template<int N>
  void mult(T a[N][N],T b[N][N],T c[N][N],int n,T p)
  {
    for(int i=0;i<n;i++)
    for(int j=0;j<n;j++)
    {
      c[i][j]=0;
      for(int k=0;k<n;k++)c[i][j]+=(a[i][k]*b[k][j]),c[i][j]%=p;
    }
  }
  template<int N>
  void matrixPower(T base[N][N],int n,llint m,T ans[N][N],T p)
  {
    memset(ans,0,sizeof(T)*n*N);
    for(int i=0;i<n;i++)ans[i][i]=1;
    memcpy(table,base,sizeof(T)*n*N);
    while(m>0)
    {
      if((m&1)==1)
      {
        mult(ans,table,tmp,n,p);
        memcpy(ans,tmp,sizeof(T)*n*N);
      }
      m>>=1;
      mult(table,table,tmp,n,p);
      memcpy(table,tmp,sizeof(T)*n*N);
    }
  }
  T base_[S<<1][S<<1],ans_[S<<1][S<<1];
  template<int N>
  //Calculate Sum{base^0+base^1+base^2+...+base^m}
  // N>=2*n must be satisfied 
  void matrixPowerSum(T base[][N],int n,llint m,T ans[][N],T p)
  {
    for(int i=0;i<n;i++)
    for(int j=0;j<n;j++)
      base_[i][j]=base[i][j],ans[i][j]=0;
    for(int i=0;i<n;i++)
    {
      for(int j=n;j<2*n;j++)
        base_[i][j]=0;
      base_[i][i+n]=1;
    }
    for(int i=n;i<2*n;i++)
    {
      for(int j=0;j<2*n;j++)
        base_[i][j]=0;
      base_[i][i]=1;
    }
    matrixPower(base_,2*n,m,ans_,p);
    for(int i=0;i<n;i++)
    for(int j=0;j<n;j++)
      ans[i][j]+=ans_[i][j]+ans_[i][j+n],ans[i][j]%=p;
  }
};


//***********************************************************************
//Calcalate det(mat)%mod
//Need to fill reverse number
//mod must be a prime

template<int N>
llint matrixDet(llint mat[][N],int n,llint mod,llint revp[]=NULL)
{
  int re=1;
  for(int k=0;k<n;k++)
  {
    int ind;
    for(ind=k;ind<n;ind++)
      if(mat[ind][k])break;
    if(ind==n)
    {
      re=0;
      break;
    }
    if(ind!=k)
    {
      for(int i=k;i<n;i++)
        std::swap(mat[ind][i],mat[k][i]);
      re=(mod-re)%mod;
    }
    re=(re*mat[k][k])%mod;
    for(int i=k+1;i<n;i++)
    {
      int mm=(getReverse(mat[k][k],mod,revp)*mat[i][k])%mod;
      if(mm==0)continue;
      for(int j=k;j<n;j++)
        mat[i][j]=(mat[i][j]+mod-((mm*mat[k][j])%mod))%mod;
    }
  }
  return re;
}

//************************************************************************************
//Random prime tester and Pollard algorithm
struct RobinMiller
{
  static const int T=20;
  //m must be less than 1<<62
  llint modMult(llint a,llint b,llint m)
  {
    if(b==0)return 0;
    return (((modMult(a,b>>1,m)<<1)%m)+(b&1)*a)%m;
  }
  llint modPower(llint a,llint n,llint m)
  {
    llint re=1,t=a;
    while(n)
    {
      if(n&1)re=modMult(re,t,m);
      t=modMult(t,t,m);
      n>>=1;
    }
    return re;
  }
  bool witness(llint a,llint n)
  {
    int t;
    const llint m=n;
    for(n--,t=0;(n&1)==0;t++,n>>=1);
    llint x=modPower(a,n,m),x1;
    while(t--)
    {
      x1=modMult(x,x,m);
      if(x1==1&&x!=1&&x!=m-1)return true;
      x=x1;
    }
    return x!=1;
  }
  bool robinMiller(llint n)
  {
    llint a;
    if(n==1)return false;
    static llint base[]=
    {
      2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71
    };
    const int B=sizeof(base)/sizeof(base[0])-1;

    for(int i=0;i<B&&base[i]<n;i++)
      if(witness(base[i],n))return false;
    for(int i=0;i<T;i++)
    {
      a=2+(rand()%(n-2));
      if(witness(a,n))return false;
    }
    return true;
  }

  llint gcd(llint a,llint b)
  {
    llint r;
    while(b)r=a%b,a=b,b=r;
    return a;
  }
  llint pollard(llint n)
  {
    int i=1,k=2;
    llint x=rand()%n;
    llint y=x,d;
    llint a=rand()%n;
    for(;;)
    {
      i++;
      x=(modMult(x,x,n)+a)%n;
      if(y>x)d=gcd(y-x,n);
      else d=gcd(x-y,n); 
      if(d!=1&&d!=n)return d;
      if(i==k)k<<=1,y=x;
    }
    return 1;
  }
};



//*************************************
// encdoe and decode a permutation in lienar time
//*************** 
unsigned int encode_factorial(const int per[],int n)
{
  typedef unsigned int uint;
  unsigned int res=0;
  int map[10]={0,1,2,3,4,5,6,7,8,9};
  int rmap[10]={0,1,2,3,4,5,6,7,8,9};
  static const unsigned int fact[]={1,1,2,2*3,2*3*4,2*3*4*5,2*3*4*5*6,2*3*4*5*6*7,(uint)2*3*4*5*6*7*8,(uint)2*3*4*5*6*7*8*9};

  for(int i=n-1;i>=0;i--)
  {
    const int p=map[per[i]];
    res+=p*fact[i];
    assert(p<=i);
    // Let p replace i
    map[rmap[i]]=p;
    rmap[p]=rmap[i];
  }
  return res;
}
void decode_factorial(int per[],int n,unsigned int code)
{
  typedef unsigned int uint;
  int map[10]={0,1,2,3,4,5,6,7,8,9};
  int rmap[10]={0,1,2,3,4,5,6,7,8,9};
  static const unsigned int fact[]={1,1,2,2*3,2*3*4,2*3*4*5,2*3*4*5*6,2*3*4*5*6*7,(uint)2*3*4*5*6*7*8,(uint)2*3*4*5*6*7*8*9};

  for(int i=n-1;i>=0;i--)
  {
    int p=code/fact[i];
    per[i]=rmap[p];
    code-=p*fact[i];
    map[rmap[i]]=p;
    rmap[p]=rmap[i];
  }
}

template<typename T,int N>
void init_bino(T com[N][N],bool take_mod=false,long long int mod=1000000000000000000LL)
{
  if(take_mod)
  {
    assert(mod>0);
    com[0][0]=1%mod;
    for(int i=1;i<N;i++)
    {
      com[i][0]=com[i][i]=1%mod;
      for(int j=1;j<i;j++)com[i][j]=(com[i-1][j-1]+com[i-1][j])%mod;
    }
  }
  else
  {
    com[0][0]=1;
    for(int i=1;i<N;i++)
    {
      com[i][0]=com[i][i]=1;
      for(int j=1;j<i;j++)com[i][j]=(com[i-1][j-1]+com[i-1][j]);
    }
  }
}
//select m objects from n objects. pick[] must have been sorted.
template<int N>
int bino_encode(int n,int m,int pick[],int com[N][N])
{
  int res=0;
  for(int i=m-1;i>=0;i--)
  {
    res+=com[pick[i]][m];
    m--;
  }
  return res;
}
template<int N>
void bino_decode(int n,int m,int code,int pick[],int com[N][N])
{
  for(int i=n-1;i>=0;i--)
  {
    if(code>=com[i][m])
    {
      code-=com[i][m];
      pick[--m]=i;
    }
  }
  assert(0==m);
}
bool is_prime(llint n)
{
  if(n==1)return false;
  for(llint t=2;t*t<=n;t++)
  {
    if(0==(n%t))return false;
  }
  return true;
}
//calculate Sum{ (A*i + B)/M |  0 <= i <=C   }
// A and B may be negative.  M > 0
// The algorithm is to transform the summation to Sum{Count( (A*i +B)/M>=k )|1<=k}
llint floorSummation(llint A,llint B,llint C,llint M) {
  /*llint ans=0;
  for(llint i=0;i<=C;i++) {
   ans+=intFloor(A*i+B,M);
  }
  cerr<<"("<<A<<"*i+"<<B<<")/"<<M<<" (0<=i<="<<C<<")= "<<ans<<endl;*/
  assert(M>0);
  if(C<0) return 0;
  llint res=(A/M)*(C*(C+1)/2);A%=M;
  res+=(C+1)*(B/M);B%=M;
  if(A<0) {
    res-=C*(C+1)/2;
    A+=M;
  }
  if(B<0) {
    res-=C+1;
    B+=M;
  }
  if(A==0) return (C+1)*(B/M)+res;
  assert(A>0);
  assert(B>=0);
  const llint upper=(A*C+B)/M;
  res+=(C+1)*upper-floorSummation(M,M+A-1-B,upper-1,A);
  //debug(res);
  //debug(ans);
  //assert(ans==res);
  return res;
}
int main() {
  //test floorSummation
  {
    assert(floorSummation(2,5,4,3)==13);
    assert(floorSummation(-2,5,4,3)==0);
    srand(time(NULL));
    const llint N=10000;
    for(int cc=0;cc<100;cc++) {
      //debug(cc);
      llint A=N-(rand()%(2*N));
      llint B=N-(rand()%(2*N));
      llint M=1+(rand()%N);
      llint C=rand()%(10*N);
      llint ans=0;
      for(llint i=0;i<=C;i++) {
        ans+=intFloor(A*i+B,M);
      }
      assert(ans==floorSummation(A,B,C,M));
    }
  }
  {
    int *prime=new int[100000000];
    int *fact=new int[100000000];
    int top=primeSieve(fact,prime,100000000);
    debug(top);
  }
  return 0;
}
