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

void throw_foo(int a) { throw a; }

int return_foo(int a) { return a; }

int main() {
  double start = 1.0 * clock() / CLOCKS_PER_SEC;
  Rep(x, 1000000) {
    try {
      throw_foo(x);
    } catch (...) {
    }
  }
  cerr << (1.0 * clock() / CLOCKS_PER_SEC - start) << endl;

  start = 1.0 * clock() / CLOCKS_PER_SEC;
  Rep(x, 1000000) { return_foo(x); }
  cerr << (1.0 * clock() / CLOCKS_PER_SEC - start) << endl;
  return 0;
}
