#ifndef __IM_PARALLEL_FRAMEWORK_BUFFER_HEADER
#define __IM_PARALLEL_FRAMEWORK_BUFFER_HEADER
//#include "com_log.h"
#include <cassert>
#include <pthread.h>
#include <semaphore.h>
#include "common_header_zhc.h"
//#include "el_common.h"
//#define PARALLEL_PRINT_TIME
#define SEM_WAIT(p_sem) while(sem_wait(p_sem))	
//#define ul_writelog ul_writelog 
template<typename T> struct parallel_framework_buffer_t;
template<typename T>
struct processor_argument_t
{
	parallel_framework_buffer_t<T>* p_frame;
	int pid;
};

template<typename T>
class queue_t
{
	T* array;
	int head,tail,cap;
public:
	bool has_item(){return head<tail;};
	bool has_spot(){return tail<cap;};
	bool is_empty(){return head==tail;}
	bool is_full(){return tail==cap;}
	int get_cap(){return cap;};
	int get_tail(){return tail;}
	int get_head(){return head;}
	void reset_head(){head=0;};
	void clear(){head=tail=0;};
	queue_t():array(NULL),head(0),tail(0),cap(0){};
	~queue_t(){if(NULL!=array) delete [] array;};
	int create(int size)
	{
		try
		{
			array=new T[size];
		}
		catch(std::bad_alloc)
		{
			ul_writelog(UL_LOG_FATAL,"New failed");
			return -1;
		}
		cap=size;
		head=tail=0;
		return 0;
	};
	T& front(){return array[head];};
	void pop(){head++;};
	T& back(){return array[tail];};
	void push(){tail++;};
};

template<typename T>
struct parallel_framework_buffer_t
{
	enum framework_status_t{
		frame_Null,  // wait for being created
		frame_Ready,  // Have been created
		frame_Running  //frame_Running
	};
	enum buffer_status_t{
		buffer_Null,  // No buffer here
		buffer_Waiting,  // Wait for processors
		buffer_Ready,  // wait for the consumer
		buffer_Error   // process error
	};
	struct queue_item_t
	{
		T item;
		int processor_return;
	};

	typedef int (*functionT)(T&,uint32_t);
	typedef int (*functionC)(T&,int);

	queue_t<queue_item_t> *p_buffer;
	buffer_status_t *buffer_status;
	sem_t *sem_ready_buffer,*sem_waiting_buffer,*sem_null_buffer;

	pthread_t *_processor_tid,_consumer_tid;

	functionT _generator;
	functionC _consumer;
	functionT _processor;
	
	framework_status_t status;

	int num_processor;
	int generator_item_count,consumer_item_count;
	int *processor_item_count;

private:
	processor_argument_t<T> *processor_arg;
	int create_threads();
public:
	inline int generate_one_buffer(int);
	inline int consume_one_buffer(int);
	inline int process_one_buffer(int);

	parallel_framework_buffer_t():p_buffer(NULL),buffer_status(NULL),sem_ready_buffer(NULL),
								sem_waiting_buffer(NULL),sem_null_buffer(NULL),_processor_tid(NULL),
								_generator(NULL),_consumer(NULL),_processor(NULL),
								status(frame_Null),num_processor(0),generator_item_count(0),
								consumer_item_count(0),processor_item_count(NULL),processor_arg(NULL){};
	~parallel_framework_buffer_t(){delete_all();};
	void delete_all()
	{
		if(NULL!=p_buffer) {
			delete [] p_buffer;
		}
		if(NULL!=buffer_status) {
			delete [] buffer_status;
		}
		if(NULL!=_processor_tid) {
			delete [] _processor_tid;
		}
		if(NULL!=processor_item_count) {
			delete [] processor_item_count;
		}
		
		if(NULL!=sem_waiting_buffer) {
			delete [] sem_waiting_buffer;
		}
		if(NULL!=sem_ready_buffer) {
			delete [] sem_ready_buffer;
		}
		if(NULL!=sem_null_buffer) {
			delete [] sem_null_buffer;
		}
		if(NULL!=processor_arg) {
			delete [] processor_arg;
		}
	};
	int create(int _num_processor,int _buf_size);
	// generator return:  0:succeed, 1:stop, <0:error and exit
	int run(functionT generator,functionT processor,functionC consumer);
	private:
	int run();
};
template<typename T>
int parallel_framework_buffer_t<T>::create(int _num_processor,int _buf_size)
{
	if(_num_processor<=0||_buf_size<=0){
		ul_writelog(UL_LOG_FATAL,"Incorrect arguments [num_processor:%d] [buf_size:%d]",
					_num_processor,_buf_size);
		return -1;
	}
	if(status!=frame_Null) {
		ul_writelog(UL_LOG_FATAL,"parallel_framework_buffer_t::create has already been called successfully");
		return -1;
	}
	try
	{
		p_buffer=new queue_t<queue_item_t>[_num_processor];
		processor_item_count=new int[_num_processor];
		_processor_tid=new pthread_t[_num_processor];
		processor_arg=new processor_argument_t<T>[_num_processor];
		buffer_status=new buffer_status_t[_num_processor];
		sem_ready_buffer=new sem_t[_num_processor];
		sem_waiting_buffer=new sem_t[_num_processor];
		sem_null_buffer=new sem_t[_num_processor];
	}
	catch(std::bad_alloc)
	{
		delete_all();
		ul_writelog(UL_LOG_FATAL,"allocating memory failed in parallel_framework_buffer_t::create");
		return -1;
	}
	for(int i=0;i<_num_processor;i++) {
		if(p_buffer[i].create(_buf_size)) {
			delete_all();
			ul_writelog(UL_LOG_FATAL,"create queue_t failed");
			return -1;
		}
	}
	num_processor=_num_processor;
	status=frame_Ready;
	generator_item_count=consumer_item_count=0;
	memset(processor_item_count,0,sizeof(int)*num_processor);
	ul_writelog(UL_LOG_NOTICE,"parallel_framework_buffer_t::create succeeded with [num_processor:%d] [buf_size:%d]",_num_processor,_buf_size);
	return 0;
}
template<typename T>
void* thread_processor(void* arg)
{
#ifdef PARALLEL_PRINT_TIME
	double pre_time,wait_time=0,post_time=0;
#endif
	parallel_framework_buffer_t<T>* p_frame=((processor_argument_t<T>*)arg)->p_frame;
	assert(NULL!=p_frame);
	const int pid=((processor_argument_t<T>*)arg)->pid;

	char thread_name[20];
	snprintf(thread_name,sizeof(thread_name),"processor #%d",pid);

	for(;;) {
#ifdef PARALLEL_PRINT_TIME
		pre_time=get_wall_time();
#endif

		SEM_WAIT(p_frame->sem_waiting_buffer+pid);	
#ifdef PARALLEL_PRINT_TIME
		wait_time+=get_wall_time()-pre_time;
#endif

		if(p_frame->buffer_status[pid]!=parallel_framework_buffer_t<T>::buffer_Waiting) {
			break;
		}
		p_frame->process_one_buffer(pid);	
#ifdef PARALLEL_PRINT_TIME
		pre_time=get_wall_time();
#endif

		sem_post(p_frame->sem_ready_buffer+pid);
#ifdef PARALLEL_PRINT_TIME
		post_time+=get_wall_time()-pre_time;
#endif

	}
	//多post一次，用于结束时唤醒consumer
	sem_post(p_frame->sem_ready_buffer+pid);
#ifdef PARALLEL_PRINT_TIME
	ul_writelog(UL_LOG_NOTICE,"processor #%d wait time=%lf post time=%lf",pid,wait_time,post_time);
#endif

	return (void*)0;
}


// User defined consumer return: 0=succeed, othe=error
template<typename T>
void* thread_consumer(void* arg)
{
#ifdef PARALLEL_PRINT_TIME
	double pre_time,wait_time=0,post_time=0;
#endif

	parallel_framework_buffer_t<T>* p_frame=(parallel_framework_buffer_t<T>*)arg;
	

	int buffer_pos=0;
	for(;;) {
#ifdef PARALLEL_PRINT_TIME
		pre_time=get_wall_time();
#endif

		SEM_WAIT(p_frame->sem_ready_buffer+buffer_pos);
#ifdef PARALLEL_PRINT_TIME
		wait_time+=get_wall_time()-pre_time;
#endif

		if(p_frame->buffer_status[buffer_pos]!=parallel_framework_buffer_t<T>::buffer_Ready) {
			break;
		}
		p_frame->consume_one_buffer(buffer_pos);
#ifdef PARALLEL_PRINT_TIME
		pre_time=get_wall_time();
#endif

		sem_post(p_frame->sem_null_buffer+buffer_pos);
#ifdef PARALLEL_PRINT_TIME
		post_time+=get_wall_time()-pre_time;
#endif

		buffer_pos=(buffer_pos+1)%p_frame->num_processor;
	}
#ifdef PARALLEL_PRINT_TIME
	ul_writelog(UL_LOG_NOTICE,"consumer wait time=%lf post time=%lf",wait_time,post_time);
#endif

	return (void*)0;
}
template<typename T>
int parallel_framework_buffer_t<T>::create_threads()
{
	status=frame_Running;
	for(int i=0;i<num_processor;i++) {
		processor_arg[i].p_frame=this;
		processor_arg[i].pid=i;
		if(0>pthread_create(_processor_tid+i,NULL,thread_processor<T>,(void*)(processor_arg+i))) {
			ul_writelog(UL_LOG_FATAL,"Create processor thread #%d failed",i);
			status=frame_Ready;
			return -1;
		}
	}
	
	ul_writelog(UL_LOG_NOTICE,"%d processors threads are created", num_processor);

	if(0>pthread_create(&_consumer_tid,NULL,thread_consumer<T>,(void*)this)) {
		ul_writelog(UL_LOG_FATAL,"Create consumer thread failed");
		status=frame_Ready;
		return -1;
	}
	ul_writelog(UL_LOG_NOTICE,"The consumer thread is created");
	return 0;
}
template<typename T>
inline int parallel_framework_buffer_t<T>::generate_one_buffer(int buffer_pos)
{
	queue_t<queue_item_t>& buffer=p_buffer[buffer_pos];
	int offset=buffer.get_cap()*buffer_pos;
	buffer.clear();
	while(buffer.has_spot()) {
		int gen_res=_generator(buffer.back().item,offset++);
		if(gen_res==1) {
			ul_writelog(UL_LOG_NOTICE,"generator successfully stopped");
			buffer_status[buffer_pos]=buffer_Waiting;
			return gen_res;
		}
		if(gen_res<0) {
			ul_writelog(UL_LOG_FATAL,"generator error, exit unsucceesfully");
			buffer_status[buffer_pos]=buffer_Waiting;
			return gen_res;
		}
		//printf("generate one item at %d\n",buffer.get_tail());
		buffer.push();	
	}
	//printf("buffer %d is filled\n",buffer_pos);
	buffer_status[buffer_pos]=buffer_Waiting;
	return 0;
}
template<typename T>
inline int parallel_framework_buffer_t<T>::consume_one_buffer(int buffer_pos)
{
	queue_t<queue_item_t>& buffer=p_buffer[buffer_pos];
	buffer.reset_head();
	//printf("start consuming buffer_pos %d\n",buffer_pos);
	while(buffer.has_item()) {
		if(0==_consumer(buffer.front().item,buffer.front().processor_return)) {
			consumer_item_count++;
		}
		//printf("consume one item at %d\n",buffer.get_head());
		buffer.pop();
	}
	buffer_status[buffer_pos]=buffer_Null;
	return 0;
}
// User defined processor return: 0 = succeed,other = error
template<typename T>
inline int parallel_framework_buffer_t<T>::process_one_buffer(int pid)
{
	queue_t<queue_item_t>& buffer=p_buffer[pid];
	buffer.reset_head();
	int pres;
	while(buffer.has_item()) {
		if((pres=_processor(buffer.front().item,pid))==0) {
			processor_item_count[pid]++;
		}
		//printf("processor #%d processed one item\n",pid);
		buffer.front().processor_return=pres;
		buffer.pop();
	}
	buffer_status[pid]=buffer_Ready;
	return 0;
}

template<typename T>
int parallel_framework_buffer_t<T>::run(functionT generator,functionT processor,functionC consumer)
{
	if(NULL==generator||NULL==processor||NULL==consumer) {
		ul_writelog(UL_LOG_FATAL,"NULL function pointer");
		return -1;
	}
	if(status!=frame_Ready) {
		ul_writelog(UL_LOG_FATAL,
				"Call parallel_framework_buffer_t::create before calling parallel_framework_t::run");
		return -1;
	}
	_generator=generator;
	_processor=processor;
	_consumer=consumer;
	for(int i=0;i<num_processor;i++) {
		sem_init(sem_waiting_buffer+i,0,0);
		sem_init(sem_ready_buffer+i,0,0);
		sem_init(sem_null_buffer+i,0,1);
	}
	if(0>create_threads()) return -1;
	return run();
}
template<typename T>
int parallel_framework_buffer_t<T>::run()
{
#ifdef PARALLEL_PRINT_TIME
	double pre_time,wait_time=0,post_time=0;
#endif

	// generator return:  0:succeed, 1:stop, <0:error, exit
	//Start generator
	int gen_res;
	int buffer_pos=0;
	for(;;) {
#ifdef PARALLEL_PRINT_TIME
		pre_time=get_wall_time();
#endif

		SEM_WAIT(sem_null_buffer+buffer_pos);
#ifdef PARALLEL_PRINT_TIME
		wait_time+=get_wall_time()-pre_time;
#endif

		gen_res=generate_one_buffer(buffer_pos);
//		fprintf(stderr,"%d: ",buffer_pos);
//		for(int i=0;i<num_processor;i++) {
//			fprintf(stderr,"%d ",buffer_status[i]);
//		}
//		fprintf(stderr,"\n");
#ifdef PARALLEL_PRINT_TIME
		pre_time=get_wall_time();
#endif

		sem_post(sem_waiting_buffer+buffer_pos);
#ifdef PARALLEL_PRINT_TIME
		post_time+=get_wall_time()-pre_time;
#endif


		if(gen_res!=0) {
			break;
		}
		buffer_pos=(buffer_pos+1)%num_processor;
	}
	status=frame_Ready;
	// generator has stopped
	//用于唤醒所有的processor
	for(int i=0;i<num_processor;i++) {
		sem_post(sem_waiting_buffer+i);
	}
#ifdef PARALLEL_PRINT_TIME
	ul_writelog(UL_LOG_NOTICE,"generator wait time=%lf post time=%lf",wait_time,post_time);
#endif

	void* thread_status;
	for(int i=0;i<num_processor;i++) {
		pthread_join(_processor_tid[i],&thread_status);
		ul_writelog(UL_LOG_NOTICE,"processor #%d exit with %lld and %d items were successfully processed",i,(long long int)(thread_status),processor_item_count[i]);
	}
	pthread_join(_consumer_tid,&thread_status);
	for(int i=0;i<num_processor;i++) {
		sem_destroy(sem_null_buffer+i);
		sem_destroy(sem_waiting_buffer+i);
		sem_destroy(sem_ready_buffer+i);
	}
	ul_writelog(UL_LOG_NOTICE,"%d items were consumed successfully",consumer_item_count);
	ul_writelog(UL_LOG_NOTICE,"generator exit with %d items generated successfully",generator_item_count);
	ul_writelog(UL_LOG_NOTICE,"parallel_framework_buffer_t::run exit!");
	return 0;
}
#endif
