#include <stdio.h>
//#define NDEBUG
#include <algorithm>
#include <assert.h>
#include <iostream>
#include <memory.h>
#include <set>
#include <vector>
using namespace std;
#define N 100
#define OUTPUT
namespace generalMatching {
#define ODD '1'
#define EVEN '2'
#define WHITE '0'
bool edge[2 * N][2 * N];
int match[2 * N];
bool removed[2 * N];
int flower[2 * N][2 * N], ftop[2 * N];
int root[N * 2];
void contract(int list[], int len, int n) {
#ifdef OUTPUT
  printf("Contraction vertices %d\n", n);
  for (int i = 0; i < len; i++)
    printf("%d ", list[i]);
  puts("");
  assert(len & 1);
#endif
  ftop[n] = len;
  for (int i = 0; i <= n; i++)
    edge[i][n] = edge[n][i] = false;
  for (int i = 0; i < len; i++)
    removed[list[i]] = true, flower[n][i] = list[i], root[list[i]] = n;
  root[n] = -1;
  for (int j = 0; j < n; j++) {
    if (removed[j])
      continue;
    for (int i = 0; i < len; i++)
      if (edge[list[i]][j]) {
        edge[n][j] = edge[j][n] = true;
        break;
      }
  }
}
void augment(const int s, int path[], int len, int n) {
  int f = 1;
#ifdef OUTPUT
  puts("Augment path");
  puts("No expansion");
  for (int i = 0; i < len; i++)
    printf("%d--", path[i]);
  puts("");
  puts("Expanded path");
#endif
  while (len > 1) {
    const int v = path[--len];
    assert(v < n);
    while (path[len - 1] >= n) {
      int super = path[--len];
      assert(edge[super][v]);
      assert(ftop[super] & 1);
      if (f) {
        int i;
        for (i = 0; i < ftop[super] && !edge[flower[super][i]][v]; i++)
          ;
        assert(i < ftop[super]);
        if (0 == (i & 1))
          for (int j = 0; j <= i; j++)
            path[len++] = flower[super][j];
        else {
          path[len++] = flower[super][0];
          for (int j = ftop[super] - 1; j >= i; j--)
            path[len++] = flower[super][j];
        }
      } else {
        int next = s;
        if (len)
          next = path[len - 1];
        int i;
        for (i = 0; i < ftop[super] && !edge[flower[super][i]][next]; i++)
          ;
        assert(i < ftop[super]);
        if (0 == (i & 1))
          for (int j = i; j >= 0; j--)
            path[len++] = flower[super][j];
        else {
          for (int j = i; j < ftop[super]; j++)
            path[len++] = flower[super][j];
          path[len++] = flower[super][0];
        }
      }
    }
    assert(len > 0 && path[len - 1] < n);
    if (f)
      match[v] = path[len - 1], match[path[len - 1]] = v;
    assert(edge[v][match[v]]);
    f ^= 1;
#ifdef OUTPUT
    printf("%d-", v);
#endif
  }
  assert(len == 1);
#ifdef OUTPUT
  printf("%d\n", path[0]);
#endif
}
int getroot(int v) {
  if (root[v] == -1)
    return v;
  return (root[v] = getroot(root[v]));
}
int pedge[2 * N];
int stack[N * 3];
char color[N * 2];
int father[N * 2];
bool instack[N * 2];
bool findPath(const int s, int n) {
#ifdef OUTPUT
  printf("find path %d\n", s);
  puts("-------------------");
#endif
  memset(removed, false, sizeof(bool) * n);
  memset(color, WHITE, sizeof(char) * n);
  memset(root, 0xff, sizeof(int) * n);
  const int nn = n;
  int top = 0;
  stack[top++] = s;
  pedge[s] = 0;
  color[s] = EVEN;
  father[s] = -1;
  while (top > 0) {
    const int v = stack[--top];
    instack[v] = false;
#ifdef OUTPUT
    printf("Expore %d\n", v);
#endif
    if (color[v] == ODD)
      continue;
    assert(color[v] == EVEN);
    while (pedge[v] < n &&
           (!edge[v][pedge[v]] || removed[pedge[v]] || color[pedge[v]] == ODD))
      pedge[v]++;
    if (pedge[v] == n)
      continue;
    stack[top++] = v;
    instack[v] = true;
    int u = pedge[v]++;
    if (color[u] == WHITE) {
      father[u] = v;
      color[u] = ODD;
      pedge[u] = 0;
      stack[top++] = u;
      instack[u] = true;
      if (match[u] == -1) {
        augment(s, stack, top, nn);
        return true;
      } else {
        stack[top++] = match[u];
        assert(color[match[u]] == WHITE);
        color[match[u]] = EVEN;
        father[match[u]] = u;
        pedge[match[u]] = 0;
      }
    } else {
      assert(color[u] == EVEN);
      if (instack[u]) {
        int len = 1;
#ifdef OUTPUT
        printf("Find backward odd cycle stem=%d new ind=%d\n", u, n);
#endif
        for (; len <= top && stack[top - len] != u; len++)
          ;
        assert(len <= top);
        assert(len & 1);
        top -= len;
        contract(stack + top, len, n);
      } else {
        // v must be a just contracted vertex vertex
        // The just contracted new vertex forms a odd cycle with some already
        // visited vertex
        /*
                    3---------------0
                    |               |
          1 ------------- 2
                    |               |
                    --------4--------

          match:  3--0  1--2
          path:   4--1--2--0--3
          backward odd cycle:  4--1--2
          forward odd cycle: 5(4,1,2)--0---3
         */
#ifdef OUTPUT
        printf("Find forward odd cycle stem=%d new ind=%d\n", u, n);
#endif
        assert(v >= nn);
        int ft = top;
        while (u != v) {
          stack[ft++] = u;
          u = getroot(father[getroot(u)]);
          assert(!removed[u]);
          assert(u != -1);
        }
        reverse(stack + top, stack + ft);
        top--;
        contract(stack + top, ft - top, n);
      }
      if (top > 0)
        father[n] = stack[top - 1];
      else
        father[n] = -1;
      stack[top++] = n;
      instack[n] = true;
      color[n] = EVEN;
      pedge[n] = 0;
      removed[n] = false;
      root[n] = -1;
      n++;
    }
#ifdef OUTPUT
    printf("current stack n=%d\n", n);
    for (int i = 0; i < top; i++)
      printf("%d ", stack[i]);
    puts("");
#endif
  }
  return false;
}
int myindex[N];
int generalMaximumMatching(int n, const vector<vector<int>> &adj_list) {
  for (int i = 0; i < 2 * n; i++)
    memset(edge[i], false, sizeof(bool) * 2 * n);
  for (int i = 0; i < n; i++)
    for (int j = 0; j < adj_list[i].size(); j++)
      edge[i][adj_list[i][j]] = true;
  for (int i = 0; i < n; i++)
    edge[i][i] = false;
  memset(match, 0xff, sizeof(int) * n);
  int re = 0;
  for (int i = 0; i < n; i++)
    myindex[i] = n - 1 - i;
  random_shuffle(myindex, myindex + n);
  for (int i = 0; i < n; i++)
    if (match[myindex[i]] == -1 && findPath(myindex[i], n))
      re++;
#ifdef OUTPUT
  for (int i = 0; i < n; i++)
    if (match[i] != -1)
      assert(match[match[i]] != -1 && match[match[i]] == i);
#endif
  return re;
}
#ifdef OUTPUT
int mask[30];
int table[1 << 22];
#define low(x) (((x - 1) ^ x) & x)
#define two(n) (1 << (n))
int bruteForce(int n) {
  for (int i = 0; i < n; i++) {
    mask[i] = 0;
    for (int j = 0; j < n; j++)
      if (generalMatching::edge[i][j])
        mask[i] ^= two(j);
  }
  for (int i = 1; i < two(n); i++) {
    int b = low(i);
    table[i] = table[i ^ b];
    for (int j = n - 1; two(j) > b; j--)
      if ((mask[j] & b) > 0 && (i & two(j)) > 0)
        table[i] = max(table[i], 1 + table[i ^ two(j) ^ b]);
  }
  printf("Brute-force %d\n", table[two(n) - 1]);
  return table[two(n) - 1];
}
#endif
}; // namespace generalMatching
