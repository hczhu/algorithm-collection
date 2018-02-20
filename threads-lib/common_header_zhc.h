#ifndef __COMMON_HEADER_ZHC__
#define __COMMON_HEADER_ZHC__

#include <algorithm>
#include <cmath>
#include <complex>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <vector>
//#define NDEBUG
#include <assert.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#define debug(x)                                                               \
  std::cerr << #x << "=\"" << x << "\""                                        \
            << " at line#" << __LINE__ << " in file " << __FILE__              \
            << std::endl;
//#define debug(x)

#define hline()                                                                \
  std::cerr << "-----------------------------------------" << std::endl;
//#define hline()

#define Func_tester(fun)                                                       \
  class Test_##fun {                                                           \
  public:                                                                      \
    Test_##fun() { fun(); };                                                   \
  } Test_obj_##fun
//#define Func_tester(fun)

#define UL_LOG_FATAL stderr
#define UL_LOG_WARNING stderr
#define UL_LOG_NOTICE stderr
#define UL_LOG_DEBUG stderr
#define UL_LOG_TRACE stderr
#define ul_writelog fprintf
typedef unsigned int uint32_t;

#define CHECK_NULL_POINTER(p, ret)                                             \
  if (NULL == (p)) {                                                           \
    ul_writelog(UL_LOG_FATAL, "NULL real pointer [%s]", #p);                   \
    return ret;                                                                \
  };
#define CHECK_NULL_VADDRESS32(p, ret)                                          \
  if (NULL_VADDRESS32 == (p)) {                                                \
    ul_writelog(UL_LOG_FATAL, "NULL Vaddress32 [%s]", #p);                     \
    return ret;                                                                \
  };

inline double get_wall_time() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + 1e-6 * tv.tv_usec;
}
inline double get_clock_time() { return 1.0 * clock() / CLOCKS_PER_SEC; }
class auto_timer_t {
  double start_time;
  const char *msg;

public:
  auto_timer_t(const char *p = "") : start_time(get_wall_time()), msg(p){};
  ~auto_timer_t() {
    ul_writelog(UL_LOG_WARNING, msg, get_wall_time() - start_time);
  };
};

inline int better_strncpy(char *dest, const char *source, size_t length) {
  size_t len = strlen(source);
  int ret = 0;
  if (length > len) {
    strcpy(dest, source);
  } else {
    memcpy(dest, source, length);
    dest[length - 1] = 0;
    ret = 1;
  }
  return ret;
}

#endif
