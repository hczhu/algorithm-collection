
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

template<class Value>
class Wrapper {
 public:
  template<class Vector>
  Wrapper(const Vector& vec) : vector_(vec.begin(), vec.end()) {}
  
  Wrapper(const Value v) : vector_(10, v) {}
 private:
  vector<Value> vector_;
};

int main() {
  Wrapper<double> wrapper1(vector<int>(10, 1));
  Wrapper<double> wrapper2(2.1);
  Wrapper<int> wrapper3(2);
  Wrapper<int> wrapper4(vector<double>(10, 1.3));
  return 0;
}
