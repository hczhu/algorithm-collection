
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
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

int main() {
  std::istringstream obj_a;
  std::istream* ptr_a = &obj_a;
  assert(nullptr != dynamic_cast<std::istringstream*>(ptr_a));

  std::ifstream obj_b;
  std::istream* ptr_b = &obj_b;
  assert(nullptr == dynamic_cast<std::istringstream*>(ptr_b));
  return 0;
}
