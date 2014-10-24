
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <iostream>
#include <cmath>
#include <array>
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
#define flag(x) FLAGS_##x

void foo(const vector<int>& ints= {1, 2}) {
  for (auto i : ints) {
    cout << i << " ";
  }
  cout << endl;
}
class Foo {
public:
  int& operator[](int idx) {
     cout << "operator[" << idx << "]" << endl;
     return array_[idx];
  }
private:
  array<int, 10> array_;
};


int main() {
  std::vector<Foo> fooVec;
  fooVec.emplace_back();
  fooVec.emplace_back();
  const auto& fooVecRef = fooVec;
  int p = fooVecRef[0][2];
  static constexpr int kMax = 10;
  static const int kConst = 200;
  array<int, kConst> ar;
  unique_ptr<int> ptr(new int(100));
  auto& ptr_ref = ptr;
  *ptr_ref = -100;
  cout << *ptr << endl;
  double a = std::numeric_limits<double>::lowest();
  cout << a << " " << a + a << endl;
  cout << ((a + a) > a) << endl;
  cout << ((a + a) < a) << endl;
  cout << (a > (a + a)) << endl;
  vector<int> aa(2);
  vector<int> bb(3);
  cout << (aa == bb) << endl;
  
  {
    foo();
    Foo f;
    int a = 10;
    a += (f[2] = -1);
    cout << a << endl;
  }
  return 0;
}
