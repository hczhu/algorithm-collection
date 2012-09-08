// MMWC: minimum mean weight cycle
// dis[k][v]: the shortest path from a feak source node s to node v consisting of exactly k edges.
// MMWC = min  { max { (dis[n][v]-dis[k][v])/ (n-k) | k } | v  }
// return the cycle represented as a serie of nodes.
// To-do: find such a cycle!!!!
template<typename Len>
//vector<int> findMMWC(const vector<vector<pair<int,Len> > >& edge) {
double findMMWC(const vector<vector<pair<int,Len> > >& edge) {
  const int n=edge.size();
  const Len inf=numeric_limits<Len>::max();
  //debug(inf);
  vector<vector<Len> > dis(n+1,vector<Len>(n,inf));
  vector<vector<int> > pred(n+1,vector<int>(n));
  for(int v=0;v<n;v++) {
    dis[0][v]=0;
  }
  for(int k=0;k<n;k++) for(int v=0;v<n;v++) {
    const Len add=dis[k][v];
    if(add>inf/2)continue;
    for(int i=0;i<edge[v].size();i++) {
      int u=edge[v][i].first;
      Len t=edge[v][i].second+add;
      if(t<dis[k+1][u]) {
        dis[k+1][u]=t;
        pred[k+1][u]=v;
      }
    }
  }
  Len best_a=inf/(n+1),best_b=1;
  int best_v=-1,best_len=1;
  for(int v=0;v<n;v++) if(dis[n][v]<inf/2) {
    Len max_a=-1;
    int max_b=1;
    for(int k=0;k<n;k++) {
      if(dis[k][v]<inf/2&&max_a*(n-k)<(dis[n][v]-dis[k][v])*max_b) {
        max_a=dis[n][v]-dis[k][v];
        max_b=n-k;
      }
    }
    if(best_v==-1||best_a*max_b>max_a*best_b) {
      best_a=max_a;best_b=max_b;
      best_v=v;
      best_len=max_b;
    }
  }
  return 1.0*best_a/best_b;
}
