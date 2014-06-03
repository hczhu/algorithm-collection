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
  int a;
  string b;
};

int main() {
  vector<Base> vv(5);
  decltype(vv)::const_iterator itr = vv.begin();
  for (int i = 0; i < 10; ++i) {
    auto citr = itr++;
    cout << (citr == itr) << " " << (itr < vv.end()) << endl;
  }
  return 0;
}
