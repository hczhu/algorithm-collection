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

int main() {
  int thread_num = 10;
  int count = 0;
  vector<thread> threads;
  mutex lock;
  for (int i = 0; i < thread_num; ++i) {
    // Trap! Have to value capture 'i'.
    threads.emplace_back([&, i](){
      lock.lock();
      cout << "counter = " << count++ << " for thread #"  << i << endl;
      lock.unlock();
    });
  }
  for (auto& thr : threads) {
    thr.join();
  }
  return 0;
}
