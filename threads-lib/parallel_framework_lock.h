#ifndef __IM_PARALLEL_FRAMEWORK_LOCK_HEADER
#define __IM_PARALLEL_FRAMEWORK_LOCK_HEADER
#include <cassert>
#include <semaphore.h>
//#include <lock.h>
#include "common_header_zhc.h"
#include <pthread.h>
#define SEM_WAIT(p_sem) while (sem_wait(p_sem))

template <typename T> struct parallel_framework_lock_t;
template <typename T> struct processor_argument_t {
  parallel_framework_lock_t<T> *p_frame;
  int pid;
};

template <typename T> struct parallel_framework_lock_t {
  enum framework_status_t {
    frame_Null,   // wait for being created
    frame_Ready,  // Have been created
    frame_Running // frame_Running
  };
  enum item_status_t {
    item_Null,    // No item here
    item_Waiting, // Wait for processors
    item_Ready,   // wait for the consumer
    item_Error    // process error
  };

  typedef int (*functionT)(T &, uint32_t);
  typedef int (*functionC)(T &, int);
  item_status_t *item_status;
  T *array;
  int head, tail;
  sem_t *sem_ready_cell, sem_waiting_cell;
  sem_t sem_null_cell;
  pthread_t *_processor_tid, _consumer_tid;
  functionT _generator;
  functionC _consumer;
  functionT _processor;

  framework_status_t status;
  int num_processor, queue_size;
  int generator_item_count, consumer_item_count;
  int *processor_item_count, *processor_return;
  int mutex_dequeue() {
    pthread_mutex_lock(&dequeue_mutex);
    int ret = head;
    if (head == tail) {
      if (status == frame_Running) {
        ul_writelog(UL_LOG_FATAL,
                    "Empty queue in mutex_dequeue while framework is running");
      }
      ret = -1;
    } else {
      head = ((head + 1) == queue_size) ? 0 : head + 1;
    }
    pthread_mutex_unlock(&dequeue_mutex);
    return ret;
  };

private:
  void inc_head() { head = ((head + 1) == queue_size) ? 0 : head + 1; };
  processor_argument_t<T> *processor_arg;
  int create_threads();
  pthread_mutex_t dequeue_mutex;

public:
  inline int generate_one_item();
  inline int consume_one_item(int);
  inline int process_one_item(int, int);

  parallel_framework_lock_t()
      : item_status(NULL), array(NULL), head(0), tail(0), sem_ready_cell(NULL),
        _processor_tid(NULL), _generator(NULL), _consumer(NULL),
        _processor(NULL), status(frame_Null), num_processor(0), queue_size(0),
        generator_item_count(0), consumer_item_count(0),
        processor_item_count(NULL), processor_return(NULL),
        processor_arg(NULL){};
  ~parallel_framework_lock_t() {
    if (NULL != item_status) {
      delete[] item_status;
    }
    if (NULL != _processor_tid) {
      delete[] _processor_tid;
    }
    if (NULL != processor_item_count) {
      delete[] processor_item_count;
    }
    if (NULL != sem_ready_cell) {
      delete[] sem_ready_cell;
    }

    if (NULL != array) {
      delete[] array;
    }
    if (NULL != processor_arg) {
      delete[] processor_arg;
    }
    if (NULL != processor_return) {
      delete[] processor_return;
    }
  };
  int create(int _num_processor, int _buf_size);
  // generator return:  0:succeed, 1:stop, <0:error and exit
  int run(functionT generator, functionT processor, functionC consumer);

private:
  int run();
};
template <typename T>
int parallel_framework_lock_t<T>::create(int _num_processor, int _buf_size) {
  if (_num_processor <= 0 || _buf_size <= 0) {
    ul_writelog(UL_LOG_FATAL,
                "Incorrect arguments [num_processor:%d] [buf_size:%d]",
                _num_processor, _buf_size);
    return -1;
  }
  if (status != frame_Null) {
    ul_writelog(UL_LOG_FATAL, "parallel_framework_lock_t::create has already "
                              "been called successfully");
    return -1;
  }
  try {
    processor_item_count = new int[_num_processor];
    processor_return = new int[_num_processor * _buf_size];
    _processor_tid = new pthread_t[_num_processor];
    processor_arg = new processor_argument_t<T>[ _num_processor ];
    array = new T[_num_processor * _buf_size];
    item_status = new item_status_t[_num_processor * _buf_size];
    sem_ready_cell = new sem_t[_num_processor * _buf_size];
  } catch (std::bad_alloc) {
    if (NULL != _processor_tid)
      delete[] _processor_tid;
    if (NULL != processor_item_count)
      delete[] processor_item_count;
    if (NULL != processor_arg)
      delete[] processor_arg;
    if (NULL != item_status)
      delete[] item_status;
    if (NULL != array)
      delete[] array;
    if (NULL != processor_return)
      delete[] processor_return;
    ul_writelog(
        UL_LOG_FATAL,
        "allocating memory failed in parallel_framework_lock_t::create");
    return -1;
  }
  num_processor = _num_processor;
  queue_size = num_processor * _buf_size;
  status = frame_Ready;
  generator_item_count = consumer_item_count = 0;
  memset(processor_item_count, 0, sizeof(int) * num_processor);
  ul_writelog(UL_LOG_NOTICE, "parallel_framework_lock_t::create succeeded with "
                             "[num_processor:%d] [buf_size:%d] [queue_size:%d]",
              _num_processor, _buf_size, queue_size);
  return 0;
}
template <typename T> void *thread_processor(void *arg) {
  parallel_framework_lock_t<T> *p_frame =
      ((processor_argument_t<T> *)arg)->p_frame;
  assert(NULL != p_frame);
  const int pid = ((processor_argument_t<T> *)arg)->pid;
  char thread_name[20];
  snprintf(thread_name, sizeof(thread_name), "processor #%d", pid);
  for (;;) {
    SEM_WAIT(&p_frame->sem_waiting_cell);
    int pos = p_frame->mutex_dequeue();
    if (pos >= 0) {
      p_frame->process_one_item(pos, pid);
    } else {
      break;
    }
    sem_post(p_frame->sem_ready_cell + pos);
  }
  sem_post(p_frame->sem_ready_cell + p_frame->head);
  return (void *)0;
}

// User defined consumer return: 0=succeed, othe=error
template <typename T> void *thread_consumer(void *arg) {
  parallel_framework_lock_t<T> *p_frame = (parallel_framework_lock_t<T> *)arg;

  int pos = 0;
  for (;;) {
    SEM_WAIT(p_frame->sem_ready_cell + pos);
    if (p_frame->item_status[pos] == parallel_framework_lock_t<T>::item_Null) {
      break;
    } else if (p_frame->item_status[pos] ==
               parallel_framework_lock_t<T>::item_Waiting) {
      ul_writelog(UL_LOG_FATAL, "A Ready item is expected by the consumer, but "
                                "a Waiting item is encountered.");
      break;
    }
    p_frame->consume_one_item(pos);
    sem_post(&(p_frame->sem_null_cell));
    pos = (pos + 1 == p_frame->queue_size) ? 0 : pos + 1;
  }
  return (void *)0;
}
template <typename T> int parallel_framework_lock_t<T>::create_threads() {
  status = frame_Running;
  for (int i = 0; i < num_processor; i++) {
    processor_arg[i].p_frame = this;
    processor_arg[i].pid = i;
    if (0 > pthread_create(_processor_tid + i, NULL, thread_processor<T>,
                           (void *)(processor_arg + i))) {
      ul_writelog(UL_LOG_FATAL, "Create processor thread #%d failed", i);
      status = frame_Ready;
      return -1;
    }
  }
  ul_writelog(UL_LOG_NOTICE, "%d processors threads are created",
              num_processor);
  if (0 >
      pthread_create(&_consumer_tid, NULL, thread_consumer<T>, (void *)this)) {
    ul_writelog(UL_LOG_FATAL, "Create consumer thread failed");
    status = frame_Ready;
    return -1;
  }
  ul_writelog(UL_LOG_NOTICE, "The consumer thread is created");
  return 0;
}
template <typename T>
inline int parallel_framework_lock_t<T>::generate_one_item() {
  int gen_res = _generator(array[tail], tail);
  if (gen_res == 1) {
    ul_writelog(UL_LOG_NOTICE, "generator successfully stopped");
  }
  if (gen_res < 0) {
    ul_writelog(UL_LOG_FATAL, "generator error, exit unsucceesfully");
  }
  return gen_res;
}
template <typename T>
inline int parallel_framework_lock_t<T>::consume_one_item(int pos) {
  int st = _consumer(array[pos], processor_return[pos]);
  if (st) {
    ul_writelog(
        UL_LOG_WARNING,
        "consumer failed to consume one item [return:%d].The item is skipped.",
        st);
  } else {
    consumer_item_count++;
  }
  item_status[pos] = item_Null;
  return 0;
}
// User defined processor return: 0 = succeed,other = error
template <typename T>
inline int parallel_framework_lock_t<T>::process_one_item(int pos, int pid) {
  int pres;
  if ((pres = _processor(array[pos], pid)) == 0) {
    item_status[pos] = item_Ready;
    processor_item_count[pid]++;
  } else {
    item_status[pos] = item_Error;
  }
  processor_return[pos] = pres;
  return pres;
}

template <typename T>
int parallel_framework_lock_t<T>::run(functionT generator, functionT processor,
                                      functionC consumer) {
  if (NULL == generator || NULL == processor || NULL == consumer) {
    ul_writelog(UL_LOG_FATAL, "NULL function pointer");
    return -1;
  }
  if (status != frame_Ready) {
    ul_writelog(UL_LOG_FATAL, "Call parallel_framework_lock_t::create before "
                              "calling parallel_framework_t::run");
    return -1;
  }
  if (pthread_mutex_init(&dequeue_mutex, NULL)) {
    ul_writelog(UL_LOG_FATAL, " initialize dequeue_mutex error");
    return -1;
  }
  _generator = generator;
  _processor = processor;
  _consumer = consumer;
  for (int i = 0; i < queue_size; i++) {
    if (sem_init(sem_ready_cell + i, 0, 0)) {
      ul_writelog(UL_LOG_FATAL, "sem_init failed");
    }
  }
  if (sem_init(&sem_null_cell, 0, queue_size - 1)) {
    ul_writelog(UL_LOG_FATAL, "sem_init failed");
  }
  if (sem_init(&sem_waiting_cell, 0, 0)) {
    ul_writelog(UL_LOG_FATAL, "sem_init failed");
  }
  if (0 > create_threads())
    return -1;

  int res = run();
  return res;
}
template <typename T> int parallel_framework_lock_t<T>::run() {
  // generator return:  0:succeed, 1:stop, <0:error, exit
  // Start generator
  int gen_res;
  // T* array=queue.array;
  for (;;) {
    SEM_WAIT(&sem_null_cell);
    gen_res = generate_one_item();
    if (gen_res != 0) {
      break;
    }
    item_status[tail] = item_Waiting;
    tail = ((tail + 1) == queue_size) ? 0 : tail + 1;
    generator_item_count++;
    sem_post(&sem_waiting_cell);
  }
  status = frame_Ready;
  //多post num_processor次用于唤醒所有的processor
  for (int i = 0; i < num_processor; i++) {
    sem_post(&sem_waiting_cell);
  }
  void *thread_status;
  for (int i = 0; i < num_processor; i++) {
    pthread_join(_processor_tid[i], &thread_status);
    ul_writelog(
        UL_LOG_NOTICE,
        "processor #%d exit with %lld and %d items were successfully processed",
        i, (long long int)(thread_status), processor_item_count[i]);
  }
  pthread_join(_consumer_tid, &thread_status);
  for (int i = 0; i < queue_size; i++) {
    sem_destroy(sem_ready_cell + i);
  }
  sem_destroy(&sem_null_cell);
  sem_destroy(&sem_waiting_cell);
  pthread_mutex_destroy(&dequeue_mutex);

  ul_writelog(UL_LOG_NOTICE, "%d items were consumed successfully",
              consumer_item_count);
  ul_writelog(UL_LOG_NOTICE,
              "generator exit with %d items generated successfully",
              generator_item_count);
  ul_writelog(UL_LOG_NOTICE, "parallel_framework_lock_t::run for  exit!");
  return 0;
}
#endif
