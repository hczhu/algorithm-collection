#include <assert.h>
#include <vector>
using namespace std;
vector<int> stable_marriage(const vector<vector<int>> &male,
                            const vector<vector<int>> &female) {
  const int n = male.size();
  assert(n == female.size());
  vector<vector<int>> frank(n, vector<int>(n + 1, n));
  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
      frank[i][female[i][j]] = j;
  vector<int> pm(n, -1), pf(n, n);
  bool f = true;
  while (f) {
    f = false;
    for (int i = 0; i < n; i++) {
      assert(pm[i] < n);
      if (-1 != pm[i] && i == pf[male[i][pm[i]]])
        continue;
      assert(1 + pm[i] < n);
      f = true;
      int w = male[i][++pm[i]];
      if (frank[w][i] < frank[w][pf[w]])
        pf[w] = i;
    }
  }
  for (int i = 0; i < n; i++) {
    assert(pm[i] < n);
    pm[i] = male[i][pm[i]];
    assert(pf[pm[i]] == i);
  }
  return pm;
}
