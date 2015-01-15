
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

enum class Animal {
  CAT,
  DOG, 
};

enum Test {
  A = 0,
  B = 1, 
};

int main() {
  int a[100];
  if (bool b = (1 == a[0]) && a[1] == 2) {
    cout << "yes" << endl;
  }
  auto test = static_cast<Test>(2);
  auto animal = static_cast<Animal>(10);
  cout << test << endl;
  return 0;
}
