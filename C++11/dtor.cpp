
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
  virtual ~Base() = default;
};

class ProtectedDtor : public Base {
 protected:
  virtual ~ProtectedDtor() = default;
};

class ProtectedDtorDeriv : public ProtectedDtor {
 public:
  virtual ~ProtectedDtorDeriv() = default;
};

class PrivateDtor : public Base {
 private:
  virtual ~PrivateDtor() = default;
};

int main() {
  {
    ProtectedDtor* pro_ptr = new ProtectedDtor();
    delete dynamic_cast<Base*>(pro_ptr);
  }
  {
    ProtectedDtorDeriv* pro_ptr = new ProtectedDtorDeriv();
    delete pro_ptr;
  }

  PrivateDtor* pri_ptr = new PrivateDtor();
  delete dynamic_cast<Base*>(pri_ptr);
  return 0;
}
