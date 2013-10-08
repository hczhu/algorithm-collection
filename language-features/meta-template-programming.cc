// Calculate Fibonacci numbers in compiling time.
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
using namespace std;
#define debug(x) cerr<<#x<<"=\""<<x<<"\""<<" at line#"<<__LINE__<<endl;
#define Rep(i,n) for(int i=0;i<(n);++i)
#define For(i,a,b) for(typeof(a) i=a;i<(b);++i)
#define SZ(vec) ((int)(vec).size())
#define All(vec) (vec).begin(),(vec).end()
typedef long long int llint;

template<int Index, llint FbN, llint FbN1>
class Fibonacci {
  public:
   static const llint fb = Fibonacci<Index - 1, FbN + FbN1, FbN>::fb;
};

template<llint FbN, llint FbN1>
class Fibonacci<0, FbN, FbN1> {
  public:
    static const llint fb = FbN;
};

int main() {
  cout << "Fibonacci[1] = << " << Fibonacci<1, 0 ,1>::fb << endl;
  cout << "Fibonacci[10] = << " << Fibonacci<10, 0 ,1>::fb << endl;
  cout << "Fibonacci[100] = << " << Fibonacci<20, 0 ,1>::fb << endl;
  return 0;
}

