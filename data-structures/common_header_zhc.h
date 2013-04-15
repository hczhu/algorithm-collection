#ifndef __COMMON_HEADER_ZHC__
#define __COMMON_HEADER_ZHC__

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
//#define NDEBUG
#include <assert.h>
#include <sys/types.h>
#include <sys/time.h>
#define debug(x) std::cerr<<#x<<"=\""<<x<<"\""<<" at line#"<<__LINE__<<" in file "<<__FILE__<<std::endl;
//#define debug(x)

#define hline() std::cerr<<"-----------------------------------------"<<std::endl;
//#define hline()
    
#define Func_tester(fun) class Test_##fun { public: Test_##fun (){ fun ();};}Test_obj_##fun
//#define Func_tester(fun)

#define LOCAL
#ifdef LOCAL
template<typename A,typename B>
void ul_writelog(A a,B b) {fprintf(stderr,b);fprintf(stderr,"\n");}
template<typename A,typename B,typename C>
void ul_writelog(A a,B b,C c) {fprintf(stderr,b,c);fprintf(stderr,"\n");}
template<typename A,typename B,typename C,typename D>
void ul_writelog(A a,B b,C c,D d) {fprintf(stderr,b,c,d);fprintf(stderr,"\n");}
template<typename A,typename B,typename C,typename D,typename E>
void ul_writelog(A a,B b,C c,D d,E e) {fprintf(stderr,b,c,d,e);fprintf(stderr,"\n");}
template<typename A,typename B,typename C,typename D,typename E,typename F>
void ul_writelog(A a,B b,C c,D d,E e,F f) {fprintf(stderr,b,c,d,e,f);fprintf(stderr,"\n");}
#define UL_LOG_FATAL stderr
#define UL_LOG_WARNING stderr
#define UL_LOG_NOTICE stderr
#define UL_LOG_DEBUG stderr
#define UL_LOG_TRACE stderr
#endif

#define CHECK_NULL_POINTER(p,ret) if(NULL==(p)) { ul_writelog(UL_LOG_FATAL,"NULL real pointer [%s]",#p);return ret;};
#define CHECK_NULL_VADDRESS32(p,ret) if(NULL_VADDRESS32==(p)) { ul_writelog(UL_LOG_FATAL,"NULL Vaddress32 [%s]",#p);return ret;};

inline double get_wall_time()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + 1e-6 * tv.tv_usec;
}
inline double get_clock_time()
{
  return 1.0*clock()/CLOCKS_PER_SEC;
}

class auto_timer_t
{
  double start_time;
  char* msg;
  auto_timer_t(const char* p=""):start_time(get_wall_time()),msg(p){};
  ~auto_timer_t()
  {
    WRITE_LOG()  
  };
};

#endif

