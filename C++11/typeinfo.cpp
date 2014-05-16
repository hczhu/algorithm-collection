
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
 public:
  void Foo() {}
  void ConstFoo() const {}
};

int main() {
  const unique_ptr<Example> ptr(new Example);
  ptr->Foo();
  using Map = map<Example, Example>;
  using ConstMap = const map<Example, Example>;
  using MapConst = map<Example, const Example>;
  ConstMap constMap;
  MapConst mapConst;
  Map myMap;
  const auto& constRef = myMap;
  cout << is_const<decltype(constRef)>::value << endl;
  cout << "--------------" << endl;
  cout << typeid(myMap.begin()->first).hash_code() << std::endl;
  cout << typeid(myMap.begin()->second).hash_code() << std::endl;
  cout << is_const<decltype(myMap.begin()->second)>::value << std::endl;
  cout << is_reference<decltype(myMap.begin()->second)>::value << std::endl;
  cout << "--------------" << endl;
  cout << typeid(mapConst.begin()->first).hash_code() << std::endl;
  cout << typeid(constMap.begin()->second).hash_code() << std::endl;
  cout << is_const<decltype(constMap.begin()->second)>::value << std::endl;
  cout << is_reference<decltype(myMap.begin()->second)>::value << std::endl;
  cout << "--------------" << endl;
  cout << typeid(mapConst.begin()->first).hash_code() << std::endl;
  cout << typeid(mapConst.begin()->second).hash_code() << std::endl;
  cout << is_const<decltype(mapConst.begin()->second)>::value << std::endl;
  cout << is_const<const Example&>::value << endl;
  return 0;
}
