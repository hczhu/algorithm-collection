#ifndef __IM_PARALLEL_FRAMEWORK_SEMAPHORE_HEADER
#define __IM_PARALLEL_FRAMEWORK_SEMAPHORE_HEADER
//#include "com_log.h"
#include <cassert>
#include <semaphore.h>
#include <pthread.h>
#include "common_header_zhc.h"
//#define OUTPUT
//#define MORE
//#define CONSUMER_OUTPUT
//#define PRINT_TIME
template<typename T> struct parallel_framework_semaphore_t;
template<typename T>
struct processor_argument_t
{
	parallel_framework_semaphore_t<T>* p_frame;
	int pid;
};

template<typename T>
struct parallel_framework_semaphore_t
{
	enum framework_status_t{
		frame_Null,  // wait for being created
		frame_Ready,  // Have been created
		frame_Running  //frame_Running
	};
	enum item_status_t{
		item_Null,  // No item here
		item_Waiting,  // Wait for processors
		item_Ready,  // wait for the consumer
		item_Error   // process error
	};

	typedef int (*functionT)(T&,uint32_t);
	typedef int (*functionC)(T&,int);
	item_status_t *item_status;
	T *array;
	int head,tail;
	sem_t *sem_ready_cell,*sem_waiting_cell;
	sem_t sem_null_cell;
	pthread_t *_processor_tid,_consumer_tid;
	functionT _generator;
	functionC _consumer;
	functionT _processor;
	int _mode;
	
	framework_status_t status;
	int num_processor,queue_size;
	int generator_item_count,consumer_item_count;
	int *processor_item_count,*processor_return;
private:
	processor_argument_t<T> *processor_arg;
	int create_threads();
	int run0();
	int run1();
	int run2();
public:
	inline int generate_one_item();
	inline int consume_one_item();
	inline int process_one_item(int,int);

	parallel_framework_semaphore_t():
							item_status(NULL),array(NULL),head(0),tail(0),sem_ready_cell(NULL),
							sem_waiting_cell(NULL),_processor_tid(NULL),_generator(NULL),_consumer(NULL),
							_processor(NULL),status(frame_Null),num_processor(0),queue_size(0),
							generator_item_count(0),consumer_item_count(0),processor_item_count(NULL),
							processor_return(NULL),processor_arg(NULL){};
	~parallel_framework_semaphore_t()
	{
		if(NULL!=item_status) {
			delete [] item_status;
		}
		if(NULL!=_processor_tid) {
			delete [] _processor_tid;
		}
		if(NULL!=processor_item_count) {
			delete [] processor_item_count;
		}
		
		if(NULL!=sem_waiting_cell) {
			for(int i=0;i<num_processor;i++) {
				sem_destroy(sem_waiting_cell+i);
			}
		}
		if(NULL!=sem_ready_cell) {
			for(int i=0;i<num_processor;i++) {
				sem_destroy(sem_ready_cell+i);
			}
			delete [] sem_ready_cell;
		}
		
		if(NULL!=array) {
			delete [] array;
		}
		if(NULL!=processor_arg) {
			delete [] processor_arg;
		}
		if(NULL!=processor_return) {
			delete [] processor_return;
		}
		sem_destroy(&sem_null_cell);
	};
	int create(int _num_processor,int _buf_size);
	// generator return:  0:succeed, 1:stop, <0:error and exit
	int run(functionT generator,functionT processor,functionC consumer,int mode=0);
};
template<typename T>
int parallel_framework_semaphore_t<T>::create(int _num_processor,int _buf_size)
{
	if(_num_processor<=0||_buf_size<=0){
		ul_writelog(UL_LOG_FATAL,"Incorrect arguments [num_processor:%d] [buf_size:%d]",
					_num_processor,_buf_size);
		return -1;
	}
	if(status!=frame_Null) {
		ul_writelog(UL_LOG_FATAL,"parallel_framework_semaphore_t::create has already been called successfully");
		return -1;
	}
	try
	{
		processor_item_count=new int[_num_processor];
		processor_return=new int[_num_processor*_buf_size];
		_processor_tid=new pthread_t[_num_processor];
		processor_arg=new processor_argument_t<T>[_num_processor];
		array=new T[_num_processor*_buf_size];
		item_status=new item_status_t[_num_processor*_buf_size];
		sem_ready_cell=new sem_t[_num_processor*2];
		sem_waiting_cell=sem_ready_cell+_num_processor;
		assert(sem_waiting_cell);
	}
	catch(std::bad_alloc)
	{
		if(NULL!=_processor_tid)delete [] _processor_tid;
		if(NULL!=processor_item_count)delete [] processor_item_count;
		if(NULL!=processor_arg)delete [] processor_arg;
		if(NULL!=item_status)delete [] item_status;
		if(NULL!=array)delete [] array;
		if(NULL!=processor_return)delete [] processor_return;
		ul_writelog(UL_LOG_FATAL,"allocating memory failed in parallel_framework_semaphore_t::create");
		return -1;
	}
	num_processor=_num_processor;
	queue_size=num_processor*_buf_size;
	status=frame_Ready;
	generator_item_count=consumer_item_count=0;
	memset(processor_item_count,0,sizeof(int)*num_processor);
	ul_writelog(UL_LOG_NOTICE,"parallel_framework_semaphore_t::create succeeded with [num_processor:%d] [buf_size:%d] [queue_size:%d]",_num_processor,_buf_size,queue_size);
	return 0;
}
template<typename T>
void* thread_processor(void* arg)
{
#ifdef PRINT_TIME
	const double wall_time=get_wall_time();
	double cpu_time=0,pre_cpu=0;
#endif
	parallel_framework_semaphore_t<T>* p_frame=((processor_argument_t<T>*)arg)->p_frame;
	assert(NULL!=p_frame);
	const int pid=((processor_argument_t<T>*)arg)->pid;
	const int inc=p_frame->num_processor;
	const int queue_size=p_frame->queue_size;	
	char thread_name[20];
	snprintf(thread_name,sizeof(thread_name),"processor #%d",pid);

	//int pres;
	int pos=pid;
	for(;;) {
		sem_wait(p_frame->sem_waiting_cell+pid);	
#ifdef PRINT_TIME
		pre_cpu=get_wall_time();
#endif
		if(p_frame->status!=parallel_framework_semaphore_t<T>::frame_Running) {
			break;
		}
		assert(p_frame->item_status[pos]==parallel_framework_semaphore_t<T>::item_Waiting);
		p_frame->process_one_item(pos,pid);	
		pos=(pos+inc)%queue_size;
#ifdef PRINT_TIME
		cpu_time+=get_wall_time()-pre_cpu;
#endif
		sem_post(p_frame->sem_ready_cell+pid);
	}
#ifdef PRINT_TIME
	pre_cpu=get_wall_time();
#endif
	while(p_frame->item_status[pos]==parallel_framework_semaphore_t<T>::item_Waiting)
	{
		p_frame->process_one_item(pos,pid);
		pos=(pos+inc)%queue_size;
		sem_post(p_frame->sem_ready_cell+pid);
	}
	//多post一次，用于结束时唤醒consumer
	sem_post(p_frame->sem_ready_cell+pid);
#ifdef PRINT_TIME
	cpu_time+=get_wall_time()-pre_cpu;
	ul_writelog(UL_LOG_NOTICE,"processor #%d CPU time: %lf, wall time: %lf",pid,cpu_time,get_wall_time()-wall_time);
#endif
	return (void*)0;
}
template<typename T>
void* thread_processor_mode2(void* arg)
{
	const double wall_time=get_wall_time();
	parallel_framework_semaphore_t<T>* p_frame=((processor_argument_t<T>*)arg)->p_frame;
	assert(NULL!=p_frame);
	const int pid=((processor_argument_t<T>*)arg)->pid;
	const int inc=p_frame->num_processor;
	const int queue_size=p_frame->queue_size;	
	char thread_name[20];
	snprintf(thread_name,sizeof(thread_name),"processor #%d",pid);

	//int pres;
	int pos=pid;
	for(;;) {
		if(p_frame->status!=parallel_framework_semaphore_t<T>::frame_Running) {
			break;
		}
		while(p_frame->item_status[pos]==parallel_framework_semaphore_t<T>::item_Waiting) {
			p_frame->process_one_item(pos,pid);	
			pos=(pos+inc)%queue_size;
			sem_post(p_frame->sem_ready_cell+pid);
		}
	}
	while(p_frame->item_status[pos]==parallel_framework_semaphore_t<T>::item_Waiting) {
		p_frame->process_one_item(pos,pid);
		pos=(pos+inc)%queue_size;
		sem_post(p_frame->sem_ready_cell+pid);
	}
	//多post一次，用于结束时唤醒consumer
	sem_post(p_frame->sem_ready_cell+pid);
	ul_writelog(UL_LOG_NOTICE,"processor(mode2)#%d  wall time: %lf",pid,get_wall_time()-wall_time);
	return (void*)0;
}


// User defined consumer return: 0=succeed, othe=error
template<typename T>
void* thread_consumer(void* arg)
{
	const double wall_time=get_wall_time();
	double cpu_time=0,pre_cpu=0;
	parallel_framework_semaphore_t<T>* p_frame=(parallel_framework_semaphore_t<T>*)arg;
	//const int queue_size=p_frame->queue_size;
	

	const int inc=p_frame->num_processor;
	for(;;) {
		sem_wait(p_frame->sem_ready_cell+(p_frame->head%inc));
		pre_cpu=get_wall_time();
		if(p_frame->status!=parallel_framework_semaphore_t<T>::frame_Running) {
			break;
		}
		assert(p_frame->item_status[p_frame->head]!=parallel_framework_semaphore_t<T>::item_Waiting);
		p_frame->consume_one_item();
		cpu_time+=get_wall_time()-pre_cpu;
		sem_post(&(p_frame->sem_null_cell));
	}
	while(p_frame->head!=p_frame->tail)
	{
		sem_wait(p_frame->sem_ready_cell+(p_frame->head%inc));
		pre_cpu=get_wall_time();
		assert(p_frame->item_status[p_frame->head]!=parallel_framework_semaphore_t<T>::item_Waiting);
		p_frame->consume_one_item();
		cpu_time+=get_wall_time()-pre_cpu;
	}
	ul_writelog(UL_LOG_NOTICE,"consumer CPU time: %lf, wall time: %lf",cpu_time,get_wall_time()-wall_time);
	return (void*)0;
}
template<typename T>
int parallel_framework_semaphore_t<T>::create_threads()
{
	status=frame_Running;
	for(int i=0;i<num_processor;i++) {
		processor_arg[i].p_frame=this;
		processor_arg[i].pid=i;
		if(_mode!=2) {
			if(0>pthread_create(_processor_tid+i,NULL,thread_processor<T>,(void*)(processor_arg+i))) {
				ul_writelog(UL_LOG_FATAL,"Create processor thread #%d failed",i);
				status=frame_Ready;
				return -1;
			}
		}
		else {
			if(0>pthread_create(_processor_tid+i,NULL,thread_processor_mode2<T>,
														(void*)(processor_arg+i))) {
				ul_writelog(UL_LOG_FATAL,"Create processor thread #%d failed",i);
				status=frame_Ready;
				return -1;
			}
		}
	}
	ul_writelog(UL_LOG_NOTICE,"%d processors [mode:%d] threads are created", num_processor,_mode);
	if(_mode==1) {
		ul_writelog(UL_LOG_NOTICE,"No consumer thread is created for mode 1");
		return 0;
	}
	if(0>pthread_create(&_consumer_tid,NULL,thread_consumer<T>,(void*)this)) {
		ul_writelog(UL_LOG_FATAL,"Create consumer thread failed");
		status=frame_Ready;
		return -1;
	}
	ul_writelog(UL_LOG_NOTICE,"The consumer thread is created");
	return 0;
}
template<typename T>
inline int parallel_framework_semaphore_t<T>::generate_one_item()
{
	int gen_res=_generator(array[tail],tail);
	if(gen_res==1) {
		ul_writelog(UL_LOG_NOTICE,"generator successfully stopped");
	}
	if(gen_res<0) {
		ul_writelog(UL_LOG_FATAL,"generator error, exit unsucceesfully");
	}
	return gen_res;
}
template<typename T>
inline int parallel_framework_semaphore_t<T>::consume_one_item()
{
	if(item_Null==item_status[head]) {
		ul_writelog(UL_LOG_FATAL,"consumer encountered a Null item at %d",head);
		return -1;
	}
	int st=_consumer(array[head],processor_return[head]);
	if(st) {
		ul_writelog(UL_LOG_WARNING,"consumer failed to consume one item at [head:%d] [return:%d].The item is skipped.",head,st);
	}
	else {
		consumer_item_count++;
	}
	item_status[head]=item_Null;
	head=((head+1)==queue_size)?0:head+1;
	return 0;
}
// User defined processor return: 0 = succeed,other = error
template<typename T>
inline int parallel_framework_semaphore_t<T>::process_one_item(int pos,int pid)
{
	int pres;
	if((pres=_processor(array[pos],pid))==0) {
		item_status[pos]=item_Ready;
		processor_item_count[pid]++;
	//	if(0==(processor_item_count[pid]%6371)) {
	//		ul_writelog(UL_LOG_NOTICE,"processor #%d snap head=%d tail=%d pos=%d size=%d rem=%d",pid,head,tail,pos,(tail+queue_size-head)%queue_size,(tail+queue_size-pos)%queue_size);
	//	}
	}
	else {	
		item_status[pos]=item_Error;
	}
	processor_return[pos]=pres;
	return pres;
}

template<typename T>
int parallel_framework_semaphore_t<T>::run(functionT generator,functionT processor,functionC consumer,int mode)
{
	if(mode<0||mode>2) {
		ul_writelog(UL_LOG_FATAL,"mode not in the range [0,2]");
		return -1;
	}
	if(NULL==generator||NULL==processor||NULL==consumer) {
		ul_writelog(UL_LOG_FATAL,"NULL function pointer");
		return -1;
	}
	if(status!=frame_Ready) {
		ul_writelog(UL_LOG_FATAL,
				"Call parallel_framework_semaphore_t::create before calling parallel_framework_t::run");
		return -1;
	}
	_mode=mode;
	_generator=generator;
	_processor=processor;
	_consumer=consumer;
	for(int i=0;i<num_processor;i++) {
		sem_init(sem_waiting_cell+i,0,0);
		sem_init(sem_ready_cell+i,0,0);
	}
	sem_init(&sem_null_cell,0,queue_size-1);
	if(0>create_threads()) return -1;
	int res=0;
	if(mode==0)res=run0();
	else if(mode==1)res=run1();
	else res=run2();
	return res;
}
template<typename T>
int parallel_framework_semaphore_t<T>::run1()
{
	// generator return:  0:succeed, 1:stop, <0:error, exit
	//Start generator
	const double wall_time=get_wall_time();
	double cpu_time=0,pre_cpu=0;
	int gen_res;
	const int inc=num_processor;
	//T* array=queue.array;
	for(;;) {
		pre_cpu=get_wall_time();	
		while(((tail+1)%queue_size)!=head) {
			//队列未满
			gen_res=generate_one_item();
			if(gen_res) {
				goto generator_end;
			}
			item_status[tail]=item_Waiting;
			int pid=tail%num_processor;
			tail=((tail+1)==queue_size)?0:tail+1;
			generator_item_count++;
			sem_post(sem_waiting_cell+pid);
		}
		cpu_time+=get_wall_time()-pre_cpu;
		sem_wait(sem_ready_cell+(head%inc));
		pre_cpu=get_wall_time();
		assert(item_status[head]!=parallel_framework_semaphore_t<T>::item_Waiting);
		consume_one_item();
		cpu_time+=get_wall_time()-pre_cpu;
	}
generator_end:
	status=frame_Ready;
	// generator has stopped
	//用于唤醒所有的processor
	for(int i=0;i<num_processor;i++) {
		sem_post(sem_waiting_cell+i);
	}
	//consumer输出队列中剩余的item
	while(head!=tail) {
		sem_wait(sem_ready_cell+(head%inc));
		pre_cpu=get_wall_time();
		assert(item_status[head]!=parallel_framework_semaphore_t<T>::item_Waiting);
		consume_one_item();
		cpu_time+=get_wall_time()-pre_cpu;
	}
	//回收子线程资源
	void* thread_status;
	for(int i=0;i<num_processor;i++) {
		pthread_join(_processor_tid[i],&thread_status);
		ul_writelog(UL_LOG_NOTICE,"processor #%d exit with %lld and %d items were successfully processed",i,(long long int)(thread_status),processor_item_count[i]);
	}
	ul_writelog(UL_LOG_NOTICE,"%d items were consumed successfully",consumer_item_count);
	ul_writelog(UL_LOG_NOTICE,"generator exit with %d items generated successfully",generator_item_count);
	ul_writelog(UL_LOG_NOTICE,"parallel_framework_semaphore_t::run for [mode:%d] exit!",_mode);

	ul_writelog(UL_LOG_NOTICE,"generator and consumer CPU time: %lf, wall time: %lf",cpu_time,get_wall_time()-wall_time);
	return 0;
}
template<typename T>
int parallel_framework_semaphore_t<T>::run2()
{
	// generator return:  0:succeed, 1:stop, <0:error, exit
	//Start generator
	const double wall_time=get_wall_time();
	double cpu_time=0,pre_cpu=0;
	int gen_res;
	//T* array=queue.array;
	for(;;) {
		sem_wait(&sem_null_cell);
		pre_cpu=get_wall_time();	
		gen_res=generate_one_item();
		if(gen_res!=0) {
			break;
		}
		item_status[tail]=item_Waiting;
		tail=((tail+1)==queue_size)?0:tail+1;
		generator_item_count++;
		cpu_time+=get_wall_time()-pre_cpu;
	}
	ul_writelog(UL_LOG_NOTICE,"generator CPU time: %lf, wall time: %lf",cpu_time,get_wall_time()-wall_time);
	status=frame_Ready;
	void* thread_status;
	for(int i=0;i<num_processor;i++) {
		pthread_join(_processor_tid[i],&thread_status);
		ul_writelog(UL_LOG_NOTICE,"processor #%d exit with %lld and %d items were successfully processed",i,(long long int)(thread_status),processor_item_count[i]);
	}
	pthread_join(_consumer_tid,&thread_status);
	ul_writelog(UL_LOG_NOTICE,"%d items were consumed successfully",consumer_item_count);
	ul_writelog(UL_LOG_NOTICE,"generator exit with %d items generated successfully",generator_item_count);
	ul_writelog(UL_LOG_NOTICE,"parallel_framework_semaphore_t::run for [mode:%d] exit!",_mode);

	return 0;
}
template<typename T>
int parallel_framework_semaphore_t<T>::run0()
{
	// generator return:  0:succeed, 1:stop, <0:error, exit
	//Start generator
	const double wall_time=get_wall_time();
	double cpu_time=0,pre_cpu=0;
	int gen_res;
	//T* array=queue.array;
	for(;;) {
		sem_wait(&sem_null_cell);
		pre_cpu=get_wall_time();	
		gen_res=generate_one_item();
		if(gen_res!=0) {
			break;
		}
#ifdef OUTPUT
		ul_writelog(UL_LOG_NOTICE,"generator generated item #%d head=%d tail=%d",generator_item_count,head,tail);
#endif
		item_status[tail]=item_Waiting;
		int pid=tail%num_processor;
		tail=((tail+1)==queue_size)?0:tail+1;
		generator_item_count++;
		cpu_time+=get_wall_time()-pre_cpu;
		sem_post(sem_waiting_cell+pid);
}
	ul_writelog(UL_LOG_NOTICE,"generator CPU time: %lf, wall time: %lf",cpu_time,get_wall_time()-wall_time);
	status=frame_Ready;
	// generator has stopped
	for(int i=0;i<num_processor;i++) {
		sem_post(sem_waiting_cell+i);
	}
	void* thread_status;
	for(int i=0;i<num_processor;i++) {
		pthread_join(_processor_tid[i],&thread_status);
		ul_writelog(UL_LOG_NOTICE,"processor #%d exit with %lld and %d items were successfully processed",i,(long long int)(thread_status),processor_item_count[i]);
	}
	pthread_join(_consumer_tid,&thread_status);
	ul_writelog(UL_LOG_NOTICE,"%d items were consumed successfully",consumer_item_count);
	ul_writelog(UL_LOG_NOTICE,"generator exit with %d items generated successfully",generator_item_count);
	ul_writelog(UL_LOG_NOTICE,"parallel_framework_semaphore_t::run for [mode:%d] exit!",_mode);

	return 0;
}
#endif
