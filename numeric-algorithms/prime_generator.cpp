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
#define NDEBUG
#include <cassert>
#include <cstdarg>
#include <limits>
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
#define Two(x) ((1U) << (x))
typedef unsigned int uint;
const int prime_seed_number = 3;
bool isPrime(uint n) {
  if (n < 2)
    return false;
  uint upper = sqrt((double)n);
  while ((upper + 1) * (upper + 1) <= n)
    upper++;
  for (uint x = 2; x <= upper; x++) {
    if (0 == (n % x))
      return false;
  }
  return true;
}
void generate_seed_prime(uint prime_seed[]) {
  // FLAGS_prime_seed_number=max(FLAGS_prime_seed_number,20);
  int top = 0;
  for (uint x = 2; top < prime_seed_number; x++) {
    if (isPrime(x)) {
      prime_seed[top++] = x;
    }
  }
}
uint gcd(uint a, uint b) {
  while (b) {
    uint r = a % b;
    a = b, b = r;
  }
  return a;
}
char out_buf[100000000];
uint *generate_prime(uint n) {
  uint prime_seed[prime_seed_number];
  generate_seed_prime(prime_seed);
  uint p = 1, euler = 1;
  for (int i = 0; i < prime_seed_number; i++)
    p *= prime_seed[i], euler *= prime_seed[i] - 1;
  debug(euler);
  debug(p);
  debug(((n + p - 1) / p * euler + 31) / 32 * 4);
  debug(1.0 * euler / p);
  const uint bsize = ((n + p - 1) / p * euler + 31) / 32;
  uint *prime = new uint[(uint)(sqrt(1.0 * n)) + 1];
  uint *bit_mask = new uint[((n + p - 1) / p * euler + 31) / 32];
  memset(bit_mask, 0, sizeof(bit_mask[0]) * bsize);
  const uint sq = sqrt(1.0 * n) + 1;
  int top = 0;
  uint *mod = new uint[euler];
  uint *hash = new uint[p];
  for (uint x = 1; x < p; x++) {
    if (gcd(x, p) == 1) {
      hash[x] = top;
      mod[top++] = x;
    }
  }
  assert(top == euler);
  top = 0;
  uint ux = n / p;
  debug(ux);
  debug(ux * euler);
  // return 0;
  int pidx = -1;
  const int step = 1;
  for (int i = 0; i < prime_seed_number; i++) {
    if ((++pidx % step) == 0) {
      printf("%u\n", prime_seed[i]);
    }
  }
  bit_mask[0] = 1;
  int out_top = 0;
  for (uint x = 0, idx = 0; x <= n; x += p) {
    for (int i = 0; i < euler; i++, idx++) {
      uint y = x + mod[i];
      if (y > n)
        break;
      if (0 == (Two((idx & 31)) & bit_mask[idx >> 5])) {
        if ((++pidx % step) == 0) {
          /*
          //printf("%u\n",y);
          int d=0,r;
          for(uint t=y;t;t/=10) {
            out_buf[out_top+(d++)]='0'+((t%10));
          }
          for(int
          j=d/2-1;j>=0;j--)swap(out_buf[out_top+j],out_buf[out_top+d-1-j]);
          out_top+=d;
          out_buf[out_top++]='\n';
          if(out_top>sizeof(out_buf)-100) {
            out_buf[out_top]=0;
            printf("%s",out_buf);
            out_top=0;
          }*/
        }
        if (y <= sq) {
          prime[top++] = y;
        }
      }
      int j = -1;
      do {
        j++;
        if (j == top || n / y < prime[j])
          break;
        //  printf("set %u\n",y*prime[j]);
        uint bidx = y * prime[j] / p * euler + hash[(y * prime[j] % p)];
        assert(0 == (bit_mask[bidx >> 5] & Two(bidx & 31)));
        bit_mask[bidx >> 5] |= Two(bidx & 31);
      } while (y % prime[j]);
    }
  }
  out_buf[out_top] = 0;
  //  printf("%s",out_buf);
  return bit_mask;
}
int main() {
  generate_prime(1000000000);
  return 0;
}
