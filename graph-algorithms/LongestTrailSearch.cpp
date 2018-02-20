
#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <cmath>
#include <complex>
#include <condition_variable>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits.h>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <valarray>
#include <vector>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <gtest/gtest.h>

#include "Graph.h"

const std::vector<std::pair<int, int>> kEdges = {
    {0, 39},  {0, 43},  {0, 45},  {14, 45}, {15, 15}, {17, 3},  {18, 44},
    {19, 40}, {2, 17},  {2, 31},  {20, 16}, {20, 24}, {20, 37}, {21, 31},
    {22, 17}, {22, 48}, {23, 40}, {24, 14}, {26, 44}, {27, 8},  {29, 12},
    {29, 29}, {29, 44}, {3, 32},  {30, 24}, {30, 35}, {32, 41}, {33, 33},
    {34, 38}, {36, 31}, {38, 20}, {38, 23}, {39, 30}, {4, 30},  {40, 24},
    {40, 44}, {41, 35}, {41, 50}, {42, 7},  {44, 30}, {44, 5},  {45, 11},
    {45, 50}, {46, 17}, {46, 50}, {47, 11}, {47, 37}, {47, 45}, {49, 4},
    {49, 41}, {5, 31},  {5, 37},  {5, 5},   {50, 36}, {6, 14},  {6, 28},
    {9, 11},
};

struct Edge {
  int weight;
  int id;
};

using EL = graph::DirectEdgeList<int, Edge>;

int dfs(int v, std::vector<bool> &usedEdges, EL &el) {
  int res = 0;
  for (auto it = el.edgeIterator(v); it.valid(); ++it) {
    if (usedEdges[it->id]) {
      continue;
    }
    usedEdges[it->id] = true;
    res = std::max(res, it->weight + dfs((*it).first, usedEdges, el));
    usedEdges[it->id] = false;
  }
  return res;
}

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);

  EL el(kEdges.size());
  for (int i = 0; i < kEdges.size(); ++i) {
    const auto &pr = kEdges[i];
    Edge ed;
    ed.weight = pr.first + pr.second;
    ed.id = i;
    el.addEdge(pr.first, pr.second, ed);
    el.addEdge(pr.second, pr.first, ed);
  }
  el.finalize();

  for (auto it = el.edgeIterator(0); it.valid(); ++it) {
    LOG(INFO) << "0 ==> " << (*it).first << " " << it->weight << " " << it->id;
  }

  std::vector<bool> usedEdges(el.numEdges());
  std::cout << dfs(0, usedEdges, el) << std::endl;
  return 0;
}
