
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
  // decltype(std::min<int>)
  std::function<const int&(const int&, const int&)> minMax = 1 ? std::max<int> : std::min<int>;
  std::vector<Foo> fooVec;
  fooVec.emplace_back();
  fooVec.emplace_back();
  const auto& fooVecRef = fooVec;
  return 0;
}
