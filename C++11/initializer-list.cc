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

template<int S, typename T>
class FixedSizeVectorBase {
 public:
  FixedSizeVectorBase() = default;
  FixedSizeVectorBase(initializer_list<T> args) {
    int pos = 0;
    for (const auto& arg : args) {
      value_[pos++] = arg;
    }
  }
  void Print() const {
    cout << "{";
    for (int i = 0; i < S; ++i) {
      cout << value_[i] << ", ";
    }
    cout << "}" <<endl;
  }
 protected:
  T value_[S];
};

template<int S, typename T>
class FixedSizeVector : public FixedSizeVectorBase<S, T>  {
 public:
  FixedSizeVector(initializer_list<T> args)
    : FixedSizeVectorBase<S, T>(args) { }
};

template<int S>
class FixedSizeVector<S, char> : public FixedSizeVectorBase<S, char> {
 public:
  FixedSizeVector(const char value[]) {
    memcpy(FixedSizeVectorBase<S, char>::value_, (const void*)value, sizeof(char) * S);
  }
  FixedSizeVector(initializer_list<char> args)
    : FixedSizeVectorBase<S, char>(args) { }
  void Set(const char value[]) {
    memcpy(FixedSizeVectorBase<S, char>::value_, (const void*)value, sizeof(char) * S);
  }
};

template<int S>
using FixedString = FixedSizeVector<S, char>;

class Base {
protected:
  int data_;
};

class Derived : public Base {
 public:
  Derived(int data) {
    data_ = data;
  }
};

struct Simple {
  int a;
  char b;
  string c;
};

int main() {
  FixedString<7> fixChar{"abcdefg"};
  FixedSizeVector<5, int> fixInt{1, 2, 3, 4, 5};
  fixInt.Print();
  fixChar.Print();
  Simple simple{1, 'a', "adfasfa"};
  simple = Simple({2, 'b', "afasfas"});
  int ivalue;
  cin >> ivalue;
  Simple another{ivalue, 'c', "afads"};
  map<string, set<string>> complicated{
    { "aa", { "1", "2"}},
    { "bb", set<string>{}},
  };
  cout << complicated.size();
  cout << "-----------" << endl;
  std::string haha(100, 'a');
  std::cout << haha << std::endl;
  const std::vector<std::string> hahaVec{std::move(haha)};
  std::cout << haha << std::endl;
  return 0;
}
