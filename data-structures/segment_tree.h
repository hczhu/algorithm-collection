#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <vector>
using namespace std;
// A generic data structure to represent a binary tree
// whoes nodes denote intervals each.
// Node_t::update(Node_t& a,Node_t& b) is called when an interval overlaps with
// the updated interval.
// Node_t::equal(Node_t& a) is called when the interval which is being queried
// contained
// the corresponding interval of a.
// Node_t::contained(Node_t& a) is called when the interval which is being
// queried is contained
// strictly by the interval of a.

namespace segment_tree {
class tree_node_t {
public:
  virtual void equal(tree_node_t &other, int low, int high){};
  virtual void contain(tree_node_t &other, int low, int high){};
  // Make it consistance between parents and sons.
  // Push down some values
  virtual void update(tree_node_t &ll, tree_node_t &rr, int low, int high){};
  virtual void update(tree_node_t &ll, tree_node_t &rr){};
  virtual void rewrite(tree_node_t &new_node, int low, int high){};
  virtual void print(int idx){};
  virtual ~tree_node_t(){};
};
template <class Node_t> class segment_tree_t {
  typedef Node_t tree_node_t;
  size_t _start, _n;
  vector<tree_node_t> _tree;

  static size_t get_start(size_t n) {
    size_t res = 1;
    while (res < n)
      res <<= 1;
    return res;
  }
  void query_helper(int start, int end, tree_node_t &result, int low, int high,
                    int idx) {
    while (low != start || high != end) {
      int ll = (idx << 1) + 1;
      int rr = ll + 1;
      _tree[idx].update(_tree[ll], _tree[rr], low, high);
      _tree[idx].contain(result, low, high);
      int mid = (low + high) >> 1;
      if (mid >= end) {
        high = mid;
        idx = ll;
      } else {
        if (start <= mid)
          query_helper(start, mid, result, low, mid, ll);
        low = mid + 1;
        start = max(start, mid + 1);
        idx = rr;
      }
    }
    assert(low == start && high == end);
    // printf("low=%d high=%d\n",low,high);
    _tree[idx].equal(result, low, high);
  }

public:
  void print() {
    for (int i = 0; i < _tree.size(); i++) {
      _tree[i].print(i);
    }
  }
  segment_tree_t(int n, tree_node_t default_node = tree_node_t()) {
    _start = 0;
    _n = n;
    int dep = 1;
    while (n > 1) {
      n = (n + 1) >> 1;
      dep++;
    }
    // debug(dep);
    _tree.resize((1 << dep) - 1, default_node);
  }
  void query(int start, int end, tree_node_t &result) {
    query_helper(start, end, result, 0, _n - 1, 0);
  }
  /*
   tree_node_t& getLeaf(int pos) {
     assert(pos<_n);
     return _tree[_start+pos];
   }*/
  void update(int leaf) {
    while (leaf) {
      leaf = (leaf - 1) >> 1;
      _tree[leaf].update(_tree[(leaf << 1) + 1], _tree[(leaf << 1) + 2]);
    }
  }

  void update(int start, int end, tree_node_t &new_node) {
    update_helper(start, end, 0, _n - 1, 0, new_node);
  }
  void update_helper(int start, int end, int low, int high, int idx,
                     tree_node_t &new_node) {
    if (start == low && end == high) {
      _tree[idx].rewrite(new_node, low, high);
      return;
    }
    assert(low <= start && high >= end);
    int mid = (low + high) >> 1;
    int ll = (idx << 1) + 1;
    int rr = ll + 1;
    _tree[idx].update(_tree[ll], _tree[rr], low, high);
    if (start <= mid)
      update_helper(start, min(mid, end), low, mid, ll, new_node);
    if (end > mid)
      update_helper(max(start, mid + 1), end, mid + 1, high, rr, new_node);
    _tree[idx].update(_tree[ll], _tree[rr], low, high);
  };
};
template <typename T> struct sum_node_t : tree_node_t {
  bool covered;
  T s, v, add;
  sum_node_t() : covered(false), s(T()), v(T()), add(T()){};
  virtual void equal(sum_node_t &sum, int low, int high) {
    sum.s += (covered ? (high + 1 - low) * v : s) + (high + 1 - low) * add;
    // cerr<<low<<" "<<high<<"="<<sum.s<<endl;
  }
  T getSum(int low, int high) {
    return (covered ? (high + 1 - low) * v : s) + (high + 1 - low) * add;
  }
  virtual void update(sum_node_t &a, sum_node_t &b, int low, int high) {
    if (covered) {
      covered = false;
      v += add;
      add = 0;
      a.v = b.v = v;
      a.covered = b.covered = true;
      a.add = b.add = 0;
    } else if (add) {
      a.add += add;
      b.add += add;
      add = 0;
    }
    int mid = (low + high) >> 1;
    s = a.getSum(low, mid) + b.getSum(mid + 1, high);
  }
  virtual void rewrite(sum_node_t &temp, int low, int high) {
    if (temp.covered) {
      covered = true;
      v = temp.v;
      add = 0;
    } else {
      add += temp.add;
    }
  }
  virtual void print(int idx) {
    cerr << "node #" << idx << "=covered:" << covered << " s:" << s
         << " add:" << add << endl;
  }
};
};
