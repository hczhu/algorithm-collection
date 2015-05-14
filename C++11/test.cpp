
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
//using namespace std;
#define debug(x) cerr<<#x<<"=\""<<x<<"\""<<" at line#"<<__LINE__<<endl;
#define flag(x) FLAGS_##x

class Foo {
public:
  Foo() {
    print();
  }
  Foo(const Foo& other) {
    std::cout << "Copy ctor." << std::endl;
  }
  Foo(Foo&& other) {
    std::cout << "Move ctor." << std::endl;
  }
  virtual void print() {
    std::cout << "Foo" << std::endl;
  }
  int& operator[](int idx) {
    using namespace std;
     std::cout << "operator[" << idx << "]" << std::endl;
     return array_[idx];
  }
private:
  std::array<int, 10> array_;
};

class Foo1 : public Foo {
 public:
  virtual void print() {
    std::cout << "Foo1" << std::endl;
  }
  static void Caller() {
    Foo1 foo1;
    foo1.Foo3();
  }
 private:
  void Foo3() {}
};

struct String {
 public:
  String(const char* a) {
  }
};

void overloaded(const std::string& a) {
  std::cout << a << std::endl;
}

void overloaded1(const String& a) {
  // std::cout << a << std::endl;
}

void overloaded(bool a, int b) {
  std::cout << "bool: " << a << std::endl;
}

void FooFun(std::function<void()> fun = []() { std::cout << "haha" << std::endl;})  {
  fun();
}

std::string getStr() {
  return "aaaa";
}

void FooStr(std::string&& mv) {
  std::cout << mv << std::endl;
}

int main() {
  std::vector<std::string> names{std::string("Hongcheng"), "Lixia"};
  std::cout << names[0].length() << std::endl;
  std::for_each(names.begin(), names.end(), std::mem_fn(&std::string::length));
  Foo foo_1, foo_2, foo_3;
  auto foos = std::forward_as_tuple(foo_1, foo_2, foo_3);
  auto foos1 = std::make_tuple(foo_1, foo_2, foo_3);
  auto foos2 = std::tie(foo_1, foo_2, foo_3);
  std::vector<Foo> allFoo{foo_1, foo_2, foo_3};
  std::for_each(allFoo.begin(), allFoo.end(), std::mem_fn(&Foo::print));
  return 0;
  FooStr(getStr());
  auto Nan = std::numeric_limits<double>::quiet_NaN();
  std::cout << "Nan = " << Nan << std::endl;
  auto copyNan = 1.0;
  if ((Nan - copyNan) < 0.0001) {
    std::cout << "Equal." << std::endl;
  }
  std::cout << std::string("\003").length() << std::endl;
  std::cout << (int)std::string("\003")[0] << std::endl;
  std::cout << std::string("\\003").length() << std::endl;
  std::cout << std::string("\\003") << std::endl;
  auto lam = []() {std::cout << "haha hehe" << std::endl;};
  auto lam1 = [](int a) {std::cout << "haha hehe" << a << std::endl;};
  auto lam2 = [](int a) -> int {std::cout << "haha hehe" << a << std::endl; return 1;};
  std::cout << typeid(lam).name() << std::endl;
  std::cout << typeid(lam1).name() << std::endl;
  std::cout << typeid(lam2).name() << std::endl;
  std::cout << typeid(std::function<void()>).name() << std::endl;
  FooFun();
  FooFun([]() {
    std::cout << "hehe" << std::endl; });
  std::map<int, int> mapA{
    {1, 2},
  };
  std::map<int, int> mapB{
    {1, 2},
  };
  std::cout << (mapA == mapB) << std::endl;
  const char* pchar = "afdasf";
  overloaded(pchar);
  Foo1 foo1;
  int a;
  std::cin >> a;
  std::cout << (5&1) << std::endl;
  auto b = a ? 5 : 0 & 1;
  std::cout << b << std::endl;
  // decltype(std::min<int>)
  // std::function<const int&(const int&, const int&)> minMax = 1 ? std::max<int> : std::min<int>;
  std::vector<Foo> fooVec;
  fooVec.emplace_back();
  fooVec.emplace_back();
  const auto& fooVecRef = fooVec;
  return 0;
}
