/*
 *
 *
 *
 *
 *
*/
#ifndef __MEM_POOL_HEADER__
#define __MEM_POOL_HEADER__

#include "data_structures_common.h"
#include "im_vector.h"
#include <algorithm>
#include <cassert>
//using namespace std;


namespace Im_mem_pool
{
const int WIDTH1 = ADDRESS_TYPE_BIT_WIDTH;
const int WIDTH2 = 13;//后面两个域指向二维数组里的一个元素
const int WIDTH3 = 32-WIDTH1-WIDTH2;

const uint32_t MASK1 = ((uint32_t)((1<<WIDTH1) -1))<<(WIDTH2+WIDTH3);//析取三个值的掩码
const uint32_t MASK2 = ((1<<WIDTH2) -1)<<(WIDTH3);
const uint32_t MASK3 = (1<<WIDTH3) - 1;
const uint32_t POOL_DETECT_BUF_LEN = 150;

inline uint32_t min(const uint32_t a, const uint32_t b)
{
	return a<b?a:b;
}

inline uint32_t max(const uint32_t a, const uint32_t b)
{
	return a>b?a:b;
}

// 注意这个pool没有脏数据保护
class fixed_size_mem_pool_t
{
private:
	char ** block_array;
	
	static const uint32_t BLOCK_NUM = 1<<WIDTH2;
	static const uint32_t MIN_ITEM_SIZE = sizeof(uint32_t);
	
	uint32_t alloc_num;
	uint32_t block_size;
	uint32_t avail_num;
	uint32_t free_list;
	uint32_t item_size; 
	
	uint32_t mem_consume;
	uint32_t free_mem;
		
	inline int new_block()
	{
		if (alloc_num < BLOCK_NUM)
		{
			char* new_block=NULL;
			// 8字节对齐
			if(0==(item_size&7)) {
				new_block=(char*)(new(std::nothrow) long long int[block_size*(item_size>>3)]);
			}
			//4字节对齐
			else if(0==(item_size&3)) {
				new_block=(char*)(new(std::nothrow) int[block_size*(item_size>>2)]);
			}
			//2字节对齐
			else if(0==(item_size&1)) {
				new_block=(char*)(new(std::nothrow) short[block_size*(item_size>>1)]);
			}
			else {
				new_block=(new(std::nothrow) char[block_size*item_size]);
			}
			block_array[alloc_num++] = new_block;//new(std::nothrow) char[block_size * item_size];
			if (block_array[alloc_num-1] == NULL)
			{
				alloc_num --;
				ul_writelog(UL_LOG_FATAL, "fail to new block_array[%d]", alloc_num-1);
				return -1;
			}
			
			mem_consume += block_size * item_size;	
			return 0;
		}
		else
		{
			return -1;
		}
	}

public:
	fixed_size_mem_pool_t():block_array(NULL),alloc_num(0){};
	inline uint32_t get_mem_consume()
	{
		return mem_consume;
	}
	inline uint32_t get_free_mem()
	{		
		return free_mem + avail_num*item_size;
	}
	
	inline char * real_address(const Vaddress32 p)
	{			
		uint32_t idx1 = (p & MASK2)>>WIDTH3;
		uint32_t idx2 = (p & MASK3)*item_size;
		if (idx1 >= alloc_num || idx2 >= block_size*item_size)
		{
			ul_writelog(UL_LOG_FATAL, "invalid ptr[%u]", p);
			return NULL;
		}
		else
		{	
			return (block_array[idx1]+idx2);
		}
	}

	int create(const uint32_t block_size_t = 1<<WIDTH3, const uint32_t item_size_t = MIN_ITEM_SIZE)
	{
		mem_consume = 0;
		free_mem = 0;

		block_size = min(block_size_t, 1<<WIDTH3);
		block_size = max(block_size, 1);
		
		item_size = max(MIN_ITEM_SIZE, item_size_t);
			
		block_array = new(std::nothrow) char*[BLOCK_NUM];
		if (NULL == block_array)
		{
			ul_writelog(UL_LOG_FATAL, "fail to new block_array");
			return -1;
		}
		
		mem_consume += sizeof(char *) * BLOCK_NUM;	
		
		alloc_num = 0;
		avail_num = 0;

		free_list = 0;

		return 0;
	}

	~fixed_size_mem_pool_t()
	{			
		for (uint32_t i=0; i<alloc_num; i++)
		{
			delete[] block_array[i];
		}
		delete[] block_array;
	}

	inline Vaddress32 fetch()
	{
		if (free_list)
		{
			free_mem--;
			uint32_t ret = free_list;
			free_list = ((uint32_t *)real_address(free_list))[0];
			//前提是每个数据项的大小必须大于等于四个字节
			return ret;
		}
		else
		{
			while (avail_num == 0)
			{
				if (new_block() < 0)
				{
					ul_writelog(UL_LOG_FATAL, "out of memory");
					return 0;					
				}
				avail_num = block_size;
				if (alloc_num == 1)
				{
					avail_num--;//第一个块的第一个地址是不能用的
				}
			}
			avail_num --;
			return (alloc_num-1) << WIDTH3 | (block_size-avail_num-1);
		}		
	}
	
	inline void back(const uint32_t p)
	{				
		char *src = real_address(p);
		if (src != NULL)
		{
			free_mem += item_size;
			((uint32_t*)real_address(p))[0]=free_list,
			free_list = p;
		}
		else
		{
			ul_writelog(UL_LOG_FATAL, "back invalid ptr[%u]", p);
		}
	}
};

class mem_pool_t
{
	private:
		// 地址空间 0xf0000000 ~ 0xffffffff 留作他用
		static const int MAX_TYPE_NUM = ((uint32_t)1)<<WIDTH1;  // 15;
		static const int DFT_DIRTY_ARRAY_SIZE=10000;
		
		fixed_size_mem_pool_t* mp[MAX_TYPE_NUM];
		uint32_t fixed_size_mem_pool_num;
		uint32_t fixed_size_array[MAX_TYPE_NUM];		
		int sorted_fixed_size_array_index[MAX_TYPE_NUM];
		im_vector_t<Vaddress32>* m_p_dynamic_dirty_array;
		Vaddress32  dirty_list;		
		uint32_t dirty_list_length;
		long total_used_length,total_allocated;
		
			
	public:
		//增加一个请求的固定长度，必须在已经调用过create的情况下调用
		//uniq为真表示如果已经存在同样长度的fixed_size_mem_pool则不添加新的fixed_size_mem_pool
		int add_fixed_size_pool(uint32_t size,bool uniq=true,uint32_t block_size = 1<<WIDTH3)
		{
			if(NULL==m_p_dynamic_dirty_array) {
				ul_writelog(UL_LOG_FATAL,"Plaese call mem_pool_t::create first");
				return -1;
			}
			size=max(size,4);
			if(uniq) {
				//去重
				for(int i=0;i<fixed_size_mem_pool_num;i++) {
					if(size==fixed_size_array[i]) {
						return 0;
					}
				}
			}
			if(fixed_size_mem_pool_num>=MAX_TYPE_NUM) {
				ul_writelog(UL_LOG_FATAL,"fixed_size_mem_pool_num reaches the upper bound [%d]",
					MAX_TYPE_NUM);
				return -1;
			}
			int i=fixed_size_mem_pool_num;
			//printf("add i=%d size=%d\n",i,size);
			mp[i] = new(std::nothrow) fixed_size_mem_pool_t();
			if (mp[i] == NULL)
			{
				ul_writelog(UL_LOG_FATAL, "fail to new fixed_size_mem_pool");
				return -1;
			}
			
			if(mp[i]->create(block_size, size) ) {
				delete mp[i];
				ul_writelog(UL_LOG_FATAL, "fail to create fixed_size_mem_pool for bloack size=%u",
						fixed_size_array[i]);
					return -1;
			}
			fixed_size_array[fixed_size_mem_pool_num]=size;		
			sorted_fixed_size_array_index[fixed_size_mem_pool_num]=fixed_size_mem_pool_num;
			for(int j=fixed_size_mem_pool_num;j>0;j--) {
				if(fixed_size_array[sorted_fixed_size_array_index[j]]<
					fixed_size_array[sorted_fixed_size_array_index[j-1]]) {
					std::swap(sorted_fixed_size_array_index[j],sorted_fixed_size_array_index[j-1]);
				}
				else {
					break;
				}
			}
			fixed_size_mem_pool_num++;
			return 0;
		}
		mem_pool_t():m_p_dynamic_dirty_array(NULL),dirty_list(NULL_VADDRESS32)
		{
			fixed_size_mem_pool_num=0;
			for(int i=0;i<MAX_TYPE_NUM;i++) {
				mp[i]=NULL;
			}
		}
		bool is_valid_type(Vaddress32 addr)
		{
			return get_vaddr_type(addr)<fixed_size_mem_pool_num;
		}
		//取等对应size内存块的type	
		inline uint32_t get_corresponding_type(uint32_t size)
		{
			if(fixed_size_mem_pool_num==0) {
				ul_writelog(UL_LOG_FATAL, "No fixed_size_mem_pool was created");
				return MAX_TYPE_NUM;
			}
			if (size > fixed_size_array[sorted_fixed_size_array_index[fixed_size_mem_pool_num-1]])
			{
				ul_writelog(UL_LOG_FATAL, "[size:%u] is too large in mem_pool_t::get_vaddr_type", size);
				return MAX_TYPE_NUM;
			}
			
			total_used_length+=size;
			
			Vaddress32 ret = NULL_VADDRESS32;
			
			for (uint32_t i=0; i<fixed_size_mem_pool_num; i++) {
				int idx=sorted_fixed_size_array_index[i];
				if (fixed_size_array[idx] >= size) {
					return idx;
				}
			}
			return MAX_TYPE_NUM;
		}
		//申请一个已知type的内存块
		Vaddress32 malloc_fixed_size(uint32_t type) 
		{
			if(type>=fixed_size_mem_pool_num) {
				ul_writelog(UL_LOG_FATAL,"[type:%u] exceeds the current [fixed_size_mem_pool_num:%u]",
						type,fixed_size_mem_pool_num);
				return NULL_VADDRESS32;
			}
			Vaddress32 ret=mp[type]->fetch();
			if(ret) {
				total_allocated+=fixed_size_array[type];
				ret|=(type<<(WIDTH2+WIDTH3));
			}
			return ret;
		}
		inline Vaddress32 malloc(uint32_t size)
		{
			if(fixed_size_mem_pool_num==0) {
				ul_writelog(UL_LOG_FATAL, "No fixed_size_mem_pool was created");
				return NULL_VADDRESS32;
			}
			if (size > fixed_size_array[sorted_fixed_size_array_index[fixed_size_mem_pool_num-1]])
			{
				ul_writelog(UL_LOG_FATAL, "Allocating [size:%u] is too large in mem_pool_t::malloc", size);
				return NULL_VADDRESS32;
			}
			
			total_used_length+=size;
			
			Vaddress32 ret = NULL_VADDRESS32;
			
			for (uint32_t i=0; i<fixed_size_mem_pool_num; i++)
			{
				int idx=sorted_fixed_size_array_index[i];
				if (fixed_size_array[idx] >= size)
				{
					ret = mp[idx]->fetch();
					if (ret)
					{
						total_allocated+=fixed_size_array[idx];
						ret |= (idx<<(WIDTH2+WIDTH3));
						break;
					}
					//这里还是可能会失败的，可能某种类型不够
				}
			}			
			return ret;
		}
		
		// 四个传出参数的意义列举如下：
		// tot_used: 所有正在被使用的内存字节数，不包括被保护的脏数据
		// tot_allocated: 已分配出的内存字节数，不包括被保护的脏数据，等于tot_used加上浪费的字节数目
		// tot_dirty: 被保护的脏数据内存块数目
		// tot_mem: 内存池向操作系统申请的内存总数
		// tot_free: 当前可用于分配的内存块数
		void get_mem_consume(long & tot_used, long& tot_allocated,long & tot_dirty, 
									long & tot_mem, long & tot_free)
		{
			tot_used = total_used_length;
			tot_allocated=total_allocated;	

			tot_dirty =( dirty_list_length +m_p_dynamic_dirty_array->size());
			
			tot_mem = 0;
			tot_free = 0;
			for (uint32_t i=0; i<fixed_size_mem_pool_num; i++)
			{
				tot_mem += mp[i]->get_mem_consume();
				tot_free += mp[i]->get_free_mem();
			}			
		}
		
		void get_detect_info(char * ret, int max_len)
		{
			if (ret != NULL)
			{
				long tot_used = 0;
				long tot_allocated = 0;
				long tot_dirty = 0;
				long tot_mem = 0;
				long tot_free = 0;
				get_mem_consume(tot_used,tot_allocated, tot_dirty, tot_mem, tot_free);
				snprintf(ret, max_len , "total used: %ld;total allocated : %ld;total dirty size: %ld;\
total mem consume : %ld;total free space : %ld", 
							tot_used,tot_allocated, tot_dirty, tot_mem, tot_free);
			}
		}

		int create(const uint32_t block_size = 1<<WIDTH3,
			const uint32_t dynamic_dirty_array_size =DFT_DIRTY_ARRAY_SIZE )
		{
			uint32_t lens[MAX_TYPE_NUM];
			return create(lens,0,block_size,dynamic_dirty_array_size);
		}
		

		int create(const uint32_t * lens, const uint32_t n, 
			const uint32_t block_size = 1<<WIDTH3,
			const uint32_t dynamic_dirty_array_size =DFT_DIRTY_ARRAY_SIZE )
		{
			if (NULL == lens)
			{
				ul_writelog(UL_LOG_FATAL, "NULL input");
				return -1;
			}

			total_allocated=total_used_length = 0;
				
			uint32_t i;
			for (i=0; i<n; i++)
			{
				fixed_size_array[i] = max(lens[i],4);				
			}
			std::sort(fixed_size_array, fixed_size_array+n);//这里不负责去重
			fixed_size_mem_pool_num=(uint32_t)(std::unique(fixed_size_array,fixed_size_array+n)-fixed_size_array);
			if(fixed_size_mem_pool_num>MAX_TYPE_NUM) {
				ul_writelog(UL_LOG_FATAL,"[fixed_size_mem_pool_num:%d] goes beyond the upper bound \
				[MAX_TYPE_NUM:%d]",fixed_size_mem_pool_num,MAX_TYPE_NUM);
				return -1;
			}
			for (i=0; i<fixed_size_mem_pool_num; i++)
			{
				mp[i] = new(std::nothrow) fixed_size_mem_pool_t();
				if (mp[i] == NULL)
				{
					ul_writelog(UL_LOG_FATAL, "fail to new fixed_size_mem_pool");
					return -1;
				}
				
				if(mp[i]->create(block_size, fixed_size_array[i]) ) {
					ul_writelog(UL_LOG_FATAL, "fail to create fixed_size_mem_pool for bloack size=%u",fixed_size_array[i]);
					return -1;
				}
			}
			m_p_dynamic_dirty_array=new(std::nothrow) im_vector_t<Vaddress32>(0xfffffff);
			if(NULL==m_p_dynamic_dirty_array) {
				ul_writelog(UL_LOG_FATAL, "fail to new dynamic_dirty_array");
				return -1;
			}
			if (m_p_dynamic_dirty_array->create(dynamic_dirty_array_size))
			{
				ul_writelog(UL_LOG_FATAL, "fail to create dynamic_dirty_array of initial size %u",dynamic_dirty_array_size);
				return -1;
			}		
			dirty_list=NULL_VADDRESS32;
			dirty_list_length=0;
			
			return 0;
		}

		~mem_pool_t()
		{	
			for (uint32_t i=0; i<fixed_size_mem_pool_num; i++)
			{
				delete mp[i];
			}
					
			delete m_p_dynamic_dirty_array;
		}
		// tail_writeable为真表示free的内存块的最后四个字节不需要保护
		inline int free(Vaddress32 p,int used_length=-1,
				bool protect_dirty=true,bool tail_writeable=false)
		{
			if (p == NULL_VADDRESS32)
			{
				return -1;
			}
			uint32_t type=0;
			if((type=get_vaddr_type(p))>=fixed_size_mem_pool_num) 
			{
				ul_writelog(UL_LOG_FATAL,"Free invalid Vaddress32 %u with type %u",p,get_vaddr_type(p));
				return 0;
			}
			if(used_length==-1) {
				used_length=fixed_size_array[type];
			}
			const uint32_t size=fixed_size_array[type];
			total_used_length-=used_length;
			total_allocated-=size;
			if(!protect_dirty) 
			{
				mp[type]->back(p);
				return 0;
			}
			if(tail_writeable||(uint32_t)used_length+4<=size) {
				assert(size>=4);
				*((Vaddress32*)(mp[type]->real_address(p)+size-4))=dirty_list;
				dirty_list=p;
				dirty_list_length++;
				return 0;
			}
			if(m_p_dynamic_dirty_array->push_back(p)) {
				ul_writelog(UL_LOG_FATAL,"Failed to push_back dirty Vaddress32 to dynamic_dirty_array with dynamic_dirty_array size=%u",m_p_dynamic_dirty_array->size());
				return -1;
			}
			return 0;
		}

		void recycle()
		{
			while(dirty_list) {
				uint32_t type=get_vaddr_type(dirty_list);
				Vaddress32 next=*((Vaddress32*)(mp[type]->real_address(dirty_list)+
												fixed_size_array[type]-4));
				mp[type]->back(dirty_list);
				dirty_list=next;
				dirty_list_length--;
			}
			assert(0==dirty_list_length);
			uint32_t vsize=m_p_dynamic_dirty_array->size();
			for(uint32_t i=0;i<vsize;i++) {
				mp[get_vaddr_type((*m_p_dynamic_dirty_array)[i])]->back((*m_p_dynamic_dirty_array)[i]);
			}
			m_p_dynamic_dirty_array->clear();
		}
			
		inline char * real_address(Vaddress32 p)
		{	
			if (NULL_VADDRESS32 == p)
			{
				return NULL;
			}
			else
			{	
				uint32_t type = get_vaddr_type(p);
				if (type >= fixed_size_mem_pool_num)
				{
					ul_writelog(UL_LOG_FATAL, "invalid Vaddress32[%u]", p);
					return NULL;
				}
				else
				{
					return mp[type]->real_address(p);
				}
			}
		}
};

void mem_pool_tester()
{
	char buf[1000];
	uint32_t lens[]={1,2,4,9,14,19,24,29,29,24};
	const int N=5;
	const char name[N][24]={"zhuhongcheng","zhuhong","chengzhuh","hongchengzhuhongcheng","abcdefghi"};
	Vaddress32 vaddr[N];
	mem_pool_t pool;
	int res=pool.create();
	for(int i=0;i<10;i++)
	{
		int res=pool.add_fixed_size_pool(lens[i],false);
		assert(0==res);
	}
	assert(0==res);
	for(int cc=0;cc<5;cc++)
	{
		int res=pool.add_fixed_size_pool(cc+5);
		assert(0==res);
	long used=0,allocated=0;
	for(int i=0;i<N;i++)
	{
		used+=strlen(name[i])+1;
		for(int j=0;j<8;j++)
		{
			if(lens[j]>=strlen(name[i])+1)
			{
				allocated+=lens[j];
				break;
			}
		}
		vaddr[i]=pool.malloc(strlen(name[i])+1);
		assert(vaddr[i]!=NULL_VADDRESS32);
		strcpy(pool.real_address(vaddr[i]),name[i]);
	}

	for(int i=0;i<N;i++)
		assert(0==strcmp(pool.real_address(vaddr[i]),name[i]));

	long a,b,c,d,e;
	pool.get_mem_consume(a,b,c,d,e);
//	pool.get_detect_info(buf,sizeof(buf));
//	printf("%s\n",buf);
	assert(a==used);
//	assert(b==allocated);
	for(int i=0;i<N;i++)
		pool.free(vaddr[i],strlen(name[i])+1);//,printf("%s\n",pool.real_address(vaddr[i]));
	
	pool.get_mem_consume(a,b,c,d,e);
	assert(0==a);
	assert(0==b);
	for(int i=0;i<N;i++)
		assert(0==strcmp(pool.real_address(vaddr[i]),name[i]));

	pool.recycle();
	}
	struct Node
	{
		int a,b;
		char c;
		short d;
		int e;
	};
	Node na,nb,nc;
	na.a=1;na.b=2;
	na.c='a';na.d=-1;
	na.e=-2;
	Vaddress32 p=pool.malloc(sizeof(Node));
	memcpy(pool.real_address(p),&na,sizeof(Node));

	pool.free(p,sizeof(Node),true,true);

	assert(((Node*)pool.real_address(p))->a==1);
	assert(((Node*)pool.real_address(p))->b==2);
	assert(((Node*)pool.real_address(p))->c=='a');
	assert(((Node*)pool.real_address(p))->d==-1);
	//assert(((Node*)pool.real_address(p))->e==-2);
	res=pool.add_fixed_size_pool(sizeof(Node));
	assert(res==0);
	res=pool.add_fixed_size_pool(sizeof(Node)+14);
	assert(res==0);
	im_vector_t<Vaddress32> have(0x7fffffff);
	have.create(10);
	Node model;
	memset(&model,0x15,sizeof(Node));
	const uint32_t type=pool.get_corresponding_type(sizeof(Node));
	assert(type<15);
	for(int i=0;i<10000000;i++) {
		if(have.size()&&0==(rand()%3)) {
			int pos=rand()%have.size();
			if(have[pos]!=NULL_VADDRESS32) {
				assert(0==memcmp(&model,pool.real_address(have[pos]),sizeof(Node)));
				pool.free(have[pos],sizeof(Node)),have[pos]=NULL_VADDRESS32;
			}
		}
		else {
			Vaddress32 addr;
			if(1&rand()) {
				addr=(rand()&1)?pool.malloc(sizeof(Node)):pool.malloc_fixed_size(type);
				assert(type==get_vaddr_type(addr));
			}
			else {
				addr=pool.malloc(sizeof(Node)+10);
				assert(NULL_VADDRESS32!=addr);
			}
			memcpy(pool.real_address(addr),&model,sizeof(Node));
			int res=have.push_back(addr);
			assert(res==0);
		}
	}
	for(int i=0;i<have.size();i++) {
		if(NULL_VADDRESS32!=have[i]) {
			assert(0==memcmp(&model,pool.real_address(have[i]),sizeof(Node)));
			if(rand()&1)res=pool.free(have[i],sizeof(Node)),have[i]=NULL_VADDRESS32;
			else res=pool.free(have[i]),have[i]=NULL_VADDRESS32;
			assert(res==0);
		}
	}
	debug("Unit test for mem_pool finished");
}

Func_tester(mem_pool_tester);
};

#endif


