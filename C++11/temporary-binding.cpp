
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
#include <thread>
#include <mutex>
// using namespace std;
#define debug(x) cerr<<#x<<"=\""<<x<<"\""<<" at line#"<<__LINE__<<endl;

class String {
 public:
  String() {
    std::cout << "String Ctor @" << this << std::endl;
  }
  ~String() {
    std::cout << "String Dtor @" << this << std::endl;
  }
 private:
  int a_{-1};
};

class StringPiece {
 public:
  StringPiece(const String& str) {
    std::cout<<"Ctor const& @" << &str << std::endl;
  }
/*
  StringPiece(String&& str) {
    std::cout<<"Ctor && @" << &str << std::endl;
  }
*/
};

String foo() {
  return String();
}

void Bar(StringPiece sp) {
  int a;
  std::cout << "Stack of Bar() @" << &a << std::endl;
  std::cout << "End of Bar." << std::endl;
}

class Sandbox {
public:
    Sandbox(const String& n) : member(n) {
      std::cout << "Ctor Sandbox @" << this << " n @" << &n << std::endl;
    }
    const String& member;
  ~Sandbox() {
    std::cout << "Dtor Sandbox @" << this << std::endl;
  }
};

int main() {
  {
  StringPiece sp(foo());
  Bar(foo());
  std::cout << "out of Bar()" << std::endl;
  std::cout << "---------------------" << std::endl;
  }
  {
  Sandbox sandbox{String()};
  std::cout << "---------------------" << std::endl;
  }
  const auto& spRef = String();
  std::cout << "---------------------" << std::endl;
  return 0;
}
