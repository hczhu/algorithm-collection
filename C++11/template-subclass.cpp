
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

class Base {
 public:
  Base(const std::string& name) : name_(name) {
  }
  virtual ~Base() {}
  virtual std::string name() const = 0;
 protected:
  const std::string name_;
};

template<typename T>
class Derived1 : public Base {
 public:
  Derived1(const T& v) : Base("Derived1"), vec_{v} {
    std::cout << "size = " << vec_.size() << std::endl;
  }
  virtual std::string name() const override {
    return name_ + " " + typeid(T).name();
  }
 private:
  std::vector<T> vec_;
};

int main() {
  Base* ptr_int = new Derived1<int>(20);
  Base* ptr_double = new Derived1<double>(20.0);
  std::cout << ptr_int->name() << " VS " << ptr_double->name() << std::endl;
  delete ptr_int;
  delete ptr_double;
  return 0;
}
