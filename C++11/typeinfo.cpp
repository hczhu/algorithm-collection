
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
class Example {
};

int main() {
  using ConstMap = const map<Example, Example>;
  using MapConst = map<Example, const Example>;
  ConstMap constMap;
  MapConst mapConst;
  cout << typeid(mapConst.begin()->first).name() << std::endl;
  cout << typeid(constMap.begin()->second).name() << std::endl;
  cout << is_const<decltype(constMap.begin()->second)>::value << std::endl;
  cout << typeid(mapConst.begin()->first).name() << std::endl;
  cout << typeid(mapConst.begin()->second).name() << std::endl;
  cout << is_const<decltype(mapConst.begin()->second)>::value << std::endl;
  return 0;
}
