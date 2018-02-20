/*
 * Author: Hongcheng Zhu
 * If no define SIZE, it will take O(1) amortized time to insert and remove
 *
*/
#include <stdio.h>
#define NDEGUB
#include <assert.h>
//#define SIZE
template <typename Key> class default_cmp_t {
public:
  int operator()(const Key &a, const Key &b) {
    if (a != b)
      return a > b ? 1 : -1;
    return 0;
  }
};
template <typename Key, class Comp = default_cmp_t<Key>> class RBTree {
public:
  struct RBNode {
    Key key;
    RBNode *ll, *rr, *pp;
#ifdef SIZE
    int size;
#endif
    int bh;
    bool red;
    RBNode(const Key &kk) : key(kk){};
    RBNode(){};
  };

private:
  Comp kcmp;
  RBNode *root;
  RBNode *const null;
  static RBNode defaultNull;
  // static RBNode defaultNull;
  void leftRotate(RBNode *x);
  void rightRotate(RBNode *x);
  void insertFixup(RBNode *x);
  void removeFixup(RBNode *x);
  int nodeCmp(RBNode *a, RBNode *b) {
    return a == b ? 0 : kcmp(a->key, b->key);
  };
#define OUTPUT
#ifdef OUTPUT
  int DFS(RBNode *v, bool f);
#endif
public:
  RBTree() : null(&RBTree::defaultNull) {
    null->ll = null->rr = null;
    null->pp = null;
    null->bh = 1;
#ifdef SIZE
    null->size = 0;
#endif
    null->red = false;
    root = null;
  };
  void insert(RBNode *x);
  RBNode *find(Key k);
  void remove(RBNode *x);
  RBNode *successor(RBNode *x);
  RBNode *predecessor(RBNode *x);
  Key &getKey(RBNode *x) { return x->key; };
  void setKey(RBNode *x, Key v) { x->key = v; };
  RBNode *begin();
  RBNode *end();
#ifdef SIZE
  RBNode *kelement(int k);
  int size() { return root->size; };
#endif
  void preInsert(RBNode *const px, RBNode *y);
  void sucInsert(RBNode *const sx, RBNode *y);
  void clear() {
    root = null;
    null->pp = null;
  };
  void remove(RBNode *low, RBNode *up);
  RBNode *upperBound(Key k);
  RBNode *lowerBound(Key k);
  void join(RBTree &lt, RBNode *pm, RBTree &rt);
  void split(RBTree &lt, RBNode *pm, RBTree &rt);
  RBNode *getRoot(RBNode *p) {
    while (p->pp != null)
      p = p->pp;
    return p;
  };
  bool empty() { return root == null; };
  int count_not_greater_than(RBNode &node);
  void setRoot(RBNode *pr) {
    root = pr;
    root->pp = null;
    if (root->red)
      root->red = false, root->bh++;
  };
#ifdef OUTPUT
  void check() {
#ifdef SIZE
    assert(null->size == 0);
#endif
    assert(!null->red);
    assert(!root->red);
    assert(root->pp == null);
    DFS(root, false);
    RBNode *pre = begin();
    for (RBNode *p = begin(); p != NULL; p = successor(p))
      assert(nodeCmp(p, pre) >= 0), pre = p;
    pre = end();
    for (RBNode *p = end(); p != NULL; p = predecessor(p))
      assert(nodeCmp(p, pre) <= 0), pre = p;
  };
  void printTree();
#endif
};
template <typename Key, class Comp>
typename RBTree<Key, Comp>::RBNode RBTree<Key, Comp>::defaultNull;
#ifdef OUTPUT
template <typename Key, class Comp>
int RBTree<Key, Comp>::DFS(RBNode *v, bool f = false) {
  assert(v);
  if (v == null)
    return 1;
#ifdef SIZE
  if (f)
    printf("Node%d:key=%d ll=%d rr=%d pp=%d size=%d red=%s bh=%d\n", v - null,
           v->key, v->ll - null, v->rr - null, v->pp - null, v->size,
           v->red ? "red" : "black", v->bh);
#else
  if (f)
    printf("Node%d:key=%d ll=%d rr=%d pp=%d red=%s bh=%d\n", v - null, v->key,
           v->ll - null, v->rr - null, v->pp - null, v->red ? "red" : "black",
           v->bh);
#endif
  assert(v->ll == null || v->ll->pp == v);
  assert(v->rr == null || v->rr->pp == v);
  int h = DFS(v->ll, f);
  if (v->red)
    assert(!(v->ll->red) && !(v->rr->red));
  assert(h == DFS(v->rr, f));
  if (!(v->red))
    h++;
  assert(h == v->bh);
#ifdef SIZE
  assert(v->size == v->ll->size + v->rr->size + 1);
#endif
  return h;
};
template <typename Key, class Comp> void RBTree<Key, Comp>::printTree() {
  puts("----------------------");
  DFS(root, true);
}
#endif
template <typename Key, class Comp>
typename RBTree<Key, Comp>::RBNode *RBTree<Key, Comp>::lowerBound(Key k) {
  if (root == null)
    return NULL;
  RBNode *re = root;
  while (re != null && kcmp(re->key, k) < 0)
    re = re->rr;
  if (re == null)
    return end();
  RBNode *v = re->ll;
  while (v != null) {
    if (kcmp(v->key, k) < 0)
      v = v->rr;
    else
      re = v, v = v->ll;
  }
  assert(re && re != null && kcmp(re->key, k) >= 0);
  return re;
}
template <typename Key, class Comp>
typename RBTree<Key, Comp>::RBNode *RBTree<Key, Comp>::end() {
  if (root == null)
    return NULL;
  RBNode *re = root;
  while (re->rr != null)
    re = re->rr;
  return re;
}
template <typename Key, class Comp>
typename RBTree<Key, Comp>::RBNode *RBTree<Key, Comp>::begin() {
  if (root == null)
    return NULL;
  RBNode *re = root;
  while (re->ll != null)
    re = re->ll;
  return re;
}
#ifdef SIZE
template <typename Key, class Comp>
typename RBTree<Key, Comp>::RBNode *RBTree<Key, Comp>::kelement(int k) {
  RBNode *v = root;
  if (v->size < k)
    return NULL;
  for (;;) {
    assert(v != null && v);
    assert(k);
    if (v->ll->size + 1 == k)
      return v;
    if (v->ll->size >= k)
      v = v->ll;
    else
      k -= v->ll->size + 1, v = v->rr;
  }
  return 0;
}
template <typename Key, class Comp>
int RBTree<Key, Comp>::count_not_greater_than(RBNode &node) {
  int res = 0;
  RBNode *pnode = root;
  while (pnode != null) {
    if (nodeCmp(&node, pnode) >= 0) {
      res += 1 + pnode->ll->size;
      pnode = pnode->rr;
    } else {
      pnode = pnode->ll;
    }
  }
  return res;
}
#endif
template <typename Key, class Comp>
void RBTree<Key, Comp>::leftRotate(RBNode *x) {
  assert(x != null && x);
  RBNode *y = x->pp;
  RBNode *z = y->pp;
  assert(y != null && y->rr == x);
  y->rr = x->ll;
  y->rr->pp = y;
  x->ll = y;
  y->pp = x;
  x->pp = z;
  if (z == null)
    root = x;
  else if (z->ll == y)
    z->ll = x;
  else if (z->rr == y)
    z->rr = x;
  else
    assert(0);
#ifdef SIZE
  y->size = y->ll->size + y->rr->size + 1;
  x->size = x->ll->size + x->rr->size + 1;
#endif
}
template <typename Key, class Comp>
void RBTree<Key, Comp>::rightRotate(RBNode *x) {
  assert(x != null && x);
  RBNode *y = x->pp;
  RBNode *z = y->pp;
  assert(y != null && y->ll == x);
  y->ll = x->rr;
  y->ll->pp = y;
  x->rr = y;
  y->pp = x;
  x->pp = z;
  if (z == null)
    root = x;
  else if (z->ll == y)
    z->ll = x;
  else if (z->rr == y)
    z->rr = x;
  else
    assert(0);
#ifdef SIZE
  y->size = y->ll->size + y->rr->size + 1;
  x->size = x->ll->size + x->rr->size + 1;
#endif
}
template <typename Key, class Comp>
typename RBTree<Key, Comp>::RBNode *RBTree<Key, Comp>::find(Key kx) {
  RBNode *v = root;
  null->key = kx;
  for (;;) {
    int d = kcmp(kx, v->key);
    if (d == 0)
      break;
    v = (d < 0) ? v->ll : v->rr;
  }
  return v == null ? NULL : v;
}
template <typename Key, class Comp> void RBTree<Key, Comp>::insert(RBNode *z) {
  assert(z != root);
  z->ll = z->rr = z->pp = null;
#ifdef SIZE
  z->size = 1;
#endif
  if (root == null) {
    root = z;
    z->red = false;
    z->bh = 2;
    return;
  }
  RBNode *y = null, *x = root;
  while (x != null) {
    y = x;
#ifdef SIZE
    x->size++;
#endif
    x = (nodeCmp(z, x) < 0) ? x->ll : x->rr;
  }
  z->pp = y;
  if (nodeCmp(z, y) < 0)
    y->ll = z;
  else
    y->rr = z;
  z->red = true;
  z->bh = 1;
  insertFixup(z);
#ifdef SIZE
  null->size = 0;
#endif
  null->red = false;
}
template <typename Key, class Comp>
void RBTree<Key, Comp>::preInsert(RBNode *const x, RBNode *z) {
  assert(x != null && x);
  z->ll = z->rr = z->pp = null;
#ifdef SIZE
  z->size = z->bh = 1;
#endif
  z->red = true;
  if (x->ll != null) {
    RBNode *y = predecessor(x);
    assert(y && y != null);
    assert(y->rr == null);
    z->pp = y;
    y->rr = z;
  } else
    z->pp = x, x->ll = z;
#ifdef SIZE
  for (RBNode *p = z->pp; p != null; p = p->pp)
    p->size++;
#endif
  insertFixup(z);
}
template <typename Key, class Comp>
void RBTree<Key, Comp>::sucInsert(RBNode *const x, RBNode *z) {
  assert(x && x != null);
  z->ll = z->rr = z->pp = null;
#ifdef SIZE
  z->bh = z->size = 1;
#endif
  z->red = true;
  if (x->rr != null) {
    RBNode *y = successor(x);
    assert(y && y != null);
    assert(y->ll == null);
    z->pp = y;
    y->ll = z;
  } else
    z->pp = x, x->rr = z;
#ifdef SIZE
  for (RBNode *p = z->pp; p != null; p = p->pp)
    p->size++;
#endif
  insertFixup(z);
}
template <typename Key, class Comp>
void RBTree<Key, Comp>::insertFixup(RBNode *z) {
  while (z->pp->red) {
    assert(z != null);
    assert(z->pp != null);
    assert(z->pp->pp != null);
    if (z->pp->pp->ll == z->pp) {
      RBNode *y = z->pp->pp->rr;
      if (y->red) {
        y->pp->red = true;
        y->red = z->pp->red = false;
        z->pp->bh++;
        y->bh++;
        z = y->pp;
      } else {
        if (z->pp->rr == z)
          leftRotate(z), z = z->ll;
        assert(z->pp->pp->rr == y);
        assert(z == z->pp->ll);
        rightRotate(z->pp);
        assert(!z->pp->rr->red);
        z->pp->rr->red = true;
        z->pp->rr->bh--;
        z->pp->red = false;
        z->pp->bh++;
      }
    } else {
      RBNode *y = z->pp->pp->ll;
      if (y->red) {
        y->pp->red = true;
        y->red = z->pp->red = false;
        y->bh++;
        z->pp->bh++;
        z = y->pp;
      } else {
        if (z->pp->ll == z)
          rightRotate(z), z = z->rr;
        assert(z->pp->pp->ll == y);
        assert(z == z->pp->rr);
        leftRotate(z->pp);
        //        cerr<<z<<" "<<z->pp<<" "<<z->pp->ll<<" "<<root<<"
        //        "<<null<<endl;
        assert(!z->pp->ll->red);
        z->pp->ll->red = true;
        z->pp->ll->bh--;
        z->pp->red = false;
        z->pp->bh++;
      }
    }
  }
  root->red = false;
  root->bh = root->ll->bh + 1;
}
template <typename Key, class Comp> void RBTree<Key, Comp>::remove(RBNode *x) {
  assert(x && x != null);
  if (x->ll != null && x->rr != null) {
    RBNode *y = predecessor(x);
    assert(y && y != null && y->rr == null);
    RBNode *z = y->ll;
    z->pp = y->pp;
    if (z->pp->ll == y)
      z->pp->ll = z;
    else
      z->pp->rr = z;
#ifdef SIZE
    for (RBNode *p = y->pp; p != null; p = p->pp)
      p->size--;
#endif
    y->pp = x->pp;
    y->ll = x->ll;
    y->rr = x->rr;
#ifdef SIZE
    y->size = x->size;
#endif
    y->bh = x->bh;
    if (root == x)
      root = y;
    if (y->pp->ll == x)
      y->pp->ll = y;
    else
      y->pp->rr = y;
    y->ll->pp = y;
    y->rr->pp = y;
    if (!y->red) {
      y->red = x->red;
      if (z == null)
        assert(z->pp->rr != null || z->pp->ll != null);
      removeFixup(z);
    } else
      y->red = x->red;
  } else {
    RBNode *z = x->ll != null ? x->ll : x->rr;
    if (root == x)
      root = z;
    if (root == null)
      return;
#ifdef SIZE
    for (RBNode *p = x->pp; p != null; p = p->pp)
      p->size--;
#endif
    z->pp = x->pp;
    if (z->pp->ll == x)
      z->pp->ll = z;
    else
      z->pp->rr = z;
    if (!(x->red)) {
      if (z == null)
        assert(z->pp->ll != null || z->pp->rr != null);
      removeFixup(z);
    }
  }
}
template <typename Key, class Comp>
void RBTree<Key, Comp>::removeFixup(RBNode *x) {
  while (x != root && !x->red) {
    if (x->pp->ll == x) {
      RBNode *w = x->pp->rr;
      assert(w != null);
      if (w->red) {
        leftRotate(w);
        w->red = false;
        w->bh++;
        w = w->ll;
        w->red = true;
        w->bh--;
        w = w->rr;
      }
      assert(!w->red);
      if (!w->ll->red && !w->rr->red) {
        w->red = true;
        w->bh--;
        x = x->pp;
        x->bh--;
        continue;
      }
      if (!w->rr->red) {
        assert(w->ll->red);
        rightRotate(w->ll);
        w->red = true;
        w->bh--;
        w = w->pp;
        assert(w != null);
        w->red = false;
        w->bh++;
      }
      assert(w && w != null && w->rr->red && !w->red);
      leftRotate(w);
      w->red = w->ll->red;
      w->bh = w->ll->bh;
      w->ll->red = false;
      w->ll->bh = x->bh + 1;
      w->rr->red = false;
      w->rr->bh++;
      x = root;
    } else {
      RBNode *w = x->pp->ll;
      assert(w != null);
      if (w->red) {
        rightRotate(w);
        w->red = false;
        w->bh++;
        w = w->rr;
        w->red = true;
        w->bh--;
        w = w->ll;
      }
      assert(!w->red);
      if (!w->rr->red && !w->ll->red) {
        w->red = true;
        w->bh--;
        x = x->pp;
        x->bh--;
        continue;
      }
      if (!w->ll->red) {
        assert(w->rr->red);
        leftRotate(w->rr);
        w->red = true;
        w->bh--;
        w = w->pp;
        assert(w != null);
        w->red = false;
        w->bh++;
      }
      assert(w && w != null && w->ll->red && !w->red);
      rightRotate(w);
      w->red = w->rr->red;
      w->bh = w->rr->bh;
      w->rr->red = false;
      w->rr->bh = x->bh + 1;
      w->ll->red = false;
      w->ll->bh++;
      x = root;
    }
  }
  x->red = false;
  x->bh = x->ll->bh + 1;
}
template <typename Key, class Comp>
typename RBTree<Key, Comp>::RBNode *RBTree<Key, Comp>::predecessor(RBNode *x) {
  assert(x && x != null);
  if (x->ll == null && x != root) {
    while (x != root && x->pp->ll == x)
      x = x->pp;
    if (x == root)
      return NULL;
    return x->pp;
  }
  x = x->ll;
  if (x == null)
    return NULL;
  while (x->rr != null)
    x = x->rr;
  return x;
}
template <typename Key, class Comp>
typename RBTree<Key, Comp>::RBNode *RBTree<Key, Comp>::successor(RBNode *x) {
  assert(x && x != null);
  if (x->rr == null && x != root) {
    while (x != root && x->pp->rr == x)
      x = x->pp;
    if (x == root)
      return NULL;
    return x->pp;
  }
  x = x->rr;
  if (x == null)
    return NULL;
  while (x->ll != null)
    x = x->ll;
  return x;
}
template <typename Key, class Comp>
void RBTree<Key, Comp>::join(RBTree &lt, RBNode *pm, RBTree &rt) {
  if (rt.root == null) {
    root = lt.root;
    lt.root = null;
    insert(pm);
    return;
  }
  if (lt.root == null) {
    root = rt.root;
    rt.root = null;
    insert(pm);
    return;
  }
  assert(nodeCmp(pm, lt.end()) >= 0);
  assert(nodeCmp(rt.begin(), pm) >= 0);
  RBNode *r1 = lt.root, *r2 = rt.root;
  if (r1->bh > r2->bh) {
    RBNode *pp = r1;
    int h = r2->bh;
    assert(h >= 2);
    while (pp->bh > h)
      pp = pp->rr;
    assert(pp && pp != null);
    if (pp->red)
      pp = pp->rr;
    assert(!pp->red);
    pm->ll = pp;
    pm->rr = r2;
    pm->pp = pp->pp;
    pm->pp->rr = pm;
    pp->pp = r2->pp = pm;
    pm->red = true;
#ifdef SIZE
    pm->size = pp->size + r2->size + 1;
    for (RBNode *p = pm->pp; p != null; p = p->pp)
      p->size = 1 + p->ll->size + p->rr->size;
#endif
    pm->bh = h;
    root = r1;
    insertFixup(pm);
  } else if (r2->bh > r1->bh) {
    RBNode *pp = r2;
    int h = r1->bh;
    assert(h >= 2);
    while (pp->bh > h)
      pp = pp->ll;
    assert(pp && pp != null);
    if (pp->red)
      pp = pp->ll;
    assert(!pp->red);
    pm->ll = r1;
    pm->rr = pp;
    pm->pp = pp->pp;
    pm->pp->ll = pm;
    r1->pp = pp->pp = pm;
    pm->red = true;
#ifdef SIZE
    pm->size = pp->size + r1->size + 1;
    for (RBNode *p = pm->pp; p != null; p = p->pp)
      p->size = 1 + p->ll->size + p->rr->size;
#endif
    pm->bh = h;
    root = r2;
    insertFixup(pm);
  } else {
    pm->pp = null;
    pm->ll = r1;
    r1->pp = pm;
    pm->rr = r2;
    r2->pp = pm;
    pm->red = false;
#ifdef SIZE
    pm->size = r1->size + 1 + r2->size;
#endif
    pm->bh = r1->bh + 1;
    root = pm;
  }
  lt.root = rt.root = null;
}
template <typename Key, class Comp>
void RBTree<Key, Comp>::split(RBTree &lt, RBNode *pm, RBTree &rt) {
  assert(pm && pm != null);
  assert(getRoot(pm) == root);
  lt.setRoot(pm->ll);
  rt.setRoot(pm->rr);
  pm->ll = pm->rr = NULL;
  RBNode *ppm;
  int h = root->bh * 2;
  RBNode **lset = new RBNode *[h];
  RBNode **rset = new RBNode *[h];
  int ltop = 0, rtop = 0;
  while (pm != root) {
    ppm = pm->pp;
    if (ppm->rr == pm)
      lset[ltop++] = ppm;
    else
      rset[rtop++] = ppm;
    pm = ppm;
  }
  RBTree tmp(kcmp), tmp1(kcmp);
  for (int i = 0; i < ltop; i++) {
    tmp.root = lt.root;
    tmp1.setRoot(lset[i]->ll);
    lt.join(tmp1, lset[i], tmp);
  }
  for (int i = 0; i < rtop; i++) {
    tmp.root = rt.root;
    tmp1.setRoot(rset[i]->rr);
    rt.join(tmp, rset[i], tmp1);
  }
  delete[] lset;
  delete[] rset;
  root = null;
}

#include <cstdlib>
#include <set>
typedef RBTree<int>::RBNode node;
node pnode[1000000];
int main() {
  std::set<int> mem;
  for (int i = 0; i < 1000000; i++)
    mem.insert(rand());
  for (int i = 0; i < 10000000; i++)
    mem.find(rand());
  /*  RBTree<int> tree;
    for(int i=0;i<1000000;i++)
    {
      pnode[i].key=rand();
      tree.insert(pnode+i);
    }
    for(int i=0;i<10000000;i++)
    {
      tree.find(rand());
    }*/
  return 0;
}
