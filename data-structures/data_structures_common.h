#ifndef __DATA_STRUCTURES_COMMON_HEADER__
#define __DATA_STRUCTURES_COMMON_HEADER__
#include "common_header_zhc.h"
const int ADDRESS_TYPE_BIT_WIDTH=4;
const int ADDRESS_OFFSET_BIT_WIDTH=32-ADDRESS_TYPE_BIT_WIDTH;
const uint32_t ADDRESS_TYPE_MASK=((((uint32_t)1)<<ADDRESS_TYPE_BIT_WIDTH)-1)<<(32-ADDRESS_TYPE_BIT_WIDTH);
const uint32_t ADDRESS_OFFSET_MASK=~ADDRESS_TYPE_MASK;
const uint32_t INVALID_VADDR_TYPE=(1<<ADDRESS_TYPE_BIT_WIDTH)-1;
typedef uint32_t Vaddress32;
const Vaddress32 NULL_VADDRESS32=0;

struct signature64_t
{
	uint32_t sig1,sig2;
	signature64_t():sig1(0),sig2(0){};
	signature64_t(uint32_t s1,uint32_t s2):sig1(s1),sig2(s2){};
	signature64_t(uint32_t s1):sig1(s1),sig2(0){};
	signature64_t(uint64_t sig):sig1(sig&0xffffffff),sig2(sig>>32){};
	inline uint64_t combine()const{return (((uint64_t)sig2)<<32)^((uint64_t)sig1);};
	bool operator==(const signature64_t& other)
	{
		return sig1==other.sig1&&sig2==other.sig2;	
	};
};

inline uint32_t get_vaddr_type(Vaddress32 addr)
{
	return (addr&ADDRESS_TYPE_MASK)>>ADDRESS_OFFSET_BIT_WIDTH;
}
inline Vaddress32 combine_vaddr(uint32_t type,uint32_t offset)
{
	return (type<<ADDRESS_OFFSET_BIT_WIDTH)^offset;
}
inline uint32_t get_vaddr_offset(Vaddress32 addr)
{
	return addr&ADDRESS_OFFSET_MASK;
}
struct default_uint32_hash_function_t
{
	inline uint32_t operator()(uint32_t key,uint32_t bucket_size)
	{
		return key%bucket_size;	
	};
};
struct default_signature64_hash_function_t
{
	inline uint32_t operator()(const signature64_t& sig,uint32_t bucket_size)
	{
		return (sig.combine()%(uint64_t)bucket_size);	
	};
};

//Be carefull to use it.
//返回头四个字节组成的uint32_t
template <typename Usr_data_t>
struct default_extract_uint32_key_t
{
	uint32_t operator()(const Usr_data_t& usr_data)
	{
		return ((uint32_t*)(&usr_data))[0];
	};
};
//返回头四个字节组成的signature64_t
template <typename Usr_data_t>
struct default_extract_signature64_key_t
{
	signature64_t operator()(const Usr_data_t& usr_data)
	{
		return ((signature64_t*)(&usr_data))[0];
	};
};

//感谢wudisheng强大的trait技巧
template<typename Usr_data_t,typename Hash_key_t>
struct hashtable_trait_t {};
template<typename Usr_data_t>
struct hashtable_trait_t<Usr_data_t,uint32_t> 
{
	typedef default_extract_uint32_key_t<Usr_data_t> Extract_key_func_t;
	typedef default_uint32_hash_function_t Hash_func_t;	
};

template<typename Usr_data_t>
struct hashtable_trait_t<Usr_data_t,signature64_t> 
{
	typedef default_extract_signature64_key_t<Usr_data_t> Extract_key_func_t;
	typedef default_signature64_hash_function_t Hash_func_t;	
};
#endif
