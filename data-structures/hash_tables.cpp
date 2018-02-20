#include <algorithm>
#include <cstdio>
#include <new>
using namespace std;
template <typename Key> class simple_hashfunc_t {
public:
  unsigned int operator()(const Key key) { return (unsigned int)key; };
};
template <typename Key, typename Value,
          typename Hashfunc = simple_hashfunc_t<Key>>
class hashtable_t {
  typedef unsigned int uint;
  struct Node {
    Key key;
    uint next;
    Value value;
  };
  Node *table;
  Node dummy[3];
  int _size, _node, _invalid_key, _top;
  Hashfunc hashf;

public:
  hashtable_t() : table(dummy), _size(2), _node(0){};
  int create(int size, const Key &invalid, bool init = false) {
    try {
      table = new Node[size + 1];
    } catch (std::bad_alloc) {
      return -1;
    }
    _invalid_key = invalid;
    _size = size;
    _node = 0;
    _top = 1;
    if (init)
      for (int i = 0; i <= size; i++)
        table[i].key = _invalid_key;
    return 0;
  };
  Value *find(const Key &key) {
    uint pos = 1 + (hashf(key) % (_size - 1));
    table[0].key = key;
    while (table[pos].key != key)
      pos = table[pos].next;
    if (pos == 0)
      return NULL;
    return &(table[pos].value);
  };
  char *monitor() {
    static char buffer[1000];
    double sum = 0.0;
    int fill = 0;
    int max_step = 0;
    for (int i = 1; i < _size; i++) {
      if (table[i].key == _invalid_key)
        continue;
      fill++;
      Key key = table[i].key;
      uint pos = 1 + (hashf(key) % (_size - 1));
      int step = 1;
      while (table[pos].key != key)
        step++, pos = table[pos].next;
      sum += step;
      max_step = max(max_step, step);
    }
    snprintf(buffer, sizeof(buffer), "max access time: %d\naverage access "
                                     "time: %lf\nnode number: %d\nload factor "
                                     "%lf\n",
             max_step, 1.0 * sum / _node, _node, 1.0 * fill / _size);
    return buffer;
  };
  bool insert(const Key &key, Value value) {
    uint pos = 1 + (hashf(key) % (_size - 1));
    if (table[pos].key == _invalid_key) {
      table[pos].key = key;
      table[pos].value = value;
      _node++;
      return true;
    }
    while (table[_top].key != _invalid_key)
      _top++;
    if (_top == _size)
      return false;
    table[_top].key = key;
    table[_top].value = value;
    table[_top].next = table[pos].next;
    table[pos].next = _top;
    _top++;
    _node++;
    return true;
  };
  int size() { return _node; };
  void clear() {
    for (int i = 0; i <= _size; i++)
      table[i].key = _invalid_key;
    _size = 0;
  };
};
