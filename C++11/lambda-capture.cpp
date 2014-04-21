
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

class A {
  public:
    A(int& a) : a_(a) {}
    void operator()() const {
      a_ = 100;
    }
  private:
    int& a_;
};

vector<int> Gvec;

int main() {
  int x = 0;
  auto lambda1 = [x]() mutable {Gvec.push_back(10);x = -1; cout << "lambda1 x=" << x << endl; };
  auto lambda4 = [&x]() { cout << "lambda4 x=" << x << endl; };
  cin >> x;
  auto lambda2 = [x]() { cout << "lambda2 x=" << x << endl; };
  auto lambda5 = [&x]() { cout << "lambda5 x=" << x << endl; };
  x++;
  auto lambda3 = [x]() { cout << "lambda3 x=" << x << endl; };
  auto lambda6 = [&x]() { cout << "lambda6 x=" << x << endl; };
  lambda1();
  cout << Gvec.size() << endl;
  lambda1();
  cout << Gvec.size() << endl;
  lambda2();
  lambda3();
  lambda4();
  lambda5();
  lambda6();
  return 0;
}
