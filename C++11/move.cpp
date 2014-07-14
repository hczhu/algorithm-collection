
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

class Base {
public:
  const int& a;
  Base(int& b) : a(b) {
  }
};

int main() {
  int x = 10, y = -10;
  Base b1(x);
  Base b2 = b1;
  Base b3(y);
  cout << b3.a << endl;
  {
    string aa = "aaaa";
    cout << aa << endl;
    string bb = std::move(aa);
    cout << aa << endl;
  }
  {
    cout << "move const reference." << endl;
    string aa = "xxxxx";
    const auto& aaa = aa;
    string cc = std::move(aaa);
    cout << aa << endl;
    cout << aaa << endl;
    cout << cc << endl;
    cout << "move const reference." << endl;
  }
  return 0;
}
