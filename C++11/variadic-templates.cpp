
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
#include <limits>
#include <limits.h>
#include <cstdarg>
#include <iterator>
#include <valarray>
#include <thread>
#include <memory>
using namespace std;
#define debug(x) cerr<<#x<<"=\""<<x<<"\""<<" at line#"<<__LINE__<<endl;
template<typename Policy>
void print(Policy& outputStream) {
  outputStream << std::endl;
}

template<typename Policy, typename Arg1, typename... Args>
void print(Policy& outputStream, const Arg1& arg1, const Args&... args) {
  outputStream << arg1 << " ";
  print(outputStream, args...);
}

int main() {
  print(cout, 1, 2, "sfsfd", 1.34243, 'a', 3241234);
  string ss;
  stringstream os(ss);
  print(os, 1, 2, "sfsfd", 1.34243, 'a', 3241234);
  cout << os.str();
  return 0;
}
