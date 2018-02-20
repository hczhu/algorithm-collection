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
#include <cassert>
#include <limits>
// INT_MAX INT_MIN
#include <cstdarg>
#include <iterator>
#include <limits.h>
#include <valarray>
using namespace std;
#define debug(x)                                                               \
  cerr << #x << "=\"" << x << "\""                                             \
       << " at line#" << __LINE__ << endl;
#define Rep(i, n) for (int i = 0; i < (n); ++i)
#define For(i, a, b) for (typeof(a) i = a; i < (b); ++i)
#define SZ(vec) ((int)(vec).size())
#define All(vec) (vec).begin(), (vec).end()

int AddAll(int a...) {
  va_list ap;
  va_start(ap, a);
  int re = a;
  while ((a = va_arg(ap, int)) != -1) {
    re += a;
  }
  return re;
}

int main() {
  debug(AddAll(1, 2, 3, 4, -1));
  debug(AddAll(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, -1));
  return 0;
}
