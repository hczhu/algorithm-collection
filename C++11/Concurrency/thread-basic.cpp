
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <iostream>
#include <cmath>
#include <thread>
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
#include <mutex>
#include <memory>
//using namespace std;
#define debug(x) cerr<<#x<<"=\""<<x<<"\""<<" at line#"<<__LINE__<<endl;

std::mutex stdoutMutex;

int threadFoo(const std::thread::id& parentId) {
  {
    std::lock_guard<std::mutex> lock(stdoutMutex);
    std::cout << "Started Thread." << std::endl;
    std::cout << "parent id: " << parentId << std::endl;
    std::cout << "self id: " << std::this_thread::get_id() << std::endl;
  }
  std::chrono::milliseconds dura( 2000 );
  std::this_thread::sleep_for( dura );
  {
    std::lock_guard<std::mutex> lock(stdoutMutex);
    std::cout << "Waited 2000 ms for thread: " << std::this_thread::get_id() << std::endl;
  }
  return 1;
}

int main() {
  std::vector<std::thread> threads;
  std::thread::id thisId = std::this_thread::get_id();
  const auto threadNum = std::thread::hardware_concurrency();
  std::cout << "Using " << threadNum << " threads." << std::endl;
  for (int i = 0; i < threadNum; ++i) {
    threads.emplace_back([&thisId]() {
      threadFoo(thisId);
    });
  }
  for (auto& thr : threads) {
    thr.join();
  }
  return 0;
}
