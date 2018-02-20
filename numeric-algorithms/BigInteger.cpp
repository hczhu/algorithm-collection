#include <algorithm>
#include <cmath>
#include <complex>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <vector>
//#define NDEBUG
#include <cassert>
using namespace std;
#ifndef NDEBUG
#define debug(x)                                                               \
  cerr << #x << "=\"" << x << "\""                                             \
       << " at line#" << __LINE__ << endl;
#define hline() cerr << "-----------------------------------------" << endl;
#else
#define debug(x)
#define hline()
#endif
#define Throw(x)                                                               \
  fprintf(stderr, "%s at [line:%d] [file:%s]\n", x, __LINE__, __FILE__);       \
  throw x;

template <unsigned int Base, unsigned int Len> class BigInteger {
public:
  typedef unsigned int uint;
  typedef long long int llint;
  uint dig[Len];
  int sig, head;
  static uint base;
  static int flag;
  int abs_compare(const BigInteger &other) {
    if (head != other.head)
      return head > other.head ? 1 : -1;
    for (int i = head; i >= 0; i--)
      if (dig[i] != other.dig[i])
        return dig[i] > other.dig[i] ? 1 : -1;
    return 0;
  };
  static uint get_base() {
    uint base;
    const uint upper = sqrt((uint)0xffffffff);
    for (base = 1; base <= upper / Base; base *= Base)
      ;
    return base;
  };
  void set_value(int a) { return set_value((llint)a); };
  void set_value(llint a) {
    sig = head = 0;
    if (a < 0)
      a = -a, sig = 1;
    head = -1;
    do {
      //  if(head+1>=Len)Throw("Overflow");
      assert(head + 1 < Len);
      dig[++head] = a % base;
      a /= base;
    } while (a);
  };

  void set_value(
      const char *str,
      const char *sym =
          "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") {
    assert(strlen(sym) >= Base);
    // if(strlen(sym)<Base)Throw("Too small sym table in set_value");
    int width = 0;
    for (uint p = base; p > 1; p /= Base)
      width++;
    head = -1, sig = 0;
    if (str[0] == '-')
      sig = 1, str++;
    for (int i = strlen(str) - 1; i >= 0; i -= width) {
      uint d = 0;
      for (int j = max(0, i - width + 1); j <= i; j++) {
        d *= Base;
        const char *p = strchr(sym, str[j]);
        if (p)
          d += (p - sym);
      }
      assert(head + 1 < Len);
      dig[++head] = d;
    }
    assert(head >= 0);
    adjust();
  };

  BigInteger(int v = 0) { set_value(v); };
  BigInteger(llint v) { set_value(v); };
  BigInteger(const BigInteger &other) { *this = other; };
  void abs_subtract(const BigInteger &b) {
    assert(abs_compare(b) >= 0);
    int ca = 0;
    for (int i = 0; i <= b.head; i++) {
      dig[i] -= b.dig[i] + ca;
      ca = 0;
      if (dig[i] < 0)
        dig[i] += base, ca = 1;
    }
    assert(ca == 0 || head > b.head);
    if (ca) {
      for (int i = b.head + 1; i <= head; i++) {
        if (dig[i]) {
          for (int j = b.head + 1; j < i; j++)
            dig[j] = base - 1;
          dig[i]--;
          ca = 0;
          break;
        }
      }
    }
    assert(0 == ca);
  };
  void abs_add(const BigInteger &b) {
    int ca = 0;
    while (head < b.head)
      dig[++head] = 0;
    for (int i = 0; i <= b.head; i++) {
      dig[i] += ca + b.dig[i];
      ca = 0;
      if (dig[i] >= base)
        ca = 1, dig[i] -= base;
    }
    if (ca) {
      for (int i = b.head + 1; i <= head; i++) {
        if (dig[i] + 1 < base) {
          dig[i]++;
          for (int j = b.head + 1; j < i; j++)
            dig[j] = 0;
          ca = 0;
          break;
        }
      }
      if (ca) {
        assert(head + 1 < Len);
        dig[++head] = ca;
        ca = 0;
      }
    }
    assert(ca == 0);
  }
  void adjust() {
    while (head && dig[head] == 0)
      head--;
    if (is_zero())
      sig = 0;
  }
  const BigInteger &operator+=(const BigInteger &b) {
    if (sig ^ b.sig) {
      if (abs_compare(b) >= 0)
        abs_subtract(b);
      else {
        BigInteger tmp = b;
        tmp.abs_subtract(*this);
        *this = tmp;
      }
    } else {
      abs_add(b);
    }
    adjust();
    return *this;
  };
  const BigInteger &operator+(const BigInteger &other) const {
    BigInteger tmp = *this;
    tmp += other;
    tmp.adjust();
    return tmp;
  };
  bool is_zero() { return head == 0 && dig[0] == 0; };
  const BigInteger &operator-=(const BigInteger &b) {
    sig ^= 1;
    (*this) += b;
    sig ^= 1;
    adjust();
    return *this;
  };
  const BigInteger &operator-(const BigInteger &other) {
    BigInteger tmp(*this);
    tmp.sig ^= 1;
    tmp += other;
    tmp.sig ^= 1;
    tmp.adjust();
    return tmp;
  };

  const BigInteger &operator*=(const BigInteger &b);
  const BigInteger &operator*(const BigInteger &other);
  const BigInteger &operator*=(int v);

  const BigInteger &operator/=(const BigInteger &b);
  const BigInteger &operator/(const BigInteger &other);
  const BigInteger &operator/=(int v);

  const BigInteger &operator<<=(const BigInteger &other);
  const BigInteger &operator<<(const BigInteger &other);

  const BigInteger &operator>>=(const BigInteger &other);
  const BigInteger &operator>>(const BigInteger &other);

  const BigInteger &operator=(const BigInteger &other) {
    sig = other.sig;
    head = other.head;
    memcpy(dig, other.dig, sizeof(dig[0]) * (head + 1));
    return *this;
  };

  bool operator>(const BigInteger &other) { return compare(other) > 0; };
  bool operator>=(const BigInteger &other) { return compare(other) >= 0; };
  bool operator<(const BigInteger &other) { return compare(other) < 0; };
  bool operator<=(const BigInteger &other) { return compare(other) <= 0; };
  bool operator==(const BigInteger &other) { return compare(other) == 0; };

  int compare(const BigInteger &other) {
    if (sig ^ other.sig)
      return sig == 0 ? 1 : -1;
    return (sig == 0 ? 1 : -1) * abs_compare(other);
  };
  void
  print(const char *tail = "", FILE *file = stdout,
        const char *sym =
            "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")
      const {
    assert(strlen(sym) >= Base);
    if (sig)
      fprintf(file, "-");
    char seg[64];
    int top = 0;
    for (uint d = dig[head]; d; d /= Base)
      seg[top++] = sym[d % Base];
    reverse(seg, seg + top);
    seg[top] = 0;
    fprintf(file, "%s", seg);
    int width = 0;
    for (uint p = base; p > 1; p /= Base)
      width++;
    seg[width] = 0;
    for (int i = head - 1; i >= 0; i--) {
      uint d = dig[i];
      for (int j = 0; j < width; j++, d /= Base)
        seg[width - 1 - j] = sym[d % Base];
      fprintf(file, "%s", seg);
    }
    fprintf(file, "%s", tail);
  };
};
template <unsigned int Base, unsigned int Len>
uint BigInteger<Base, Len>::base = BigInteger<Base, Len>::get_base();
template <unsigned int Base, unsigned int Len>
int BigInteger<Base, Len>::flag = 0;

int main() {
  BigInteger<2, 100> aa, bb, cc;
  debug(aa.base);
  debug(aa.flag);
  aa.set_value("-11010101010010100010101");
  aa.print("\n");
  bb.set_value("00000000000111");
  bb.print("\n");
  cc = (aa + bb);
  cc.print("\n");
  cc = (aa - bb);
  cc.print("\n");
  cc = (bb - aa);
  cc.print("\n");
  return 0;
}
