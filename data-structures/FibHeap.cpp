#include <algorithm>
#include <assert.h>
#include <stdio.h>
struct Hnode {
  int key, rank;
  bool mark;
  Hnode *son, *pre, *next, *pp;
  Hnode(int kk)
      : key(kk), rank(0), son(NULL), pre(NULL), next(NULL), pp(NULL),
        mark(false){};
  Hnode() : rank(0), son(NULL), pre(NULL), next(NULL), pp(NULL), mark(false){};
};
class FibHeap {
private:
  int kcmp(const Hnode *p1, const Hnode *p2) { return hcmp(p1->key, p2->key); }
  Hnode *root;
  int (*hcmp)(int, int);
  void doubleLink(Hnode *ph, Hnode *pn);
  void consolidate();
  Hnode *mergeTree(Hnode *r1, Hnode *r2);
  void cutHnode(Hnode *ph);

public:
  FibHeap(int (*cmp)(int, int)) : root(NULL), hcmp(cmp){};
  void insert(Hnode *ph);
  Hnode *extractMin();
  void decrease(Hnode *ph);
  int getMin() {
    assert(root != NULL);
    return root->key;
  };
  void merge(FibHeap &fh);
  bool empty() { return root == NULL; };
};

void FibHeap::doubleLink(Hnode *ph, Hnode *pn) {
  assert(ph);
  assert(pn);
  Hnode *next = ph->next;
  ph->next = pn;
  pn->pre = ph;
  pn->next = next;
  next->pre = pn;
}
void FibHeap::insert(Hnode *pn) {
  assert(pn);
  pn->pp = pn->son = NULL;
  pn->rank = 0;
  pn->mark = false;
  if (root == NULL) {
    root = pn;
    root->pre = root->next = root;
    return;
  }
  doubleLink(root, pn);
  if (kcmp(root, pn) > 0)
    root = pn;
}
void FibHeap::merge(FibHeap &fh) {
  if (root == NULL)
    root = fh.root;
  else if (fh.root) {
    Hnode *n1 = root->next, *n2 = fh.root->next;
    root->next = n2;
    n2->pre = root;
    fh.root->next = n1;
    n1->pre = fh.root;
    if (0 > kcmp(root, fh.root))
      root = fh.root;
  }
  fh.root = NULL;
}
void FibHeap::consolidate() {
  if (root == NULL)
    return;
  int sum = 1 << (root->rank);
  for (Hnode *p = root->next; p != root; p = p->next)
    sum += 1 << (p->rank);
  int len;
  for (len = 1; sum; sum >>= 1, len++)
    ;
  Hnode **bucket = new Hnode *[len + 1];
  memset(bucket, 0, sizeof(Hnode *) * (len + 1));
  assert(root->pre);
  root->pre->next = NULL;
  for (Hnode *p = root; p;) {
    Hnode *nr = p, *tmp = p->next;
    nr->pp = NULL, nr->mark = false;
    while (bucket[nr->rank])
      nr = mergeTree(nr, bucket[nr->rank]), bucket[nr->rank - 1] = NULL;
    bucket[nr->rank] = nr;
    p = tmp;
  }
  int i;
  for (i = 0; bucket[i] == NULL; i++)
    ;
  root = bucket[i];
  root->pre = root->next = root;
  for (i++; i <= len; i++) {
    if (bucket[i]) {
      doubleLink(root, bucket[i]);
      if (kcmp(root, bucket[i]) > 0)
        root = bucket[i];
    }
  }
  delete[] bucket;
}
Hnode *FibHeap::mergeTree(Hnode *r1, Hnode *r2) {
  assert(r1);
  assert(r2);
  assert(r1->rank == r2->rank);
  if (kcmp(r1, r2) > 0)
    std::swap(r1, r2);
  r1->rank++;
  if (r1->son == NULL)
    r1->son = r2, r2->pre = r2->next = r2;
  else
    doubleLink(r1->son, r2);
  r2->pp = r1;
  return r1;
}
void FibHeap::cutHnode(Hnode *ph) {
  assert(ph->pp);
  Hnode *pp = ph->pp;
  assert(pp->rank);
  if (pp->rank == 1) {
    pp->son = NULL;
  } else {
    if (pp->son == ph)
      pp->son = ph->next;
    assert(pp->son != ph);
    Hnode *pre = ph->pre;
    Hnode *next = ph->next;
    pre->next = next;
    next->pre = pre;
  }
  pp->rank--;
  ph->pp = NULL;
  ph->mark = false;
}
Hnode *FibHeap::extractMin() {
  assert(root);
  Hnode *res = root;
  if (root->son) {
    Hnode *root1 = root->son;
    Hnode *n1 = root->next, *n2 = root1->next;
    root->next = n2;
    n2->pre = root;
    root1->next = n1;
    n1->pre = root1;
  }
  if (root->next == root) {
    root = NULL;
    return res;
  }
  Hnode *pre = root->pre;
  Hnode *next = root->next;
  pre->next = next;
  next->pre = pre;
  root = pre;
  consolidate();
  return res;
}
void FibHeap::decrease(Hnode *ph) {
  assert(root != NULL);
  if (ph->pp == NULL) {
    if (0 < kcmp(root, ph))
      root = ph;
    return;
  }
  Hnode *pp = ph->pp;
  cutHnode(ph);
  doubleLink(root, ph);
  if (0 < kcmp(root, ph))
    root = ph;
  while (pp->mark) {
    ph = pp->pp;
    assert(ph);
    cutHnode(pp);
    doubleLink(root, pp);
    if (0 < kcmp(root, pp))
      root = pp;
    pp = ph;
  }
  assert(pp);
  if (pp->pp)
    pp->mark = true;
}