/*
 * zhuhongcheng@baidu.com
 * November 11, 2009 (Single men's festival)
 *
 */
#ifndef __LINKED_LIST_HEADER__
#define __LINKED_LIST_HEADER__

#include "data_structures_common.h"
#include "hash_table.h"
#include "im_vector.h"
#include "mem_pool.h"
#include <algorithm>
#include <cassert>
// using namespace std;
//�ı��һ������ָ��Ľڵ��pointer
#define SET_POINTER(vptr, prev_or_next, new_value)                             \
  {                                                                            \
    double_pointer_t *tmp_pointer = get_pointer_func((vptr));                  \
    CHECK_NULL_POINTER(tmp_pointer, -1);                                       \
    tmp_pointer->prev_or_next = (new_value);                                   \
  }
//�ı��һ����������
#define GET_POINTER(dest, vptr, prev_or_next)                                  \
  {                                                                            \
    double_pointer_t *tmp_pointer = get_pointer_func(vptr);                    \
    CHECK_NULL_POINTER(tmp_pointer, -1);                                       \
    (dest) = tmp_pointer->prev_or_next;                                        \
  }
struct double_pointer_t {
  Vaddress32 next, prev;
  double_pointer_t() : next(NULL_VADDRESS32), prev(NULL_VADDRESS32){};
  double_pointer_t(Vaddress32 a, Vaddress32 b) : next(a), prev(b){};
};
template <class Get_list_head_t, class Get_pointer_t>
class doubly_linked_list_unsorted_t {
protected:
  Get_list_head_t get_list_head_func;
  Get_pointer_t get_pointer_func;

public:
  //ע��һд���
  //�ڵ㱻�ӵ�ͷ��
  int add_node(Vaddress32 vptr_list, Vaddress32 vptr_node) {
    double_pointer_t *dpointer = get_pointer_func(vptr_node);
    CHECK_NULL_POINTER(dpointer, -1);
    Vaddress32 *m_head = get_list_head_func(vptr_list);
    CHECK_NULL_POINTER(m_head, -1);
    //�ǿ�����
    if (NULL_VADDRESS32 != *m_head) {
      double_pointer_t *next_dpointer = get_pointer_func(*m_head);
      CHECK_NULL_POINTER(next_dpointer, -1);
      dpointer->prev = next_dpointer->prev;
      next_dpointer->prev = vptr_node;
    }
    //������
    else {
      dpointer->prev =
          combine_vaddr(INVALID_VADDR_TYPE, get_vaddr_offset(vptr_list));
    }
    dpointer->next = *m_head;
    *m_head = vptr_node;
    return 0;
  };
  //ע��һд���
  // v_list_head�Ǵ������������ɾ���ڵ�󣬽ڵ�ԭ�����ڵ������ɿ�������ò���Ϊ����ͷ�ڵ�����ַ
  //��������£��ô���������ֵΪNULL_VADDRESS32;
  int remove_node(Vaddress32 vptr_node, Vaddress32 &v_list_head_out) {
    v_list_head_out = NULL_VADDRESS32;
    const double_pointer_t *dpointer = get_pointer_func(vptr_node);
    CHECK_NULL_POINTER(dpointer, -1);
    if (dpointer->prev == NULL_VADDRESS32) {
      ul_writelog(UL_LOG_WARNING,
                  "The node [%u] is not in any doubly_linked_list_unsorted_t",
                  vptr_node);
      return 0;
    }
    //�������׽ڵ�
    if (get_vaddr_type(dpointer->prev) != INVALID_VADDR_TYPE) {
      //���ں�̽ڵ�
      if (NULL_VADDRESS32 != dpointer->next) {
        SET_POINTER(dpointer->next, prev, dpointer->prev);
      }
      //��remove_node��������ȥ��
      SET_POINTER(dpointer->prev, next, dpointer->next);
    }
    //�����׽ڵ�
    else {
      assert(get_vaddr_type(dpointer->prev) == INVALID_VADDR_TYPE);
      const Vaddress32 header_vaddr = get_vaddr_offset(dpointer->prev);
      Vaddress32 *m_head = get_list_head_func((header_vaddr));
      *m_head = dpointer->next;
      //���ں�̽ڵ�
      if (NULL_VADDRESS32 != *m_head) {
        SET_POINTER(*m_head, prev, dpointer->prev);
      }
      //��Ϊ������
      else {
        v_list_head_out = *m_head;
      }
    }
    return 0;
  };
  int remove_node(Vaddress32 vptr_node) {
    Vaddress32 v_list_head_out = NULL_VADDRESS32;
    return remove_node(vptr_node, v_list_head_out);
  };
  bool is_empty(Vaddress32 m_head) {
    return get_vaddr_type(m_head) == INVALID_VADDR_TYPE;
  };
  Vaddress32 get_next(Vaddress32 vptr) {
    if (vptr == NULL_VADDRESS32) {
      return NULL_VADDRESS32;
    }
    double_pointer_t *pointer = get_pointer_func(vptr);
    CHECK_NULL_POINTER(pointer, NULL_VADDRESS32);
    return pointer->next;
  };
  Vaddress32 get_prev(Vaddress32 vptr) {
    if (vptr == NULL_VADDRESS32) {
      return NULL_VADDRESS32;
    }
    double_pointer_t *pointer = get_pointer_func(vptr);
    CHECK_NULL_POINTER(pointer, NULL_VADDRESS32);
    Vaddress32 ret = pointer->prev;
    //��ǰ�ڵ���ͷ�ڵ�
    if (get_vaddr_type(ret) == INVALID_VADDR_TYPE) {
      ret = NULL_VADDRESS32;
    }
    return ret;
  };
};

//***********************************************************************************
// comparator(a,b)<0  ��ڵ�a���ڽڵ�bǰ�档
template <class Get_list_head_t, class Get_pointer_t, class Node_comparator_t>
class doubly_linked_list_sorted_t
    : public doubly_linked_list_unsorted_t<Get_list_head_t, Get_pointer_t> {
  Node_comparator_t node_comparator;
  typedef doubly_linked_list_unsorted_t<Get_list_head_t, Get_pointer_t> Base_t;
  using doubly_linked_list_unsorted_t<Get_list_head_t,
                                      Get_pointer_t>::get_list_head_func;
  using doubly_linked_list_unsorted_t<Get_list_head_t,
                                      Get_pointer_t>::get_pointer_func;
  int insert_node_after(Vaddress32 v_pre_node, Vaddress32 v_add_node) {
    //���αȽϽڵ�
    double_pointer_t *p_add_node_pointer = get_pointer_func(v_add_node);
    CHECK_NULL_POINTER(p_add_node_pointer, -1);
    Vaddress32 v_next_node = NULL_VADDRESS32;
    assert(node_comparator(v_add_node, v_pre_node) >= 0);
    for (;;) {
      GET_POINTER(v_next_node, v_pre_node, next);
      if (NULL_VADDRESS32 == v_next_node ||
          node_comparator(v_add_node, v_next_node) <= 0) {
        break;
      }
      v_pre_node = v_next_node;
    }
    assert(NULL_VADDRESS32 != v_pre_node);
    p_add_node_pointer->next = v_next_node;
    p_add_node_pointer->prev = v_pre_node;
    if (NULL_VADDRESS32 != v_next_node) {
      SET_POINTER(v_next_node, prev, v_add_node);
    }
    SET_POINTER(v_pre_node, next, v_add_node);
    return 0;
  }

public:
  //����debug
  bool check_sorted_list(Vaddress32 v_list_head) {
    if (NULL_VADDRESS32 == v_list_head) {
      return true;
    }
    Vaddress32 v_next_node = NULL_VADDRESS32;
    for (;;) {
      GET_POINTER(v_next_node, v_list_head, next);
      if (NULL_VADDRESS32 == v_next_node) {
        break;
      }
      if (node_comparator(v_list_head, v_next_node) > 0) {
        return false;
      }
      v_list_head = v_next_node;
    }
    return true;
  }
  //��������v_list_head_outͬremove_node
  int remove_sorted_node(Vaddress32 v_remove_node,
                         Vaddress32 &v_list_head_out) {
    return Base_t::remove_node(v_remove_node, v_list_head_out);
  }
  int remove_sorted_node(Vaddress32 v_remove_node) {
    return Base_t::remove_node(v_remove_node);
  }
  int add_sorted_node(const Vaddress32 vptr_list, const Vaddress32 vptr_node) {
    double_pointer_t *p_add_node_pointer = get_pointer_func(vptr_node);
    CHECK_NULL_POINTER(p_add_node_pointer, -1);
    Vaddress32 *p_list_head = Base_t::get_list_head_func(vptr_list);
    CHECK_NULL_POINTER(p_list_head, -1);

    //ע��һд���
    //������
    if (NULL_VADDRESS32 == *p_list_head) {
      p_add_node_pointer->next = NULL_VADDRESS32;
      p_add_node_pointer->prev =
          combine_vaddr(INVALID_VADDR_TYPE, get_vaddr_offset(vptr_list));
      *p_list_head = vptr_node;
      return 0;
    }
    //��ͷ�ڵ㻹С
    if (node_comparator(vptr_node, *p_list_head) <= 0) {
      double_pointer_t *p_head_pointer = Base_t::get_pointer_func(*p_list_head);
      CHECK_NULL_POINTER(p_head_pointer, -1);
      p_add_node_pointer->next = *p_list_head;
      p_add_node_pointer->prev = p_head_pointer->prev;
      p_head_pointer->prev = vptr_node;
      *p_list_head = vptr_node;
      return 0;
    }
    return insert_node_after(*p_list_head, vptr_node);
  };
  int change_sorted_node(Vaddress32 v_old_node, Vaddress32 v_new_node) {
    Vaddress32 v_pre_node;
    GET_POINTER(v_pre_node, v_old_node, prev);
    //��Ҫ �ı�Ľڵ��������׽ڵ�
    if (INVALID_VADDR_TYPE == get_vaddr_type(v_pre_node)) {
      Vaddress32 v_list_head = Base_t::get_list_head_func(v_pre_node);
      if (remove_sorted_node(v_old_node) ||
          add_sorted_node(v_list_head, v_new_node)) {
        return -1;
      }
      return 0;
    }

    double_pointer_t *p_new_pointer = Base_t::get_pointer_func(v_new_node);
    CHECK_NULL_POINTER(p_new_pointer, -1);
    //�ڵ���������δ�仯

    if (0 == node_comparator(v_old_node, v_new_node)) {
      // copy on write
      GET_POINTER(p_new_pointer->prev, v_old_node, prev);
      GET_POINTER(p_new_pointer->next, v_old_node, next);
      assert(NULL_VADDRESS32 != p_new_pointer->prev);
      CHECK_NULL_VADDRESS32(p_new_pointer->prev, -1);
      if (NULL_VADDRESS32 != p_new_pointer->next) {
        SET_POINTER(p_new_pointer->next, prev, v_new_node);
      }
      SET_POINTER(p_new_pointer->prev, next, v_new_node);
      return 0;
    }
    v_pre_node = p_new_pointer->prev;
    CHECK_NULL_VADDRESS32(v_pre_node, -1);
    if (remove_sorted_node(v_old_node)) {
      return -1;
    }
    //����ǰ�ƶ�
    while (get_vaddr_type(v_pre_node) != INVALID_VADDR_TYPE &&
           node_comparator(v_pre_node, v_new_node) > 0) {
      GET_POINTER(v_pre_node, v_pre_node, prev);
    }
    //��������ͷ
    if (get_vaddr_type(v_pre_node) == INVALID_VADDR_TYPE) {
      return add_sorted_node(get_vaddr_offset(v_pre_node), v_new_node);
    }
    // v_pre_node��������ͷ
    return insert_node_after(v_pre_node, v_new_node);
  };
};

//**************************************************************************
// �����ǵ������

namespace Doubly_linked_list_unsorted_tester {
struct node_t {
  signature64_t sig64;
  double_pointer_t pointer;
  uint32_t value;
};
struct list_t {
  uint32_t key;
  Vaddress32 list;
};

hashtable_t<node_t, signature64_t> hash_node;
hashtable_t<list_t, uint32_t> hash_list;

struct get_pointer_t {
  double_pointer_t *operator()(Vaddress32 vptr) {
    node_t *node = hash_node.get_usr_data(vptr);
    CHECK_NULL_POINTER(node, NULL);
    return &(node->pointer);
  };
};
struct get_list_head_t {
  Vaddress32 *operator()(Vaddress32 vptr) {
    list_t *list = hash_list.get_usr_data(vptr);
    CHECK_NULL_POINTER(list, NULL);
    return &(list->list);
  };
};

void doubly_linked_list_unsorted_tester() {
  srand(time(NULL));
  debug("Unit test for doubly_linked_list_unsorted_t starts");
  doubly_linked_list_unsorted_t<get_list_head_t, get_pointer_t>
      doubly_linked_list_unsorted;
  debug(sizeof(doubly_linked_list_unsorted));
  const int N = 10000000;
  const int M = 1000;
  int res;
  Im_mem_pool::mem_pool_t mem_pool;
  res = mem_pool.create();
  assert(0 == res);
  res = hash_node.create(std::max(10, N / 10), mem_pool);
  assert(0 == res);
  res = hash_list.create(std::max(10, N / 10), mem_pool);
  assert(0 == res);
  std::map<int, int> mem;
  node_t node;
  list_t list;
  int node_num = 0;
  for (int i = 0; i < N; i++) {
    node.sig64.sig1 = rand() % M;
    node.sig64.sig2 = rand();
    node.value = node.sig64.sig1 ^ node.sig64.sig2;
    if (NULL != hash_node.seek_node(node.sig64))
      continue;
    Vaddress32 vptr = hash_node.add_node_vaddr(node.sig64, node, false);
    assert(vptr != NULL_VADDRESS32);
    node_num++;
    list.key = node.sig64.sig1;
    Vaddress32 vlist = hash_list.seek_node_vaddr(list.key);
    if (vlist == NULL_VADDRESS32) {
      vlist = hash_list.add_node_vaddr(list, false);
      assert(vlist);
      hash_list.get_usr_data(vlist)->list = NULL_VADDRESS32;
    }
    assert(NULL_VADDRESS32 != vlist);
    doubly_linked_list_unsorted.add_node(vlist, vptr);
    mem[node.sig64.sig1]++;
    //  std::cerr<<"Add node:"<<node.sig64.sig1<<"
    //  "<<node.sig64.sig2<<std::endl;
  }
  debug(node_num);
  for (hashtable_t<node_t, signature64_t>::iterator_t itr = hash_node.begin();
       itr != hash_node.end(); itr++) {
    if (1 & rand())
      continue;
    //  std::cerr<<"Remove node from list:"<<(*itr).sig64.sig1<<"
    //  "<<(*itr).sig64.sig2<<std::endl;
    Vaddress32 vptr = hash_node.seek_node_vaddr((*itr)->sig64);
    assert(vptr != NULL_VADDRESS32);
    int res = doubly_linked_list_unsorted.remove_node(vptr);
    assert(res == 0);
    (*itr)->pointer.prev = NULL_VADDRESS32;
    (*itr)->pointer.next = NULL_VADDRESS32;
    //    res=doubly_linked_list_unsorted.remove_node(vptr,hash_list,hash_node);
    //    assert(res==-2);
    mem[(*itr)->sig64.sig1]--;
  }
  for (hashtable_t<list_t, uint32_t>::iterator_t itr = hash_list.begin();
       itr != hash_list.end(); itr++) {
    uint32_t key = (*itr)->key;
    Vaddress32 list = (*itr)->list;
    int cnt = 0;
    if (doubly_linked_list_unsorted.is_empty(list))
      continue;
    while (list != NULL_VADDRESS32) {
      node_t *pnode = hash_node.get_usr_data(list);
      assert(pnode != NULL);
      assert(pnode->sig64.sig1 == key);
      assert(pnode->value == (pnode->sig64.sig1 ^ pnode->sig64.sig2));
      cnt++;
      list = doubly_linked_list_unsorted.get_next(list);
    }
    assert(mem[(*itr)->key] == cnt);
  }
  char buf[1000];
  hash_list.get_detect_info(buf, sizeof(buf));
  printf("%s\n", buf);

  hash_node.get_detect_info(buf, sizeof(buf));
  printf("%s\n", buf);
  debug("Unit test for doubly_linked_list_unsorted_t finished successfully");

  for (hashtable_t<node_t, signature64_t>::iterator_t itr = hash_node.begin();
       itr != hash_node.end(); itr++) {
    //  std::cerr<<"Remove node from list:"<<(*itr).sig64.sig1<<"
    //  "<<(*itr).sig64.sig2<<std::endl;
    Vaddress32 vptr = hash_node.seek_node_vaddr((*itr)->sig64);
    if ((*itr)->pointer.prev == NULL_VADDRESS32)
      continue;
    assert(vptr != NULL_VADDRESS32);
    int res = doubly_linked_list_unsorted.remove_node(vptr);
    assert(res == 0);
    (*itr)->pointer.prev = NULL_VADDRESS32;
    (*itr)->pointer.next = NULL_VADDRESS32;
    mem[(*itr)->sig64.sig1]--;
  }
  for (hashtable_t<list_t, uint32_t>::iterator_t itr = hash_list.begin();
       itr != hash_list.end(); itr++) {
    uint32_t key = (*itr)->key;
    Vaddress32 list = (*itr)->list;
    int cnt = 0;
    if (doubly_linked_list_unsorted.is_empty(list))
      continue;
    while (list != NULL_VADDRESS32) {
      node_t *pnode = hash_node.get_usr_data(list);
      assert(pnode != NULL);
      assert(pnode->sig64.sig1 == key);
      assert(pnode->value == (pnode->sig64.sig1 ^ pnode->sig64.sig2));
      cnt++;
      list = doubly_linked_list_unsorted.get_next(list);
    }
    assert(mem[(*itr)->key] == cnt);
  }
}

Func_tester(doubly_linked_list_unsorted_tester);
};

namespace Doubly_linked_list_sorted_tester {
struct node_t {
  signature64_t sig64;
  double_pointer_t pointer;
  uint32_t value;
};

struct list_t {
  uint32_t key;
  Vaddress32 list;
};

hashtable_t<node_t, signature64_t> hash_node;
hashtable_t<list_t, uint32_t> hash_list;

struct get_pointer_t {
  double_pointer_t *operator()(Vaddress32 vptr) {
    node_t *node = hash_node.get_usr_data(vptr);
    CHECK_NULL_POINTER(node, NULL);
    return &(node->pointer);
  };
};
struct get_list_head_t {
  Vaddress32 *operator()(Vaddress32 vptr) {
    list_t *list = hash_list.get_usr_data(vptr);
    CHECK_NULL_POINTER(list, NULL);
    return &(list->list);
  };
};
struct node_comparator_t {
  int operator()(Vaddress32 v_node1, Vaddress32 v_node2) {
    node_t *p_node1 = hash_node.get_usr_data(v_node1);
    CHECK_NULL_POINTER(p_node1, 0);
    node_t *p_node2 = hash_node.get_usr_data(v_node2);
    CHECK_NULL_POINTER(p_node2, 0);
    if (p_node1->value == p_node2->value)
      return 0;
    return p_node1->value > p_node2->value ? 1 : -1;
  };
};
void doubly_linked_list_sorted_tester() {
  srand(time(NULL));
  debug("Unit test for doubly_linked_list_sorted_t starts");
  doubly_linked_list_sorted_t<get_list_head_t, get_pointer_t, node_comparator_t>
      doubly_linked_list_sorted;
  debug(sizeof(doubly_linked_list_sorted));
  const int N = 1000000;
  const int M = 1000;
  int res;
  Im_mem_pool::mem_pool_t mem_pool;
  res = mem_pool.create();
  assert(0 == res);
  res = hash_node.create(std::max(10, N / 10), mem_pool);
  assert(0 == res);
  res = hash_list.create(std::max(10, N / 10), mem_pool);
  assert(0 == res);
  std::map<int, int> mem;
  node_t node;
  list_t list;
  int node_num = 0;
  for (int i = 0; i < N; i++) {
    node.sig64.sig1 = rand() % M;
    node.sig64.sig2 = rand();
    node.value = node.sig64.sig1 ^ node.sig64.sig2;
    if (NULL != hash_node.seek_node(node.sig64))
      continue;
    Vaddress32 vptr = hash_node.add_node_vaddr(node.sig64, node, false);
    assert(vptr != NULL_VADDRESS32);
    node_num++;
    list.key = node.sig64.sig1;
    Vaddress32 vlist = hash_list.seek_node_vaddr(list.key);
    if (vlist == NULL_VADDRESS32) {
      vlist = hash_list.add_node_vaddr(list, false);
      assert(vlist);
      hash_list.get_usr_data(vlist)->list = NULL_VADDRESS32;
    }
    assert(NULL_VADDRESS32 != vlist);
    doubly_linked_list_sorted.add_sorted_node(vlist, vptr);
    mem[node.sig64.sig1]++;
  }
  for (hashtable_t<list_t, uint32_t>::iterator_t itr = hash_list.begin();
       itr != hash_list.end(); itr++) {
    Vaddress32 list = (*itr)->list;
    assert(doubly_linked_list_sorted.check_sorted_list(list));
  }
  debug(node_num);
  for (hashtable_t<node_t, signature64_t>::iterator_t itr = hash_node.begin();
       itr != hash_node.end(); itr++) {
    if (1 & rand())
      continue;
    //  std::cerr<<"Remove node from list:"<<(*itr).sig64.sig1<<"
    //  "<<(*itr).sig64.sig2<<std::endl;
    Vaddress32 vptr = hash_node.seek_node_vaddr((*itr)->sig64);
    assert(vptr != NULL_VADDRESS32);
    int res = doubly_linked_list_sorted.remove_sorted_node(vptr);
    assert(res == 0);
    (*itr)->pointer.prev = NULL_VADDRESS32;
    (*itr)->pointer.next = NULL_VADDRESS32;
    //    res=doubly_linked_list_unsorted.remove_node(vptr,hash_list,hash_node);
    //    assert(res==-2);
    mem[(*itr)->sig64.sig1]--;
    node_num--;
  }
  debug(node_num);
  for (hashtable_t<list_t, uint32_t>::iterator_t itr = hash_list.begin();
       itr != hash_list.end(); itr++) {
    uint32_t key = (*itr)->key;
    Vaddress32 list = (*itr)->list;
    assert(doubly_linked_list_sorted.check_sorted_list(list));
    int cnt = 0;
    while (list != NULL_VADDRESS32) {
      node_t *pnode = hash_node.get_usr_data(list);
      assert(pnode != NULL);
      assert(pnode->sig64.sig1 == key);
      assert(pnode->value == (pnode->sig64.sig1 ^ pnode->sig64.sig2));
      cnt++;
      list = doubly_linked_list_sorted.get_next(list);
    }
    assert(mem[(*itr)->key] == cnt);
  }
  char buf[1000];
  hash_list.get_detect_info(buf, sizeof(buf));
  printf("%s\n", buf);

  hash_node.get_detect_info(buf, sizeof(buf));
  printf("%s\n", buf);

  for (hashtable_t<node_t, signature64_t>::iterator_t itr = hash_node.begin();
       itr != hash_node.end(); itr++) {
    //  std::cerr<<"Remove node from list:"<<(*itr).sig64.sig1<<"
    //  "<<(*itr).sig64.sig2<<std::endl;
    Vaddress32 vptr = hash_node.seek_node_vaddr((*itr)->sig64);
    if ((*itr)->pointer.prev == NULL_VADDRESS32)
      continue;
    assert(vptr != NULL_VADDRESS32);
    int res = doubly_linked_list_sorted.remove_node(vptr);
    assert(res == 0);
    (*itr)->pointer.prev = NULL_VADDRESS32;
    (*itr)->pointer.next = NULL_VADDRESS32;
    mem[(*itr)->sig64.sig1]--;
    node_num--;
  }
  debug(node_num);
  for (hashtable_t<list_t, uint32_t>::iterator_t itr = hash_list.begin();
       itr != hash_list.end(); itr++) {
    uint32_t key = (*itr)->key;
    Vaddress32 list = (*itr)->list;
    assert(doubly_linked_list_sorted.check_sorted_list(list));
    int cnt = 0;
    while (list != NULL_VADDRESS32) {
      node_t *pnode = hash_node.get_usr_data(list);
      assert(pnode != NULL);
      assert(pnode->sig64.sig1 == key);
      assert(pnode->value == (pnode->sig64.sig1 ^ pnode->sig64.sig2));
      cnt++;
      list = doubly_linked_list_sorted.get_next(list);
    }
    assert(mem[(*itr)->key] == cnt);
  }
  debug("Unit test for doubly_linked_list_sorted_t finished successfully");
}

Func_tester(doubly_linked_list_sorted_tester);
};

#endif
