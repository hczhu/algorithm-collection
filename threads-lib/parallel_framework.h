#ifndef __IM_PARALLEL_FRAMEWORK_HEADER__
#define __IM_PARALLEL_FRAMEWORK_HEADER__
//#include "com_log.h"
#include <cassert>
#include <pthread.h>
#include "common_header_zhc.h"
//#define OUTPUT
//#define MORE
//#define CONSUMER_OUTPUT
template<typename T>
struct circular_queue_t
{
	enum item_status_t{
		Null,  // No item here
		Waiting,  // Wait for processors
		Ready,  // wait for the consumer
		Error  //process 处理出错的item
		};
	T* array;
	item_status_t *item_status;
	int cap,head,tail;
	public:
	circular_queue_t():array(NULL),item_status(NULL),cap(0),head(0),tail(0){};
	~circular_queue_t(){if(NULL!=array)delete [] array;if(NULL!=item_status)delete [] item_status;};
	int create(int n);
	inline bool is_full()const{return ((tail+1)%cap)==head;};
	inline bool not_full(){return ((tail+1)%cap)!=head;};
	inline bool is_empty()const{return head==tail;};
	inline bool has_item()const{return head!=tail;};
	//inline void enqueue(const T& item)
	//		{array[tail]=item;status[tail]=Waiting;tail=(tail+1)%cap;item_count++;};
	inline void enqueue(){item_status[tail]=Waiting;tail=(tail+1)%cap;};
	inline void dequeue(){item_status[head]=Null;head=(head+1)%cap;};
//	inline T& front(){return array[head];};
	inline item_status_t front_item_status(){return item_status[head];};
	inline bool valid(int pos){return ((tail+cap-head)%cap)>((pos+cap-head)%cap);};
	void clear()
	{
		if(cap==0)return;
		head=tail;
		for(int i=0;i<cap;i++)item_status[i]=Null;
	};
};
template<typename T>
int circular_queue_t<T>::create(int n)
{
	if(n<=1) {
		ul_writelog(UL_LOG_FATAL,"Illegal queue capacity %d",n);
		return -1;
	}
	if(NULL!=array) delete [] array;
	if(NULL!=item_status) delete [] item_status;
	circular_queue_t<T>();
	try
	{
		array=new T[n];
		item_status=new item_status_t[n];	
	}
	catch(std::bad_alloc)
	{
		ul_writelog(UL_LOG_FATAL,"Allocating memory failed in circular_queue_t::create failed");
		if(NULL!=array) delete [] array;
		return -1;
	}
	cap=n;
	head=tail=0;
	for(int i=0;i<cap;i++)item_status[i]=Null;
	return 0;
}

template<typename T> struct parallel_framework_t;
template<typename T>
struct processor_argument_t
{
	parallel_framework_t<T>* p_frame;
	int pid;
};

//******************************************************************

template<typename T>
struct parallel_framework_t
{
	enum framework_status_t{
		Null,  // wait for being created
		Ready,  // Have been created
		Running  //Running
		};
	enum framework_running_mode_t{

	};
	typedef int (*functionT)(T&,uint32_t);
	typedef int (*functionC)(T&,int);

	circular_queue_t<T> queue;
	pthread_t *_processor_tid,_consumer_tid;
	
	functionT _generator;
	functionC _consumer;
	functionT _processor;

	framework_status_t status;
	int num_processor,queue_size;
	int generator_item_count,consumer_item_count;
	int *processor_item_count;
private:
	processor_argument_t<T> *processor_arg;
	int mode;
	int* processor_return;
	int create_threads();
	inline int stop();
	int run0();// num_processor processor threads, 1 thread for generator, 1 thread for consumer
	int run1();// num_processor processor threads, 1 threads for both generator and consumer
	int run2(); // num_processor processor threads, 1 threads for generator, consumer and an extra processor
public:
	inline int generate_one_item();
	inline int consume_one_item();
	inline int process_one_item(int,int);

	inline int get_mode()const{return mode;};
	parallel_framework_t():_processor_tid(NULL),_generator(NULL),_consumer(NULL),_processor(NULL),
							status(Null),num_processor(0),queue_size(0),generator_item_count(0),
							consumer_item_count(0),processor_item_count(NULL),processor_arg(NULL),
							mode(0),processor_return(NULL){};
	~parallel_framework_t()
	{
		if(NULL!=_processor_tid)delete [] _processor_tid;
		if(NULL!=processor_item_count)delete [] processor_item_count;
		if(NULL!=processor_arg)delete [] processor_arg;
		if(NULL!=processor_return)delete [] processor_return;
	};
	int create(int _num_processor,int _buf_size,int _mode=0);
	// generator return:  0:succeed, 1:try again later, 2:stop, -1:error
	int run(functionT generator,functionT processor,functionC consumer);

};
template<typename T>
int parallel_framework_t<T>::create(int _num_processor,int _buf_size,int _mode)
{
	if(_num_processor<0||_buf_size<=0||_mode<0||_mode>2) {
		ul_writelog(UL_LOG_FATAL,"Incorrect arguments [num_processor:%d] [buf_size:%d] [mode:%d]",_num_processor,_buf_size,_mode);
		return -1;
	}
	if(_mode==2)_num_processor--;
	if(_num_processor==0&&_mode!=2) {
		ul_writelog(UL_LOG_FATAL,"Incorrect argument combination: [num_processor:%d] [mode:%d]",_num_processor,_mode);
		return -1;
	}
	if(NULL!=_processor_tid)delete [] _processor_tid;
	if(NULL!=processor_item_count)delete [] processor_item_count;
	if(NULL!=processor_arg)delete [] processor_arg;
	if(NULL!=processor_return)delete [] processor_return;
	processor_item_count=NULL;
	status=Null;
	_processor_tid=NULL;
	processor_arg=NULL;
	try
	{
		processor_item_count=new int[_num_processor+1];
		_processor_tid=new pthread_t[_num_processor];
		processor_arg=new processor_argument_t<T>[_num_processor];
		processor_return=new int[(_num_processor+(_mode==2?1:0))*_buf_size];
	}
	catch(std::bad_alloc)
	{
		if(NULL!=_processor_tid)delete [] _processor_tid;
		if(NULL!=processor_item_count)delete [] processor_item_count;
		if(NULL!=processor_arg)delete [] processor_arg;
		if(NULL!=processor_return)delete [] processor_return;
		ul_writelog(UL_LOG_FATAL,"allocating memory failed in parallel_framework_t::create");
		return -1;
	}
	if(queue.create((_num_processor+(_mode==2?1:0))*_buf_size))
	{
		delete [] _processor_tid;
		delete [] processor_item_count;
		delete [] processor_arg;
		return -1;
	}
	num_processor=_num_processor;
	mode=_mode;
	queue_size=queue.cap;
	status=Ready;
	generator_item_count=consumer_item_count=0;
	memset(processor_item_count,0,sizeof(int)*(num_processor+1));
	ul_writelog(UL_LOG_NOTICE,"parallel_framework_t::create succeeded with [num_processor:%d] [buf_size:%d] [mode:%d] [queue_size:%d]",_num_processor,_buf_size,_mode,queue_size);
	return 0;
}
// processor return: 0:succeed,-1:error, other:fatal error and the thread must exit, 
template<typename T>
void* thread_processor(void* arg)
{
	const double wall_time=get_wall_time();
	const double clock_time=get_clock_time();
	parallel_framework_t<T>* p_frame=((processor_argument_t<T>*)arg)->p_frame;
	assert(NULL!=p_frame);
	assert(NULL!=p_frame->queue.item_status);
	
	typename circular_queue_t<T>::item_status_t* status=p_frame->queue.item_status;

	const int pid=((processor_argument_t<T>*)arg)->pid;
	const int inc=p_frame->num_processor+(p_frame->get_mode()==2?1:0);
	const int queue_size=p_frame->queue_size;
	char thread_name[20];
	snprintf(thread_name,sizeof(thread_name),"processor #%d",pid);
	//int pres;
	int pos=pid;
	assert(p_frame->queue.head<=pos);
	for(bool go=true;go;) {
		if(p_frame->status!=parallel_framework_t<T>::Running)go=false;	
		for(;circular_queue_t<T>::Waiting==status[pos];pos=(pos+inc)%queue_size) {
			p_frame->process_one_item(pos,pid);	
		}
	}
	ul_writelog(UL_LOG_NOTICE,"processor #%d clock time: %lf, wall time: %lf",pid,get_clock_time()-clock_time,get_wall_time()-wall_time);
	return (void*)0;
}

// consumer return: 0:succeed, othe:error
template<typename T>
void* thread_consumer(void* arg)
{
	const double wall_time=get_wall_time();
	const double clock_time=get_clock_time();

	parallel_framework_t<T>* p_frame=(parallel_framework_t<T>*)arg;
	circular_queue_t<T>& queue=p_frame->queue;
	
	//int (*consumer)(T&)=p_frame->_consumer;
	//typename parallel_framework_t<T>::functionT consumer=p_frame->_consumer;
	for(;p_frame->status==parallel_framework_t<T>::Running||queue.has_item();) {
		while(queue.has_item()&&p_frame->consume_one_item());
	}
	ul_writelog(UL_LOG_NOTICE,"consumer clock time: %lf, wall time: %lf",get_clock_time()-clock_time,get_wall_time()-wall_time);
	return (void*)0;
}
template<typename T>
int parallel_framework_t<T>::create_threads()
{
	status=Running;
	for(int i=0;i<num_processor;i++) {
		processor_arg[i].p_frame=this;
		processor_arg[i].pid=i;
		if(0>pthread_create(_processor_tid+i,NULL,thread_processor<T>,(void*)(processor_arg+i))) {
			ul_writelog(UL_LOG_FATAL,"Create processor thread #%d failed",i);
			status=Ready;
			return -1;
		}
	}
	ul_writelog(UL_LOG_NOTICE,"%d processors threads are created", num_processor);

	if(mode==0) {
		if(0>pthread_create(&_consumer_tid,NULL,thread_consumer<T>,(void*)this)) {
			ul_writelog(UL_LOG_FATAL,"Create consumer thread failed");
			status=Ready;
			return -1;
		}
		ul_writelog(UL_LOG_NOTICE,"The consumer thread is created for mode 0");
	}
	else {
		ul_writelog(UL_LOG_NOTICE,"No consumer thread is created for mode %d",mode);
	}
	return 0;
}
template<typename T>
inline int parallel_framework_t<T>::generate_one_item()
{
	int gen_res=_generator(queue.array[queue.tail],queue.tail);
	if(gen_res==1) {
		ul_writelog(UL_LOG_NOTICE,"generator successfully stopped");
	}
	if(gen_res<0) {
		ul_writelog(UL_LOG_FATAL,"generator error, exit unsucceesfully");
	}
	return gen_res;
}
template<typename T>
inline int parallel_framework_t<T>::consume_one_item()
{
	if(queue.front_item_status()!=circular_queue_t<T>::Waiting) {
		if(0==_consumer(queue.array[queue.head],processor_return[queue.head])) {
			consumer_item_count++;
		}	
		queue.dequeue();
	}
	return 0;
}
template<typename T>
inline int parallel_framework_t<T>::process_one_item(int pos,int pid)
{
	int pres;
	if((pres=_processor(queue.array[pos],pid))==0) {
		queue.item_status[pos]=circular_queue_t<T>::Ready;
		processor_item_count[pid]++;
	}
	else {	
		queue.item_status[pos]=circular_queue_t<T>::Error;
	}
	processor_return[pos]=pres;
	return pres;
}

template<typename T>
int parallel_framework_t<T>::run(functionT generator,functionT processor,functionC consumer)
{
	if(NULL==generator||NULL==processor||NULL==consumer) {
		ul_writelog(UL_LOG_FATAL,"NULL function pointer");
		return -1;
	}
	if(status!=Ready) {
		ul_writelog(UL_LOG_FATAL,
							"Call parallel_framework_t::create before calling parallel_framework_t::run");
		return -1;
	}
	if(mode<0||mode>2) {
		ul_writelog(UL_LOG_FATAL,"Illegal [mode:%d]",mode);
		return -1;
	}
	_generator=generator;
	_processor=processor;
	_consumer=consumer;
	if(0>create_threads()) return -1;
	int res=0;
	if(mode==0)res=run0();
	else if(mode==1)res=run1();
	else if(mode==2)res=run2();
	stop();
	return res;
}
template<typename T>
int parallel_framework_t<T>::run0()
{
	// generator return:  0:succeed, 1:stop, <0:error, exit
	int gen_res;
	for(;;) {
		while(queue.is_full());
		gen_res=generate_one_item();
		if(gen_res)break;
		generator_item_count++;
		queue.enqueue();
	}
	return 0;
}
template<typename T>
int parallel_framework_t<T>::run1()
{
	int gen_res;
	bool generator_running=true;
	for(;generator_running||queue.has_item();) {
		if(generator_running&&queue.not_full()) {
			gen_res=generate_one_item();
			if(gen_res) {
				generator_running=false;
			}
			else {
				queue.enqueue();
				generator_item_count++;
			}
		}
		if(queue.has_item()) {
			consume_one_item();
		}
	}
	return 0;
}
template<typename T>
int parallel_framework_t<T>::run2()
{

	const double wall_time=get_wall_time();
	const double clock_time=get_clock_time();

	int gen_res;
	bool generator_running=true;
	const int inc=num_processor+1;
	for(;generator_running||queue.has_item();) {
		if(generator_running&&queue.not_full()) {
			gen_res=generate_one_item();
			if(gen_res) {
				generator_running=false;
			}
			else {
				queue.enqueue();
				generator_item_count++;
			}
		}
		if(queue.has_item()) {
			if((queue.head%inc)==num_processor) {
				process_one_item(queue.head,num_processor);
			}
			consume_one_item();
		}
	}
	ul_writelog(UL_LOG_NOTICE,"generator clock time: %lf, wall time: %lf",get_clock_time()-clock_time,get_wall_time()-wall_time);
	return 0;
}

template <typename T>
inline int parallel_framework_t<T>::stop()
{
	status=Ready;
	// generator has stopped
	void* thread_status;
	for(int i=0;i<num_processor;i++) {
		pthread_join(_processor_tid[i],&thread_status);
		ul_writelog(UL_LOG_NOTICE,"processor #%d exit with %lld and %d items were successfully processed",i,(long long int)(thread_status),processor_item_count[i]);
	}
	if(mode==0) {
		pthread_join(_consumer_tid,&thread_status);
	}
	ul_writelog(UL_LOG_NOTICE,"%d items were consumed successfully",consumer_item_count);
	ul_writelog(UL_LOG_NOTICE,"generator exit with %d items generated successfully",generator_item_count);
	ul_writelog(UL_LOG_NOTICE,"parallel_framework_t::run exit!");
	return 0;
}
#endif
