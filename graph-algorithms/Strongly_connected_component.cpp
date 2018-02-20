#include <algorithm>
#include <memory.h>
#include <stdio.h>
#include <vector>
#define NDEBUG
#include <assert.h>
using namespace std;
namespace SCC {
#define N 2000
#define M (N * N)
int begin[N], edge[M];
int etop[N];
int cnt[N + 1];
int aa[M], bb[M];
void forwardStar(int n, int m) {
  memset(cnt, 0, sizeof(int) * (n + 1));
  for (int i = 0; i < m; i++)
    cnt[1 + aa[i]]++;
  for (int i = 1; i <= n; i++)
    cnt[i] += cnt[i - 1];
  for (int i = 0; i < n; i++)
    begin[i] = cnt[i], etop[i] = cnt[i + 1];
  for (int i = 0; i < m; i++)
    edge[--cnt[aa[i] + 1]] = i;
}
// The SCCs are topologically sorted.
int stack[N], low[N];
int flag[N], buffer[N];
void stronglyConnectedComponents(int n, int m, int head[], int tail[],
                                 vector<vector<int>> &scc) {
  scc.clear();
  memcpy(aa, head, sizeof(int) * m);
  memcpy(bb, tail, sizeof(int) * m);
  forwardStar(n, m);
  memset(flag, 0xff, sizeof(int) * n);
  int ff = 0;
  for (int i = 0; i < n; i++) {
    if (flag[i] != -1)
      continue;
    int top = 0, btop = 0;
    buffer[btop++] = stack[top++] = i;
    low[i] = flag[i] = ff++;
    while (top) {
      int v = stack[--top], u;
      assert(v < n);
      while (begin[v] < etop[v] && flag[u = bb[edge[begin[v]]]] != -1) {
        begin[v]++;
        if (flag[u] != -2)
          low[v] = min(low[v], low[u]);
      }
      if (begin[v] == etop[v]) {
        //        printf("Pop stack %d low=%d flag=%d\n",v,low[v],flag[v]);
        if (top)
          low[stack[top - 1]] = min(low[stack[top - 1]], low[v]);
        if (low[v] == flag[v]) {
          vector<int> tmp;
#ifdef OUTPUT
          printf("Find SCC rooted at %d\n", v);
#endif
          do {
            assert(btop);
            btop--;
            tmp.push_back(buffer[btop]);
#ifdef OUTPUT
            printf("%d ", buffer[btop]);
#endif
            assert(buffer[btop] == v || low[buffer[btop]] < flag[buffer[btop]]);
            flag[buffer[btop]] = -2;
          } while (buffer[btop] != v);
          scc.push_back(tmp);
#ifdef OUTPUT
          puts("");
#endif
        }
      } else {
        stack[top++] = v;
        begin[v]++;
        assert(flag[u] == -1);
        low[u] = flag[u] = ff++;
        stack[top++] = u;
        buffer[btop++] = u;
        //        printf("Push stack %d\n",u);
      }
    }
  }
}
// Solve two SAT
// 2*i denotes i, while 2*i+1 denotes ~i
// aa[i]+bb[i] is a clause.
// sat[] return assignment
// m is the number of clauses and n is the number variants
// sat[i]==1 denotes i should be false
int head[M], tail[M];
bool twoSAT(int aa[], int bb[], int m, int n, int sat[]) {
  int top = 0;
  assert(2 * m <= M);
  for (int i = 0; i < m; i++) {
    head[top] = aa[i] ^ 1;
    tail[top++] = bb[i];
    head[top] = bb[i] ^ 1;
    tail[top++] = aa[i];
  }
  vector<vector<int>> scc;
  stronglyConnectedComponents(2 * n, top, head, tail, scc);
  memset(flag, 0xff, sizeof(int) * n);
  for (int i = 0; i < scc.size(); i++)
    for (int j = 0; j < scc[i].size(); j++) {
      if (flag[scc[i][j] >> 1] == i)
        return false;
      flag[scc[i][j] >> 1] = i;
    }
  for (int i = 0; i < scc.size(); i++)
    for (int j = 0; j < scc[i].size(); j++)
      flag[scc[i][j]] = i;
  memset(buffer, 0, sizeof(int) * n);
  for (int i = 0; i < scc.size(); i++) {
    int f = buffer[i];
    for (int j = 0; j < scc[i].size(); j++) {
      int v = scc[i][j];
      sat[v >> 1] = f ^ (v & 1);
      buffer[flag[v ^ 1]] = 1;
    }
  }
  return true;
}
}; // namespace SCC
