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

class Base {
public:
  Base& operator=(Base&& other) = default;
  Base& operator=(const Base& other) = default;
  Base(const Base& other) = default;
  Base(const vector<int>& v) : v2(v) {}
  int data_;
  const vector<int> v1;
  const vector<int>& v2;
};

int main() {
  double c = 1.0;
  double d = 3;
  double e;
  cin >> e;
  double a = c / d;
  double b = a * e;
  cout << c << " == " << b << endl;
  // vector<int> vv;
  // Base base(vv);
  // Base base1 = base;
  // base1 = Base(vv);
  return 0;
}
