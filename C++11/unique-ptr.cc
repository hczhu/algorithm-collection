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
struct Int {
  Int(int v) : v_(v) {}
  int operator()()const {
    return v_;
  }
  ~Int();
  int v_;
};

vector<int> Dummy() {
  vector<int> dummy();
  return dummy;
}

int main() {
  unique_ptr<int> ptr_int(new int(100));
  auto& ptr_int_ref = ptr_int;
  return 0;
}

Int::~Int() {
  cout << "Dtor is called for v_ = "  << v_ << endl;
}
