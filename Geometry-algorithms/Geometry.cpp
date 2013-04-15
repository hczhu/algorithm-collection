#include <stdio.h>
#include <math.h>
#include <algorithm>
#include <vector>
#include <iostream>
#define NDEBUG
#include <assert.h>
#include <complex>
using namespace std;
#ifndef NDEBUG
    #define debug(x) cerr<<#x<<"=\""<<x<<"\""<<" at line#"<<__LINE__<<endl;
    #define hline() cerr<<"-----------------------------------------"<<endl;
#else
    #define debug(x)
    #define hline()
#endif
const double EPS=1e-15;
const double PI=acos(-1.0);
typedef long long int llint;
typedef unsigned long long int ullint;
double mysqrt(double v)
{
  if(v<EPS)return 0.0;
  return sqrt(v);
}
int sigcross(llint x1,llint y1,llint x2,llint y2,llint x3,llint y3)
{
  llint re=(llint)(x1-x3)*(y2-y3)-(llint)(y1-y3)*(x2-x3);
  if(re<0)return -1;
  if(re>0)return 1;
  return 0;
}
template<typename T>
class point2D
{
  public:
  T _x,_y;
  T x()const{return _x;};
  T& x(){return _x;};
  T y()const{return _y;};
  T& y(){return _y;};
  point2D(const T& x=T(),const T& y=T()):_x(x),_y(y){};
  point2D(const point2D& other):_x(other._x),_y(other._y){};
  const point2D& operator=(const point2D<T>& other){_x=other._x,_y=other._y;return *this;};
  bool operator==(const point2D<T>& other)const{return _x==other._x&&_y==other._y;};
  double length()const {return sqrt(1.0*_x*_x+1.0*_y*_y);};
  double dnorm()const {return 1.0*_x*_x+1.0*_y*_y;};
  T norm()const
  {
    return  _x*_x+_y*_y;
  };
  bool operator<(const point2D& other)const
  {
    return x()<other.x()||(x()==other.x()&&y()<other.y());
  }
  point2D operator+=(const point2D& other)
  {
    _x+=other._x;
    _y+=other._y;
    return *this;
  };
  point2D operator+(const point2D& other)const
  {
    point2D res=*this;
    res+=other;
    return res;
  };
  
  point2D operator-=(const point2D& other)
  {
    _x-=other._x;
    _y-=other._y;
    return *this;
  };
  point2D operator-(const point2D& other)const
  {
    point2D res=*this;
    res-=other;
    return res;
  };

  point2D operator*=(const point2D& other)
  {
    point2D res=(*this)*other;
    *this=res;
    return *this;
  };
  point2D operator*(const point2D& other)const
  {
    point2D res;
    res._x=_x*other._x-_y*other._y;
    res._y=_x*other._y+_y*other._x;
    return res;
  };
  point2D conjugate()const
  {
    return point2D(_x,-_y);
  }
  point2D negate()const
  {
    return point2D(-_x,-_y);
  }
  point2D<double> operator/(const point2D<T>& other)const
  {
    assert(other.dnorm()>0.0);
    point2D<double> res=*this;
    res*=other.conjugate();
    res._x/=other.dnorm();
    res._y/=other.dnorm();
    return res;
  }
  point2D<double> operator/=(const point2D<T>& other)
  {
    *this=(*this)/other;
    return *this;
  }
  point2D<double> operator/(const T& other)
  {
    assert(fabs(1.0*other)>0.0);
    point2D res=*this;
    res._x/=other;
    res._y/=other;
    return res;
  }
  point2D<double> operator/=(T& other)
  {
    *this=(*this)/other;
    return *this;
  }
};
template<typename T>
ostream& operator<<(ostream& out,const point2D<T>& other)
{
  out<<"("<<other.x()<<","<<other.y()<<")";
  return out;
}
template<typename T>
istream& operator>>(istream& in,point2D<T>& other)
{
  in>>other.x()>>other.y();
  return in;
}


typedef point2D<double> dpoint2D ;

template<typename T>
T cross(const point2D<T>& p1,const point2D<T>& p2)
{
  return p1.x()*p2.y()-p1.y()*p2.x();
}


template<typename T>
llint llcross(const point2D<T>& p1,const point2D<T>& p2)
{
  return (llint)p1.x()*(llint)p2.y()-(llint)p2.x()*(llint)p1.y();
};

template<typename T>
int sigcross(point2D<T> p1,point2D<T> p2)
{
  llint re=llcross(p1,p2);
  if(re<0)return -1;
  if(re>0)return 1;
  return 0;
}

template<typename T1,typename T2>
double dcross(point2D<T1> p1,point2D<T2> p2)
{
  return 1.0*p1.x()*p2.y()-p2.x()*p1.y();
}

template<typename T1,typename T2>
double ddot(point2D<T1> p1,point2D<T2> p2)
{
  return 1.0*p1.x()*p2.x()+1.0*p1.y()*p2.y();
}
template<typename T1,typename T2>
T1 dot(point2D<T1> p1,point2D<T2> p2)
{
  return p1.x()*p2.x()+p1.y()*p2.y();
}
template<typename T>
T norm(const point2D<T>& p)
{
  return p.norm();
}

template<typename T>
double dnorm(const point2D<T>& p)
{
  return 1.0*p._x*p._x+1.0*p._y*p._y;
}
template<typename T>
double length(const point2D<T>& p)
{
  return sqrt(dnorm(p));
}
//*********************************************************
// 3D point
template<typename T>
class point3D
{
  public:
  T _x,_y,_z;
  T x()const{return _x;};
  T y()const{return _y;};
  T z()const{return _z;};
  T& x(){return _x;};
  T& y(){return _y;};
  T& z(){return _z;};

  point3D(const T xx=T(),const T& yy=T(),const T& zz=T()):_x(xx),_y(yy),_z(zz){};
  const point3D& operator=(const point3D& other)
  {
    _x=other._x;
    _y=other._y;
    _z=other._z;
  }
  const point3D& operator+(const point3D& other)const
  {
    point3D res=*this;
    res+=other;
    return res;
  };
  const point3D& operator+=(const point3D& other)
  {
    _x+=other._x;
    _y+=other._y;
    _z+=other._z;
    return *this;
  }
  const point3D& operator-(const point3D& other)const
  {
    point3D res=*this;
    res-=other;
    return res;
  };
  const point3D& operator-=(const point3D& other)
  {
    _x-=other._x;
    _y-=other._y;
    _z-=other._z;
    return *this;
  };
  const point3D& operator*(T k)const
  {
    point3D res=*this;
    res*=k;
    return res;
  };
  const point3D& operator*=(T k)
  {
    _x*=k;
    _y*=k;
    _z*=k;
    return *this;
  };
  const point3D<T>& operator*(const point3D<T>& other)const
  {
    point3D res;
    res._x=_y*(other._z)-_z*(other._y);
    res._y=-_x*(other._z)+_z*(other._x);
    res._z=_x*(other._y)-_y*(other._x);
    return res;
  };

  const point3D& operator*=(const point3D& other)
  {
    point3D res=(*this)*other;
    *this=res;
    return *this;
  };
  T norm()const
  {
    return  _x*_x+_y*_y+_z*_z;
  };
  double dnorm()const
  {
    return 1.0*_x*_x+1.0*_y*_y+1.0*_z*_z;
  }
  double length()const
  {
    return sqrt(dnorm());
  };

};
template<typename T1,typename T2>
double ddot(point3D<T1> p1,point3D<T2> p2)
{
  return 1.0*p1.x()*p2.x()+1.0*p1.y()*p2.y()+1.0*p1.z()*p2.z();
}

template<typename T1,typename T2>
T1 dot(point3D<T1> p1,point3D<T2> p2)
{
  return p1.x()*p2.x()+p1.y()*p2.y()+p1.z()*p2.z();
}

template<typename T>
ostream& operator<<(ostream& out,const point3D<T>& other)
{
  out<<"("<<other.x()<<","<<other.y()<<","<<other.z()<<")";
  return out;
}
template<typename T>
istream& operator>>(istream& in,point3D<T>& other)
{
  in>>other.x()>>other.y()>>other.z();
  return in;
}


template<typename T>
T norm(const point3D<T>& p)
{
  return p.norm();
}

template<typename T>
double dnorm(const point3D<T>& p)
{
  return p.dnorm();
}
template<typename T>
double length(const point3D<T>& p)
{
  return p.length();
}
//sort vectors clockwise
template<typename T>
int get_type(const point2D<T>& a)
{
  if(a.y())return a.y()>0?1:-1;
  assert(a.x());
  return a.x()>0?1:-1;
}
template<typename T>
bool cmp_vector(const point2D<T>& a,const point2D<T>& b)
{
  int ta=get_type(a);
  int tb=get_type(b);
  if(ta!=tb)return ta==1;
  return sigcross(b,a)>0;
}
//****************************************************************************
//End of base classes and functions

//return the foot of a perpendicular 
//The plane is represented by (X-px)pv=0
template<typename T>
point3D<double> point_plane_intersection(point3D<T> p0,point3D<T> px,point3D<T> pv)
{
  double t=ddot(p0-px,pv)/dnorm(pv);
  return point3D<double>(p0.x()+t*pv.x(),p0.y()+t*pv.y(),p0.z()+t*pv.z());
}


// Some functions about lines and segments

//check intersection of two lines
//Return false if there are infinite intersections
template<typename T>
bool linesIntersection(point2D<T> p1,point2D<T> p2,point2D<T> p3,
      point2D<T> p4,point2D<double>& p)
{
  if(fabs(dcross(p1-p2,p3-p4))<EPS)return false;
  double t=dcross(p3-p1,p3-p4)/dcross(p2-p1,p3-p4);
  p.x()=1.0*p1.x()+1.0*(p2.x()-p1.x())*t;
  p.y()=1.0*p1.y()+1.0*(p2.y()-p1.y())*t;
  return true;
}
// Return the intersection of two lines in the form of "kx1*x+ky1*y+k1=0"
//Return false if they coincide or parallel 
bool linesIntersection
(double kx1,double ky1,double k1,double kx2,double ky2,double k2,point2D<double>& p)
{
  double t;
  if(fabs(t=kx1*ky2-kx2*ky1)<EPS)return false;
  p=point2D<double>(-(k1*ky2-k2*ky1)/t,(k1*kx2-k2*kx1)/t);
  return true;
}
//The two segments must have only one intersection
//Find the intersection of two segments (p1,p2) and (p3,p4)
//Return 0: no intersection.
//Return 1: one intersection.
//Return 2: infinite intersection. overlap.
template<typename T>
int segmentsIntersection
(point2D<T> p1,point2D<T> p2,point2D<T> p3,point2D<T> p4,point2D<double>& inter)
{
  if(sigcross(p3-p1,p2-p1)*sigcross(p2-p1,p4-p1)<0)return 0;
  if(sigcross(p1-p3,p4-p3)*sigcross(p4-p3,p2-p3)<0)return 0;
  if(sigcross(p2-p1,p3-p1)==0&&sigcross(p2-p1,p4-p1)==0)
  {
    if(p1.x()==p2.x())
    {
      if(p1.y()>p2.y())swap(p1,p2);
      if(p3.y()>p4.y())swap(p3,p4);
      T ly=max(p1.y(),p3.y());
      T hy=min(p2.y(),p4.y());
      if(ly>hy)return 0;
      if(ly<hy)return 2;
      inter=point2D<double>(p2.x(),p2.y());
      return 1;
    }
    else
    {
      if(p1.x()>p2.x())swap(p1,p2);
      if(p3.x()>p4.x())swap(p3,p4);
      T lx=max(p1.x(),p3.x());
      T hx=min(p2.x(),p4.x());
      if(lx>hx)return 0;
      if(lx<hx)return 2;
      inter=point2D<double>(p2.x(),p2.y());
      return 1;
    }
  }
  if(!linesIntersection(p1,p2,p3,p4,inter))*((int*)0)=0;
  return 1;
}
//Return the distance between (x1,y1) and segment (bx,by)--(ex,ey) 
template<typename T>
double pointSegmentDistance(point2D<T> p1,point2D<T> bp,point2D<T> ep)
{
  dpoint2D dp1(p1.x(),p1.y()),dbp(bp.x(),bp.y()),dep(ep.x(),ep.y());
  dpoint2D dp=dep-dbp;
  double a=dp.dnorm();
//  double b=2.0*(bp.x()-p1.x())*dp.x()+2.0*(bp.y()-p1.y())*dp.y();
  double b=2.0*ddot(dbp-dp1,dp);
//  double c=(bx-x1)*(bx-x1)+(by-y1)*(by-y1);
  double c=(dbp-dp1).dnorm();
  double ans=min(c,a+b+c);
  if(fabs(a)<EPS)return mysqrt(ans);
  double t=-b/2.0/a;
  if(t<0.0||t>1.0)return mysqrt(ans);
  return mysqrt(t*t*a+t*b+c);
}
//Return the distance of a point p and a line p+dp*t 
//The minimum point in the line is p+dp*tt.
template<typename T>
double pointLineDistance(point2D<T> p,point2D<T> p0,point2D<T> dp,double& tt)
{
  tt=0.0;
  double a=dp.dnorm();
  double b=2.0*ddot(p0-p,dp);//(x0-xx)*dx+2.0*(y0-yy)*dy;
  double c=(p0-p).dnorm();//(x0-xx)*(x0-xx)+(y0-yy)*(y0-yy);
  if(fabs(a)<EPS)return mysqrt(c);
  tt=-b/2.0/a;
  return mysqrt(a*tt*tt+b*tt+c+EPS);
}
//Return the intersection of segment (p1,p2) and a*x+b*y+c=0
//0--> no intersection; 2--> infinite intersection
template<typename T>
int lineSegmentIntersection(point2D<T> p1,point2D<T> p2,
    double a,double b,double c,dpoint2D& inter)
{
  double de=1.0*(p2.x()-p1.x())*a+1.0*(p2.y()-p1.y())*b;
  double no=-c-a*p1.x()-b*p1.y();
  if(fabs(de)<EPS)return fabs(no)<EPS?2:0;
  double t=no/de;
  if(t<-EPS||t>1.0+EPS)return 0;
  inter.x()=t*(p2.x()-p1.x())+p1.x();
  inter.y()=t*(p2.y()-p1.y())+p1.y();
  return 1;
}
//Convert line form from (p1,p2) to a*x+b*y+c=0
template<typename T,typename A>
void convert(point2D<T> p1,point2D<T> p2,A& a,A& b,A& c)
{
  a=((A)p2.y()-(A)p1.y());
  b=((A)p1.x()-(A)p2.x());
  c=(A)p1.y()*((A)p2.x()-p1.x())-(A)p1.x()*((A)p2.y()-p1.y());
}

// End of lines and segments functions
//****************************************************************

//**********************************************************
//Functions of circles

//Find one intersection of two circles. Return the upper intersection point if x1<x2
template<typename T>
bool circlesIntersection
  ( point2D<T> p1,T r1,point2D<T> p2,T r2,point2D<double>& inter)
{
  double dx = p2.x() - p1.x();
  double dy = p2.y() - p1.y();
  double d = mysqrt(dx*dx + dy*dy);
  if(d>r1+r2+EPS||d+EPS<fabs(r1-r2))return false;
  //circle overlap
  if(d<EPS)return false;
  double e = (r1*r1 - r2*r2 + d*d)/(2*d);
  double f = mysqrt(r1*r1 - e*e);
  inter=point2D<double>(1.0*p1.x() + (e*dx - f*dy)/d,1.0*p1.y() + (f*dx + e*dy)/d);
  return true;
}

//Intersections of a line p+dp*t and a circle (center,r).
//Two intersections are p+dp*t1 and p+dp*t2.
template<typename T>
bool lineCircleIntersection
  (point2D<T> p0,point2D<T> dp,point2D<T> center,T r,double& t1,double& t2)
{
  if(dp.dnorm()<EPS)
  {
    t1=t2=0.0;
    return fabs((center-p0).dnorm()-r*r)<EPS;
  }
  double tt;
  double d=pointLineDistance(center,p0,dp,tt);
  if(d>r+EPS)return false;
  double dt=mysqrt(EPS+(r*r-d*d)/(dp.dnorm()));
  t1=tt-dt;t2=tt+dt;
  return true;
}

// Find a circle going through the given three points
// Return false if there are an infinite number of such circles or no such circle
bool circle_on_point3(dpoint2D a,dpoint2D b,dpoint2D c,dpoint2D& center,double& radius)
{
  dpoint2D ab0=(a+b)/2.0;
  dpoint2D ab1=(a-b)*dpoint2D(0.0,1)+ab0;
  
  dpoint2D ac0=(a+c)/2.0;
  dpoint2D ac1=(a-c)*dpoint2D(0.0,1)+ac0;
  if(!linesIntersection(ab0,ab1,ac0,ac1,center))return false;
  radius=(center-a).length();
  return true;
}
// Overlaped area of two circles.
template<typename T>
double circleOverlap(point2D<T> c1,double r1,point2D<T> c2,double r2)
{
  if(length(c1-c2)+EPS>r1+r2)return 0.0;
  if(r1>r2)swap(c1,c2),swap(r1,r2);
  if(length(c1-c2)<r2-r1+EPS)return PI*r1*r1;
  point2D<double> p1,p2;
  circlesIntersection(c1,r1,c2,r2,p1);
  circlesIntersection(c2,r2,c1,r1,p2);
  double th1=acos(dot(p1-c1,p2-c1)/length(p1-c1)/length(p2-c1));
  double th2=acos(dot(p1-c2,p2-c2)/length(p1-c2)/length(p2-c2));
  double sum=th1*r1*r1/2+th2*r2*r2/2;
  double d=length(p1-p2);
  sum-=sqrt(r1*r1-d*d/4)*d/2+sqrt(r2*r2-d*d/4)*d/2;
  return sum;
}

// nocolinear=true means remove all points on the edges of convex hull.
// Be carefull to the points on the same coordanates.
template<typename T>
vector<point2D<T> > convexHull(vector<point2D<T> > all,bool nocolinear=true)
{
  if(all.size()<2)return all;
  sort(all.begin(),all.end());
  vector<int>  qq;
  qq.push_back(0);
  int cut=1;
  if(nocolinear)cut=0;
  for(int i=1;i<all.size();i++)
  {
    // If all[qq.back()]==all[qq[qq.size()-2]], then all point2D<T> s will be pushed to the stack
    while(qq.size()>=2&&llcross(all[i]-all[qq.back()],
          all[qq.back()]-all[qq[qq.size()-2]])>=cut)
      qq.pop_back();
    qq.push_back(i);
  }
  vector<point2D<T> > res;
  vector<bool> used(all.size(),false);
  for(int i=0;i<qq.size();i++)
    res.push_back(all[qq[i]]),used[qq[i]]=true;
  used[0]=used[all.size()-1]=false;
  qq.clear();
  qq.push_back(all.size()-1);
  for(int i=all.size()-2;i>=0;i--)
  {
    if(used[i])continue;
    while(qq.size()>=2&&llcross(all[i]-all[qq.back()],
                  all[qq.back()]-all[qq[qq.size()-2]])>=cut)
      qq.pop_back();
    qq.push_back(i);
  }
  for(int i=1;i+1<qq.size();i++)res.push_back(all[qq[i]]);
  return res;
}
//Use line start-->end to cut convex
//The half plane of the left side of start-->end is kept
vector<point2D<double> > cut_convex(const vector<point2D<double> >& convex,
      point2D<double> start,point2D<double> end)
{
  vector<point2D<double> > ret;
  const int n=convex.size();
  if(n<3)return ret;
  vector<int> inside(n,0);
  for(int idx=0;idx<n;idx++)
    inside[idx]=(dcross(end-start,convex[idx]-start)>EPS)?1:0;
//  if(*max_element(inside.begin(),inside.end())==0)return ret;
//  if(*min_element(inside.begin(),inside.end())==1)return convex;
  point2D<double> inter;
  for(int i=0,pre=0,next=0;i<n;i++)
  {
    next=(pre+1)%n;
    if(inside[pre]!=inside[next])
    {
      bool cnt=linesIntersection(start,end,convex[pre],convex[next],inter);
      assert(cnt);
      ret.push_back(inter);
    }
    if(inside[next])ret.push_back(convex[next]);
    pre=next;
  }
  return ret;
}
template<typename T>
T convex_area(const vector<point2D<T> >& convex)
{
  const int n=convex.size();
  T ret=0;
  for(int i=0;i<n;i++)ret+=cross(convex[i],convex[(i+1)%n]);
  if(ret<0)ret=-ret;
  ret/=2;
  return ret;
}
struct rect_t
{
  int x1,x2,y1,y2;
  rect_t(){};
  rect_t(int a,int b,int c,int d):x1(a),x2(b),y1(c),y2(d){};
  bool operator<(const rect_t& other)const
  {
    return y1<other.y1;
  }
  const rect_t& operator=(const rect_t& other)
  {
    x1=other.x1;
    x2=other.x2;
    y1=other.y1;
    y2=other.y2;
    return *this;
  };
  rect_t intersaction(const rect_t& other) {
    rect_t res(max(x1,other.x1),min(x2,other.x2),
            max(y1,other.y1),min(y2,other.y2));
    if(res.x1>res.x2)res.x1=res.x2;
    if(res.y1>res.y2)res.y1=res.y2;
    return res;
  }
  int Xdim() {
    return x2-x1;
  }
  int Ydim() {
    return y2-y1;
  }
};
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

