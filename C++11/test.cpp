
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <iostream>
#include <cmath>
#include <unordered_map>
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
#include <fstream>
//using namespace std;
#define debug(x) cerr<<#x<<"=\""<<x<<"\""<<" at line#"<<__LINE__<<endl;
#define flag(x) FLAGS_##x

class Foo {
public:
  Foo() {
    ptr_ = nullptr;
    print();
  }
  Foo(const Foo& other) {
    std::cout << "Copy ctor." << std::endl;
  }
  Foo(Foo&& other) {
    std::cout << "Move ctor." << std::endl;
  }
  ~Foo() {
    std::cout << "dtor Foo." << std::endl;
  }
  void print() {
    ptr_ = (const std::string*)0x111;
    std::cout << "Foo" << std::endl;
  }
  void printValue(int value) {
    std::cout << "Foo " << value << std::endl;
  }
  void printValue2(int value, int value1) {
    std::cout << "Foo " << value << " " << value1 << std::endl;
  }
  int& operator[](int idx) {
    using namespace std;
     std::cout << "operator[" << idx << "]" << std::endl;
     return array_[idx];
  }
private:
  std::array<int, 10> array_;
  const std::string* ptr_;
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
/*

std::vector<std::string> names{std::string("Hongcheng"), "Lixia"};
Foo foo_1, foo_2, foo_3;
std::vector<Foo> allFoo{foo_1, foo_2, foo_3};

template<typename Method, typename... Args>
void forEachName(Method methodPtr, const Args&... args) {
  for (auto& foo : allFoo) {
    (foo.*methodPtr)(args...);
  }
}

typedef void (Foo::*FooMethod)();
#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

void callFoo(Foo& foo, FooMethod method) {
  (foo.*method)();
  // CALL_MEMBER_FN(foo, method)();
}
*/

struct MoveOnly {
  int* ptr = nullptr;
  MoveOnly() = default;
  MoveOnly(const MoveOnly&) = delete;
  MoveOnly(MoveOnly&&) = default;
  operator int() const {
    return 0;
  }
  std::string str_{"aaa"};
  String Str_{"badfasd"};
 private:
  class SubMove {
  };
  SubMove Foo();
};

MoveOnly::SubMove MoveOnly::Foo() {
  return SubMove();
}

template<typename Arg>
size_t getHash(const Arg& arg) {
  return std::hash<Arg>()(arg);
}

template<typename Arg1, typename... Args>
size_t getHash(const Arg1& arg1, const Args&... args) {
  return std::hash<Arg1>()(arg1) ^ getHash(args...);
}

std::ostream& printFoo(std::ostream& out) {
  return out << "printFoo()";
}

class Root {
 public:
  virtual void Foo() = 0;
};

class Implemeter {
 public:
  Implemeter() = default;
  Implemeter(int a, const char* p) : a_(a), p_(p) { };
  virtual void Foo() final {
    std::cout << "Implemeter::Foo() " << a_ << " " << p_ << std::endl;
  }
 private:
  int a_;
  const char* p_;
};

class Middle1 : public virtual Implemeter {
 public:
  ~Middle1() = default;
};

class Middle2 : public virtual Implemeter {
 public:
  ~Middle2() = default;
};

class Leaf : public Middle1, public Middle2 {

};

class CA {
 public:
  CA() = default;
  CA(const CA&) = default;
/*
  CA(CA&& other) {
    std::cout << "move @" << &other << " to @" << this << std::endl;
    a_ = 1; other.a_ = 0;
  }*/
  ~CA() { std::cout << "CA::~CA()" << std::endl;}
  int a_{1};
};

class CB {
 public:
  CB() = default;
  CB(CA& ca) : ca(std::move(ca)) { std::cout << "here" << std::endl; }
  ~CB() { std::cout << "CB::~CB()" << std::endl;}
  CA ca;
};

void Fooo(std::shared_ptr<const Implemeter> imp) {

}

class StrStr {
 public:
  StrStr(std::string) {
    std::cout << "value" << std::endl;
  }
  StrStr(std::string&&) {
    std::cout << "move" << std::endl;
  }
  StrStr(const StrStr& other) {
    std::cout << "value ctor." << std::endl;
  }
  StrStr(StrStr&& other) {
    std::cout << "move ctor." << std::endl;
  }
  StrStr operator=(StrStr other) {
    std::cout << "value =." << std::endl;
    return *this;
  }
  StrStr operator=(StrStr&& other) {
    std::cout << "move =." << std::endl;
    return *this;
  }
};

void FooStrStr(StrStr value) {

}

static const std::string kStr = "kStr";
int main() {
  const std::string& refStr = kStr;
  std::string strPtr = &kStr;
  std::string hehe;
  StrStr strStr(hehe);
  std::cout << "--------" << std::endl;
  FooStrStr(strStr);
  FooStrStr(std::move(strStr));
  return 0;
  std::string aaaaaa;
  std::cout << sizeof(aaaaaa) << std::endl;
  std::cout << sizeof(std::string) << std::endl;
  std::shared_ptr<Middle1 const> mid1 = std::make_shared<Middle1>();
  Fooo(mid1);
  const std::unique_ptr<std::string> pstring(new std::string("haha"));
  pstring->clear();
  CA ca;
  CA caa(std::move(ca));
  return 0;
  CB cb(ca);
  std::cout << ca.a_ << " " << cb.ca.a_ << std::endl;
  return 0;
  std::unique_ptr<int> nullp;
  if (!nullp) {
    std::cout << "nullp" << std::endl;
  }
  return 0;
  std::vector<Implemeter> si{
    {10, "faadsf"},
    {10, "faadsf"},
  };
  for (auto& imp : si) {
    imp.Foo();
  }
  std::vector<std::string> vs{
    std::string{10, 'a'},
    {10, 'b'},
    {"hahah"},
  };
  vs.emplace_back(3, 'c');
  std::cout << vs[0] << "|" << vs[1] << "|" << vs[2] << std::endl;
  std::cout << std::string{10, 'd'} << std::endl;
  return 0;
  Leaf().Foo();
  std::cout << printFoo << std::endl;
  printFoo(std::cout);
  std::cout << getHash(1, 2, std::string("sdfasf"), 2423.3) << std::endl;
  return 0;
  bool abool = false;
  abool |= true;
  std::cout << std::numeric_limits<char>::min() + '\1' << std::endl;
  std::cout << (int)std::numeric_limits<char>::max() << std::endl;
  return 0;
  {
    const std::string a = "afasd";
    const std::string b = "\xff\xff";
    std::cout << int(a[0]) << " V.S. " << int(b[0]) << std::endl;
    assert(a[0] < b[0]);
    assert(a < b);
  }
  std::unique_ptr<int> intPtr;
  if (!intPtr) {
    std::cout << "nullptr" << std::endl;
  }
  std::ifstream input(std::ifstream("/dev/null"));
  const std::shared_ptr<std::vector<int>> sharedVec = std::make_shared<std::vector<int>>();
  sharedVec->push_back(10);
  int a;
  MoveOnly mo1;
  mo1.ptr = &a;
  MoveOnly mo2(std::move(mo1));
  std::cout << mo1.ptr << " VS " << mo2.ptr << std::endl;
  fflush(stdout);
/*
  std::unordered_map<int, Foo> fooMap;
  fooMap.insert(std::make_pair(10, Foo()));
  fooMap.erase(10);
  std::cout << "at last." << std::endl; */
  std::vector<Foo> fooVec;
  fooVec.push_back(Foo());
  fooVec.pop_back();
  std::cout << "at last." << std::endl;
  return 0;
/*
  auto mth = &Foo::print;
  std::cout << typeid(mth).name() << std::endl;
  std::mem_fn(mth)(foo_1);
  // callFoo(foo_1, &Foo::print);
  forEachName(&Foo::printValue2, -100, -200);
  std::cout << names[0].length() << std::endl;
  auto foos = std::forward_as_tuple(foo_1, foo_2, foo_3);
  auto foos1 = std::make_tuple(foo_1, foo_2, foo_3);
  auto foos2 = std::tie(foo_1, foo_2, foo_3);
  // std::for_each(allFoo.begin(), allFoo.end(), std::mem_fn<>(&Foo::print));
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
  return 0; */
}
