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

int main() {
  map<int, unique_ptr<Int>> mem;
  mem.emplace(1, unique_ptr<Int>(new Int(1)));
  mem[0] = unique_ptr<Int>(new Int(0));
  cout << mem[0]->v_ << endl;
  auto itr = mem.find(1);
  cout << itr->second->v_ << endl;
  mem.erase(itr);

  vector<unique_ptr<Int>> vec;
  vec.emplace_back(new Int(2));
  vec.emplace_back(new Int(3));
  vec.push_back(std::move(vec[0]));
  cout << vec[0].get() << endl;
  return 0;
}

Int::~Int() {
  cout << "Dtor is called for v_ = "  << v_ << endl;
}
