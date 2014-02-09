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
#include <algorithm>
using namespace std;
#define debug(x) cerr<<#x<<"=\""<<x<<"\""<<" at line#"<<__LINE__<<endl;

class BigInteger {
public:
  BigInteger(const char* str, size_t n)
    : str_(str, n){}
  BigInteger(const BigInteger& thr) = default;
  BigInteger& operator=(const BigInteger& thr) = default;
  friend ostream& operator <<(ostream& out, const BigInteger& bi) {
    out << bi.str_; 
    return out;
  }
private:
  string str_;
};

BigInteger operator "" _big(const char* ptr) {
  return BigInteger(ptr, strlen(ptr));
}

string operator "" _fs(const char* ptr, size_t n) {
  return string(ptr, min((size_t)10, n));
}

template<typename Tag>
struct Distance {
  unsigned long long int d_;
  Distance(unsigned long long d) : d_(d) {}
  Distance(const Distance& d) = default;
  Distance& operator =(const Distance& d) = default;
  Distance operator+(Distance other) const {
    return d_ + other.d_;
  }
  static string Unit() {
    return Tag::GetName();
  }
  friend ostream& operator<<(ostream& os, const Distance& d) {
    os << d.d_ << Unit();
    return os;
  }
};

struct KmTag {
static string GetName() {
  return "km";
}
};

struct MileTag {
static string GetName() {
  return "mile";
}
};

typedef Distance<KmTag> Km;

typedef Distance<MileTag> Mile;

Km operator"" _km(unsigned long long km) {
  return Km(km);
}

Mile operator"" _mile(unsigned long long mile) {
  return Mile(mile);
}


int main() {
  const string ten = "012345678900012010201302"_fs;
  cout << ten << endl;
  BigInteger bi = 1341234312413513413413_big;
  cout << bi << endl;
  Km a(234132_km);
  Km aa(1_km);
  Mile b(234132_mile);
  cout << a << " + " << aa << " = " << (a + aa) << endl;
  cout << b << endl;
  return 0;
}
