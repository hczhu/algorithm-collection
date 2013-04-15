#include <vector>
#include <algorithm>
#include <assert.h>
using namespace std;
// Matrix class
template<typename T>
class Matrix:public vector<vector<T> >
{
  static T (*_add)(const T&,const T&);
  static T (*_mul)(const T&,const T&);
  // add(zero,x)=x;  mul(zero,x)=zero; 
  // mul(one,x)=x;
  static T _zero,_one;
public:
  static void set_para(T (*add)(const T&,const T&),T (*mul)(const T&,const T&),const T& zero,const T& one)
  {_add=add;_mul=mul;_zero=zero;_one=one;};
  void resize(int n,int m){this->resize(n);for(int i=0;i<n;i++)this->at(i).resize(m);};
  Matrix(){vector<vector<T> >();};
  Matrix(int n,int m):vector<vector<T> >(n,vector<T>(m)){};
  Matrix(int n,int m,const T& v):vector<vector<T> >(n,vector<T>(m,v)){};
  Matrix<T> operator+(const Matrix& other)const;
  Matrix<T> operator-(const Matrix& other)const;
  Matrix<T> operator*(const Matrix&)const;
  Matrix<T> power(long long int s)const;
  // M^1 + M^2 + ... + M^s
  Matrix<T> power_sum(long long int s)const;
  static Matrix<T> ONE(int n,int m){Matrix<T> res(n,m,_zero);for(int i=0;i<n;i++)res[i][i]=_one;return res;};
  static Matrix<T> ZERO(int n,int m){Matrix<T> res(n,m,_zero);return res;};
};
template<typename T> T (*Matrix<T>::_add)(const T&,const T&)=NULL;
template<typename T> T (*Matrix<T>::_mul)(const T&,const T&)=NULL;
template<typename T> T Matrix<T>::_zero;
template<typename T> T Matrix<T>::_one;
template<typename T>
Matrix<T> Matrix<T>::power(long long int s)const
{
  assert(this->size()==this->at(0).size());
  Matrix<T> res=Matrix<T>::ONE(this->size(),this->at(0).size());
  Matrix<T> t=*this;assert(s>=0);
  while(s){if(1&s)res=res*t;t=t*t;s>>=1;}return res;
}
template<typename T>
Matrix<T> Matrix<T>::power_sum(long long int s)const
{
  assert(this->size==this->at(0).size());
  if(s==1)return *this;
  if(s==0)return Matrix<T>::ZERO(this->size(),this->size());
  long long int mid=s/2;Matrix<T> low=power_sum(mid);Matrix<T> high;
  if(s-mid>mid)high=low+power(s-mid);else high=low;
  high=high*power(mid);return low+high;
}

template<typename T>
Matrix<T> Matrix<T>::operator*(const Matrix& b)const
{
  const Matrix<T>& a=*this;const int n=a.size();assert(n);
  int p=a[0].size();assert(p);assert(p==b.size());int m=b[0].size();assert(m);
  Matrix<T> res(n,m);for(int i=0;i<n;i++)for(int j=0;j<m;j++)
  {T t=_zero;for(int k=0;k<p;k++)t=_add(t,_mul(a[i][k],b[k][j]));res[i][j]=t;}return res;
}
template<typename T>
Matrix<T> Matrix<T>::operator+(const Matrix& other)const
{
  assert(this->size==other.size());
  assert(this->size()==0||this->at(0).size()==other[0].size());
  const int n=this->size();if(n==0)return Matrix<T>();const int m=other[0].size();
  Matrix<T> res(n,m);for(int i=0;i<n;i++)for(int j=0;j<m;j++)res[i][j]=_add((*this)[i][j],other[i][j]);
  return res;
}

// solve equations  a[i]*x[i-1]+b[i]*x[i]+c[i]*x[i+1]=d[i]
void TridiagonalSolve
  (const double *a, const double *b, double *c, double *d, double *x, unsigned int n)
{
  const double eps=1e-10;
  if(fabs(b[0])<eps)
  {
    //So we know x[1] directly
    printf("Special\n");
    return;
  }
  // make the form of x[0]+c[0]*x[1]=d[0]
  c[0] /= b[0];  
  d[0] /= b[0];  
  for (int i = 1; i < n; i++){

    double id =  (b[i] - c[i-1] * a[i]);  
    if(fabs(id)<eps)
    {
      printf("Special\n");
      return;
    }
    // Eliminate x[i-1] from a[i]*x[i-1]+b[i]*x[i]+c[i]*x[i+1]=d[i]
    // and make it the form of x[i]+c[i]*x[i+1]=d[i]
    id=1/id;
    c[i] *= id;                           
    d[i] = (d[i] - d[i-1] * a[i]) * id;
  }
 
  x[n - 1] = d[n - 1];
  for (int i = n - 2; i >= 0; i--)
    x[i] = d[i] - c[i] * x[i + 1];
}

//Return 0: no solution
//Return 1: one solution
//Return >1: the dimension of the solution space+1
#define OUT
int GuassElimination(vector<vector<double> > aa,vector<double> bb,
    vector<double>& x)
{
  vector<vector<double> > caa=aa;
  vector<double> cbb=bb;
  const int m=aa.size();
  const int n=aa[0].size();
  assert(bb.size()==m);
#ifdef OUTPUT
  hline();
  cerr<<m<<" X "<<n<<endl;
  for(int i=0;i<m;i++)
  {
    for(int j=0;j<n;j++)
      cerr<<aa[i][j]<<" ";
    cerr<<"= "<<bb[i]<<endl;
  }
#endif
  const double eps=1e-10;
  x.clear();
  for(int i=0;i<n;i++)x.push_back(0.0);
  vector<int> major;
  for(int k=0,p=0;k<n&&p<m;k++)
  {
    int best=p;
    for(int i=p;i<m;i++)
      if(fabs(aa[i][k])>fabs(aa[best][k]))best=i;
    if(fabs(aa[best][k])<eps)continue;
    for(int i=k;i<n;i++)swap(aa[best][i],aa[p][i]);
    swap(bb[best],bb[p]);
    major.push_back(k);
    bb[p]/=aa[p][k];
    for(int i=n-1;i>=k;i--)aa[p][i]/=aa[p][k];
    for(int i=p+1;i<m;i++)
    {
      double t=aa[i][k];
      //if(fabs(t)<eps)continue;
      bb[i]-=bb[p]*t;
      for(int j=k;j<n;j++)aa[i][j]-=t*aa[p][j];
      assert(fabs(aa[i][k])<1e-10);
    }
    p++;
  }
#ifdef OUTPUT
  hline();
  debug(major.size());
  for(int i=0;i<m;i++)
  {
    for(int j=0;j<n;j++)
      cerr<<aa[i][j]<<" ";
    cerr<<"= "<<bb[i]<<endl;
  }
#endif
  for(int i=major.size();i<m;i++)
    if(fabs(bb[i])>eps)return 0;
  if(major.empty())return n+1;

  for(int i=major.size()-1;i>=0;i--)
  {
    int begin=major[i];
    x[begin]=bb[i];
    for(int j=begin+1;j<n;j++)
      x[begin]-=x[j]*aa[i][j];
  }

#ifdef OUTPUT
  hline();
  cerr<<"x="<<endl;
  for(int i=0;i<n;i++)
    cerr<<x[i]<<" ";
  cerr<<endl;
  for(int i=0;i<m;i++)
  {
    double sum=0.0;
    for(int j=0;j<n;j++)sum+=x[j]*caa[i][j];
    debug(sum);
    debug(cbb[i]);
    debug(sum-cbb[i]);
    assert(fabs(sum-cbb[i])<eps);
  }
#endif
  return n-major.size()+1;
}
namespace LinearSolver
{
int nzero[N];
int vindex[N];
bool addVect(double vect[][N],int ind,int& top,int m)
{
  nzero[ind]=0;
  while(nzero[ind]<m&&fabs(vect[ind][nzero[ind]])<EPS)
    nzero[ind]++;
  if(nzero[ind]==m)return true;
  for(int i=0;i<top;i++)
  {
    int v=vindex[i];
    if(nzero[ind]!=nzero[v])continue;
    double t=vect[ind][nzero[ind]]/vect[v][nzero[v]];
    for(int j=nzero[ind];j<m;j++)
      vect[ind][j]-=vect[v][j]*t;
    vect[ind][nzero[ind]]=0.0;
    while(nzero[ind]<m&&fabs(vect[ind][nzero[ind]])<EPS)
      nzero[ind]++;
    if(nzero[ind]==m)
      return true;
  }
  if(nzero[ind]==m-1)return false;
  double t=vect[ind][nzero[ind]];
  for(int j=nzero[ind];j<m;j++)
    vect[ind][j]/=t;
  vindex[top++]=ind;
  for(int j=top-1;j;j--)
    if(nzero[vindex[j]]<nzero[vindex[j-1]])
      swap(vindex[j],vindex[j-1]);
  return true;
}
bool solve(double vect[][N],int m,int n,double sol[],int basic[])
{
  int top=0;
  for(int i=0;i<m;i++)
    if(!addVect(vect,i,top,n+1))
      return false;
  for(int i=0;i<n;i++)basic[i]=-1,sol[i]=0.0;
  for(int i=top-1;i>=0;i--)
  {
    int ind=vindex[i];
    basic[nzero[ind]]=i;
    sol[nzero[ind]]=vect[ind][n];
    for(int j=n-1;j>nzero[ind];j--)
      sol[nzero[ind]]-=sol[j]*vect[ind][j];
  }
  return true;
}
};
// Circular matrix can be operated in O(n) time
