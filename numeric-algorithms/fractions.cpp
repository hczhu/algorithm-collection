#include <algorithm>
#include <stdio.h>
#define NDEBUG
#include <assert.h>
#include <cctype>
#include <iostream>
using namespace std;
typedef long long int llint;
llint __gcd(llint a, llint b) {
  // while(a==0&&b==0);
  while (b) {
    llint r = a % b;
    a = b;
    b = r;
  }
  return a;
}
class fraction {
#define myabs(x) ((x < 0) ? -(x) : (x))
  llint a, b;
  void simplify() {
    while (b == 0)
      ;
    assert(b);
    llint g = __gcd(myabs(a), myabs(b));
    a /= g;
    b /= g;
    if (b < 0)
      b = -b, a = -a;
  };

public:
  fraction(llint aa = 0, llint bb = 1) : a(aa), b(bb) {
    assert(b);
    simplify();
  };
  void setValue(llint aa, llint bb) {
    a = aa, b = bb;
    assert(b);
    simplify();
  };
  void getValue(llint &aa, llint &bb) { aa = a, bb = b; };
  fraction operator+(const fraction &fb);
  fraction operator+=(const fraction &ff);

  fraction operator-(const fraction &ff);
  fraction operator-=(const fraction &ff);

  fraction operator*(const fraction &ff);
  fraction operator*=(const fraction &ff);

  fraction operator/(const fraction &ff);
  fraction operator/=(const fraction &ff);
  void print() { cout << a << "/" << b << endl; };
  friend bool operator==(const fraction &fa, const fraction &fb) {
    return fa.a == fb.a && fa.b == fb.b;
  };
  friend bool operator<(const fraction &fa, const fraction &fb) {
    fraction d = fraction(fa) - (fb);
    return d.a < 0;
  };
  friend bool operator>(const fraction &fa, const fraction &fb) {
    fraction d = fraction(fa) - fb;
    return d.a > 0;
  };
};

fraction fraction::operator+(const fraction &fb) {
  llint g = __gcd(myabs(b), myabs(fb.b));
  fraction res;
  res.b = b / g * fb.b;
  res.a = a * (fb.b / g) + fb.a * (b / g);
  res.simplify();
  return res;
}
fraction fraction::operator+=(const fraction &fb) {
  fraction res = (*this) + fb;
  *this = res;
  return *this;
}
fraction fraction::operator-(const fraction &fb) {
  llint g = __gcd(myabs(b), myabs(fb.b));
  fraction res;
  res.b = b / g * fb.b;
  res.a = a * (fb.b / g) - fb.a * (b / g);
  res.simplify();
  return res;
}
fraction fraction::operator-=(const fraction &fb) {
  fraction res = (*this) - fb;
  *this = res;
  return *this;
}
fraction fraction::operator*(const fraction &fb) {
  llint g1 = __gcd(myabs(a), myabs(fb.b));
  llint g2 = __gcd(myabs(fb.a), myabs(b));
  fraction res(a / g1 * (fb.a / g2), b / g2 * (fb.b / g1));
  res.simplify();
  return res;
}
fraction fraction::operator*=(const fraction &fb) {
  fraction res = (*this) * fb;
  *this = res;
  return *this;
}
fraction fraction::operator/(const fraction &fb) {
  while (fb.a == 0)
    ;
  llint g1 = __gcd(myabs(a), myabs(fb.a));
  llint g2 = __gcd(myabs(fb.b), myabs(b));
  fraction res(a / g1 * (fb.b / g2), b / g2 * (fb.a / g1));
  res.simplify();
  return res;
}
fraction fraction::operator/=(const fraction &fb) {
  fraction res = (*this) / fb;
  *this = res;
  return *this;
}
