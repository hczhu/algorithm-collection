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
#include <cassert>
#include <limits>
//INT_MAX INT_MIN
#include <limits.h>
#include <cstdarg>
#include <iterator>
#include <valarray>
#include <thread>
using namespace std;
#define debug(x) cerr<<#x<<"=\""<<x<<"\""<<" at line#"<<__LINE__<<endl;
constexpr int Fibo(int n) {
  return n > 1 ? Fibo(n - 1) + Fibo(n - 2) : 1;
}


int main() {
  constexpr int F10 = Fibo(10);
  int F10c = Fibo(10);
  static_assert(F10 == 89, "F10 is not equal to 89");
  cout << "constexpr = " << F10 << " const = " << F10c <<endl;
  const string str1("hello world.");
  const string str2("hello world.");
  cout << "str1 address: " << (uint64_t)str1.data() << " str2 address: " << (uint64_t)str2.data() << endl;
  const char* const ptr1("hello world");
  const char* const ptr2("hello world");
  cout << "ptr1 address: " << (uint64_t)ptr1 << " ptr2 address: " << (uint64_t)ptr2 << endl;
  return 0;
}
