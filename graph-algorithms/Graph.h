#pragma once

#include <algorithm>
#include <limits>
#include <tuple>
#include <vector>

#include <glog/logging.h>

namespace graph {

template <typename Vint, class EdgeInfo> class DirectEdgeList {
public:
  explicit DirectEdgeList(size_t numEdges = 10) {
    edges_.reserve(numEdges + 1);
  }

  void addEdge(Vint a, Vint b, EdgeInfo info) {
    CHECK_LT(a, std::numeric_limits<Vint>::max());
    CHECK_LT(b, std::numeric_limits<Vint>::max());

    edges_.emplace_back();
    edges_.back().a = a;
    edges_.back().b = b;
    edges_.back().info = std::move(info);
    numVerts_ = std::max<size_t>(numVerts_, a + 1);
    numVerts_ = std::max<size_t>(numVerts_, b + 1);
  }

  void finalize() {
    LOG(INFO) << "There are " << numVerts_ << " nodes and " << edges_.size()
              << " directed edges.";
    std::sort(edges_.begin(), edges_.end(),
              [](const auto &lhs, const auto &rhs) {
                return std::tie(lhs.a, lhs.b) < std::tie(rhs.a, rhs.b);
              });
    edges_.emplace_back();
    start_.resize(numVerts_);
    size_t idx = 0;
    for (size_t v = 0; v < numVerts_; ++v) {
      while (edges_[idx].a < v) {
        ++idx;
      }
      start_[v] = idx;
    }
  }

  friend class Iterator;
  class Iterator {
  public:
    Iterator(Vint v, DirectEdgeList &edgeList) : v_(v), edgeList_(edgeList) {
      idx_ = edgeList_.start_[v];
    }
    std::pair<Vint, EdgeInfo &> operator*() {
      return {edgeList_.edges_[idx_].b, edgeList_.edges_[idx_].info};
    }

    EdgeInfo *operator->() { return &edgeList_.edges_[idx_].info; }

    bool valid() const { return edgeList_.edges_[idx_].a == v_; }

    bool operator==(const Iterator &rhs) const {
      return !valid() && !rhs.valid();
    }
    bool operator!=(const Iterator &rhs) const { return !(*this == rhs); }
    Iterator &operator++() {
      if (valid()) {
        ++idx_;
      }
      return *this;
    }

  private:
    const Vint v_;
    DirectEdgeList &edgeList_;
    size_t idx_ = 0;
  };

  Iterator edgeIterator(Vint v) { return Iterator(v, *this); }

  size_t numEdges() const { return edges_.size() - 1; }
  size_t numNodes() const { return numVerts_; }

private:
  struct Edge {
    Vint a = std::numeric_limits<Vint>::max();
    Vint b = std::numeric_limits<Vint>::max();
    EdgeInfo info;
  };

  std::vector<Edge> edges_;
  size_t numVerts_ = 0;
  // 'start_[i]' is the first index to 'edges_' where an edge has a
  // starting node 'i'.
  std::vector<size_t> start_;
};

}
