#ifndef __HASHTABLE_HEADER__
#define __HASHTABLE_HEADER__

#include "data_structures_common.h"
#include "im_vector.h"
#include "mem_pool.h"
#include <algorithm>
#include <cassert>
// using namespace std;

template <typename Usr_data_t, typename Hash_key_t,
          typename Extract_key_func_t = typename hashtable_trait_t<
              Usr_data_t, Hash_key_t>::Extract_key_func_t,
          typename Hash_func_t =
              typename hashtable_trait_t<Usr_data_t, Hash_key_t>::Hash_func_t>
class hashtable_t {
public:
  typedef Usr_data_t Hashtable_usr_data_t;

private:
  Vaddress32 *m_p_hash_bucket;
  Im_mem_pool::mem_pool_t *m_p_mem_pool;
  uint32_t m_bucket_size, m_node_num, m_mem_type;
  Extract_key_func_t m_extract_key_func;
  Hash_func_t m_hash_func;
  bool m_protect_dirty;
  void get_next(uint32_t &bucket_pos, Vaddress32 &vptr) {
    assert(vptr);
    vptr = convert(vptr)->next;
    if (vptr != NULL_VADDRESS32)
      return;
    bucket_pos++;
    while (bucket_pos < m_bucket_size &&
           NULL_VADDRESS32 == (vptr = m_p_hash_bucket[bucket_pos])) {
      bucket_pos++;
    }
  };

public:
  inline uint32_t get_mem_type() { return m_mem_type; }
  struct Hash_node_t {
    Vaddress32 next;
    Usr_data_t usr_data;
  };
  inline Hash_node_t *convert(Vaddress32 vptr) {
    return (Hash_node_t *)(m_p_mem_pool->real_address(vptr));
  };
  inline Usr_data_t *get_usr_data(Vaddress32 vptr) {
    vptr = combine_vaddr(m_mem_type, get_vaddr_offset(vptr));
    Hash_node_t *hash_node_ptr = convert(vptr);
    CHECK_NULL_POINTER(hash_node_ptr, NULL);
    return &(hash_node_ptr->usr_data);
  }
  class iterator_t {
    friend class hashtable_t;
    uint32_t bucket_pos;
    Vaddress32 vptr;
    hashtable_t *p_hash;

  public:
    void print(FILE *file) {
      fprintf(file, "iterator=(%u,%u)\n", bucket_pos, vptr);
    };
    iterator_t() : bucket_pos(0), vptr(NULL_VADDRESS32), p_hash(NULL){};
    iterator_t operator++(int) {
      const iterator_t ret = *this;
      p_hash->get_next(bucket_pos, vptr);
      return ret;
    };
    bool operator==(const iterator_t &other) {
      return bucket_pos == other.bucket_pos && vptr == other.vptr &&
             p_hash == other.p_hash;
    };
    bool operator!=(const iterator_t &other) {
      return bucket_pos != other.bucket_pos || vptr != other.vptr ||
             p_hash != other.p_hash;
    };
    Usr_data_t *operator*() {
      Hash_node_t *p_hash_node = p_hash->convert(vptr);
      CHECK_NULL_POINTER(p_hash_node, NULL);
      return &(p_hash_node->usr_data);
    };
  };
  iterator_t begin() {
    iterator_t ret;
    ret.bucket_pos = 0;
    ret.p_hash = this;
    while (ret.bucket_pos < m_bucket_size &&
           (ret.vptr = m_p_hash_bucket[ret.bucket_pos]) == NULL_VADDRESS32) {
      ret.bucket_pos++;
    }
    return ret;
  };
  iterator_t end() {
    iterator_t ret;
    ret.bucket_pos = m_bucket_size;
    ret.vptr = NULL_VADDRESS32;
    ret.p_hash = this;
    return ret;
  }
  hashtable_t()
      : m_p_hash_bucket(NULL), m_p_mem_pool(NULL), m_bucket_size(0),
        m_node_num(0), m_protect_dirty(true){};
  int create(uint32_t bucket_size, Im_mem_pool::mem_pool_t &mem_pool,
             bool protect_dirty = true, bool uniq = true) {
    if (sizeof(Hash_node_t) != sizeof(Vaddress32) + sizeof(Usr_data_t)) {
      ul_writelog(UL_LOG_WARNING,
                  "There might be alignment problem in hashtable_t");
    }
    try {
      m_p_hash_bucket = new Vaddress32[bucket_size];
    } catch (std::bad_alloc) {
      ul_writelog(UL_LOG_FATAL, "Allocating hash bucket of size %u failed",
                  bucket_size);
      return -1;
    }
    m_bucket_size = bucket_size;
    m_p_mem_pool = &mem_pool;
    if (m_p_mem_pool->add_fixed_size_pool(sizeof(Hash_node_t), uniq)) {
      ul_writelog(UL_LOG_FATAL,
                  "add_fixed_size_pool with size %u for hashtable_t failed",
                  sizeof(Hash_node_t));
      delete[] m_p_hash_bucket;
      return -1;
    }
    m_mem_type = m_p_mem_pool->get_corresponding_type(sizeof(Hash_node_t));
    memset(m_p_hash_bucket, 0, sizeof(Vaddress32) * m_bucket_size);
    m_protect_dirty = protect_dirty;
    return 0;
  }
  int clear() {
    const iterator_t end = end();
    for (iterator_t itr = begin(); itr != end; itr++)
      m_p_mem_pool->free(
          combine_vaddr(m_mem_type, get_vaddr_offset((*itr)->vptr)),
          sizeof(Hash_node_t));
    return 0;
  }
  Vaddress32 seek_node_vaddr(const Hash_key_t &key) {
    for (Vaddress32 vptr = m_p_hash_bucket[m_hash_func(key, m_bucket_size)];
         NULL_VADDRESS32 != vptr; vptr = (convert(vptr))->next) {
      if (m_extract_key_func(convert(vptr)->usr_data) == key) {
        return vptr;
      }
    }
    return NULL_VADDRESS32;
  }
  Usr_data_t *seek_node(const Hash_key_t &key) {
    Vaddress32 addr = seek_node_vaddr(key);
    if (NULL_VADDRESS32 == addr)
      return NULL;
    char *ret = (m_p_mem_pool->real_address(addr));
    if (NULL == ret)
      return NULL;
    return &(((Hash_node_t *)ret)->usr_data);
  }
  Vaddress32 add_node_vaddr(const Usr_data_t &usr_data, bool overwrite) {
    return add_node_vaddr(m_extract_key_func(usr_data), usr_data, overwrite);
  }
  Vaddress32 add_node_vaddr(const Hash_key_t &key, const Usr_data_t &usr_data,
                            bool overwrite) {
    Vaddress32 vptr = NULL_VADDRESS32;
    Hash_node_t *ptr = NULL;
    if (!overwrite) {
      vptr = seek_node_vaddr(key);
      if (NULL_VADDRESS32 != vptr) {
        return vptr;
      }
    }
    vptr = m_p_mem_pool->malloc_fixed_size(m_mem_type);
    if (NULL_VADDRESS32 == vptr) {
      ul_writelog(UL_LOG_FATAL,
                  "Malloc memory of length %u failed from mem_pool",
                  sizeof(Hash_node_t));
      return NULL_VADDRESS32;
    }
    ptr = convert(vptr);
    CHECK_NULL_POINTER(ptr, NULL_VADDRESS32);
    if (overwrite) {
      remove_node_vaddr(key);
    }
    uint32_t pos = m_hash_func(key, m_bucket_size);
    ptr->next = m_p_hash_bucket[pos];
    ptr->usr_data = usr_data;
    m_p_hash_bucket[pos] = vptr;
    m_node_num++;
    return vptr;
  }
  Usr_data_t *add_node(const Usr_data_t &usr_data, bool overwrite) {
    return add_node(m_extract_key_func(usr_data), usr_data, overwrite);
  }
  Usr_data_t *add_node(const Hash_key_t &key, const Usr_data_t &usr_data,
                       bool overwrite) {
    Hash_node_t *ptr = convert(add_node_vaddr(key, usr_data, overwrite));
    if (NULL == ptr)
      return NULL;
    return &(ptr->usr_data);
  }
  Vaddress32 remove_node_vaddr(const Hash_key_t &key) {
    Hash_node_t *ptr = NULL;
    Vaddress32 *vpre = &m_p_hash_bucket[m_hash_func(key, m_bucket_size)];
    Vaddress32 vcur = *vpre;
    while (NULL_VADDRESS32 != vcur) {
      ptr = convert(vcur);
      if (NULL == ptr) {
        ul_writelog(UL_LOG_FATAL, "Dereference fake address [%u] failed", vcur);
        return NULL_VADDRESS32;
      }
      if (m_extract_key_func(ptr->usr_data) == key) {
        *vpre = ptr->next;
        m_p_mem_pool->free(vcur, sizeof(Hash_node_t), m_protect_dirty);
        m_node_num--;
        return vcur;
      }
      vpre = &(ptr->next);
      vcur = ptr->next;
    }
    return NULL_VADDRESS32;
  }
  Usr_data_t *remove_node(const Hash_key_t &key) {
    Vaddress32 ret = remove_node_vaddr(key);
    if (NULL_VADDRESS32 == ret)
      return NULL;
    return &(convert(ret)->usr_data);
  }
  struct detect_info_t {
    uint32_t node_num;
    uint32_t bucket_size;
    uint32_t used_bucket;
    uint32_t longest_list_length;
    double load_factor;
    double average_list_length;
  };
  int get_detect_info(detect_info_t &detect_info) {
    detect_info.bucket_size = m_bucket_size;
    uint32_t node_num = 0, longest = 0, used_bucket = 0;
    for (uint32_t pos = 0; pos < m_bucket_size; pos++) {
      uint32_t length = 0;
      for (Vaddress32 vptr = m_p_hash_bucket[pos]; NULL_VADDRESS32 != vptr;
           vptr = convert(vptr)->next) {
        length++;
      }
      longest = std::max(longest, length);
      node_num += length;
      if (length)
        used_bucket++;
    }
    detect_info.node_num = node_num;
    detect_info.used_bucket = used_bucket;
    detect_info.longest_list_length = longest;
    detect_info.load_factor =
        1.0 * detect_info.used_bucket / detect_info.bucket_size;
    detect_info.average_list_length =
        1.0 * detect_info.node_num / detect_info.used_bucket;

    if (node_num != m_node_num) {
      ul_writelog(UL_LOG_WARNING,
                  "[m_node_num:%u] does not match [node_num:%u]", m_node_num,
                  node_num);
    }
    return 0;
  };
  int get_detect_info(char buf[], uint32_t buf_size) {
    detect_info_t detect_info;
    get_detect_info(detect_info);
    snprintf(buf, buf_size, "node number : %u\nbucket size : %u\nused_bucket : "
                            "%u\nlongest list length : %u\nload factor : "
                            "%.3lf\naverage_list_length : %.3lf\n",
             detect_info.node_num, detect_info.bucket_size,
             detect_info.used_bucket, detect_info.longest_list_length,
             detect_info.load_factor, detect_info.average_list_length);
    return 0;
  };
  int rehash(uint32_t new_bucket_size) {
    if (0 == new_bucket_size) {
      new_bucket_size = m_bucket_size << 1;
    }
    ul_writelog(UL_LOG_WARNING,
                "hashtable_t is about to rehash from [%u] to [%u]",
                m_bucket_size, new_bucket_size);
    Vaddress32 *new_hash_bucket;
    try {
      new_hash_bucket = new Vaddress32[new_bucket_size];
    } catch (std::bad_alloc) {
      ul_writelog(UL_LOG_FATAL, "Allocating new hash_bucket failed");
      return -1;
    }
    memset(new_hash_bucket, 0, sizeof(Vaddress32) * new_bucket_size);
    for (uint32_t pos = 0; pos < m_bucket_size; pos++) {
      Vaddress32 &head = m_p_hash_bucket[pos];
      while (head != NULL_VADDRESS32) {
        Vaddress32 vptr = head;
        Hash_node_t *ptr = convert(vptr);
        head = ptr->next;
        uint32_t new_pos =
            m_hash_func(m_extract_key_func(ptr->usr_data), new_bucket_size);
        ptr->next = new_hash_bucket[new_pos];
        new_hash_bucket[new_pos] = vptr;
      }
    }
    //以下作切换
    const Vaddress32 *old_bucket = m_p_hash_bucket;
    //如果hash
    // bucket变大，则先切换bucket，再改变bucket_size;否则可能在一写多读的情况下出现访问越界
    if (new_bucket_size > m_bucket_size) {
      m_p_hash_bucket = new_hash_bucket;
      m_bucket_size = new_bucket_size;
    } else {
      m_bucket_size = new_bucket_size;
      m_p_hash_bucket = new_hash_bucket;
    }
    //最后再delete 原有bucket
    delete[] old_bucket;
    return 0;
  }
};

//*********************************************************
// Unit tester
namespace Hashtable_unit_tester {
struct User_data_t {
  uint32_t a;
  int b;
  char buf[12];
  bool operator==(const User_data_t &other) {
    return a == other.a && b == other.b && 0 == strcmp(buf, other.buf);
  };
};
class Extract_key_t {
public:
  inline int operator()(const User_data_t &data) { return data.a; };
};
class Hash_func_t {
public:
  inline uint32_t operator()(int a, uint32_t m) { return a % m; };
};

void hashtable_tester() {
  Im_mem_pool::mem_pool_t pool;
  int res;
  res = pool.create();
  assert(0 == res);
  hashtable_t<User_data_t, uint32_t> hashtable;
  const int N = 1000000;
  res = hashtable.create(N / 10, pool);
  assert(res == 0);
  User_data_t data;
  const char name[][14] = {"aaa",    "bbbb",    "ccccc",
                           "dddddd", "eeeeeee", "fffffffffff"};
  char buf[1000];
  for (int i = 0; i < N / 2; i++) {
    data.a = i;
    data.b = i * 2;
    strcpy(data.buf, name[i % 4]);
    User_data_t *ptr = hashtable.add_node(i, data, false);
    assert(NULL != ptr);
  }

  for (int i = 0; i < N / 2; i++) {
    User_data_t *ptr = hashtable.seek_node(i);
    assert(ptr != NULL);
    assert(ptr->b == i * 2);
    assert(0 == strcmp(ptr->buf, name[i % 4]));
  }
  for (int i = 0; i < N / 2; i++) {
    data.a = i;
    data.b = i / 2;
    strcpy(data.buf, name[i % 6]);
    User_data_t *ptr = hashtable.add_node(i, data, true);
    assert(NULL != ptr);
  }
  for (int i = N / 2; i < N; i++) {
    data.a = i;
    data.b = i / 2;
    strcpy(data.buf, name[i % 6]);
    Vaddress32 vptr = hashtable.add_node_vaddr(i, data, false);
    assert(NULL_VADDRESS32 != vptr);
    assert(*(hashtable.get_usr_data(vptr)) == data);
  }
  hashtable.get_detect_info(buf, sizeof(buf));
  debug(buf);
  int cc = 0;
  for (hashtable_t<User_data_t, uint32_t>::iterator_t itr = hashtable.begin();
       itr != hashtable.end(); itr++) {
    cc++;
    int key = (*itr)->a;
    assert((*itr)->b == key / 2);
    assert(0 == strcmp((*itr)->buf, name[key % 6]));
  }
  res = hashtable.rehash(N);
  assert(res == 0);
  hashtable.get_detect_info(buf, sizeof(buf));
  debug(buf);
  cc = 0;
  for (hashtable_t<User_data_t, uint32_t>::iterator_t itr = hashtable.begin();
       itr != hashtable.end(); itr++) {
    cc++;
    int key = (*itr)->a;
    assert((*itr)->b == key / 2);
    assert(0 == strcmp((*itr)->buf, name[key % 6]));
  }
  assert(cc == N);
  for (int i = 2 * N; i >= 0; i -= 2) {
    User_data_t *ptr = hashtable.remove_node(i);
    assert(i >= N || ptr);
  }
  cc = 0;
  for (hashtable_t<User_data_t, uint32_t>::iterator_t itr = hashtable.begin();
       itr != hashtable.end(); itr++) {
    cc++;
    int key = (*itr)->a;
    assert(key & 1);
    assert((*itr)->b == key / 2);
    assert(0 == strcmp((*itr)->buf, name[key % 6]));
  }
  for (int i = 0; i < 10000; i++) {
    int key = rand() % N;
    User_data_t *ptr = hashtable.seek_node(key);
    if (key & 1) {
      assert(ptr != NULL);
      assert(ptr->b == key / 2);
      assert(0 == strcmp(ptr->buf, name[key % 6]));
    } else {
      assert(NULL == ptr);
    }
  }
  assert(cc == N / 2);
  hashtable.get_detect_info(buf, sizeof(buf));
  debug(buf);
  debug("Unit test for hashtable finished");
}
Func_tester(hashtable_tester);
};

#endif
