/***************************************************************************
 * 
 * Copyright (c) 2009 Baidu.com, Inc. All Rights Reserved
 * $Id: Huffman.cpp,v 1.1.2.1 2009/09/10 06:29:33 zhuhongcheng Exp $ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file Huffman.cpp
 * @author Zhu Hongcheng 
 * @date 2009/08/31 10:31
 * @version $Revision: 1.1.2.1 $ 
 **/

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <stdint.h>
#include <new>
//#include <iostream>


#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <stdint.h>
#include <new>
//#define NDEBUG
#include <assert.h>




namespace Huffman
{
#define com_writelog fprintf
#define COMLOG_FATAL stderr
#define COMLOG_WARNING stderr
#define COMLOG_DEBUG stderr
#define TWO(x) (((uint32_t)1)<<(x))

#define MASK(x) ((TWO(x))-1)

typedef uint32_t uint;

typedef unsigned long long int uint64;

const int MAX_HUFFMAN_SIZE=300000;
// Here normalized Huffman encoding is used
class Huffman_encode_table_t
{
	int capacity;
public:
	int n;
	uint *code;
	uint *length;
	int *map_table;
	int resize(int);
	Huffman_encode_table_t():capacity(0),n(0),code(NULL),length(NULL),map_table(NULL){};
	int create(const uint frequency[],int n,const int map_t[]=NULL);
	int serialization(FILE* out_file)const;
	int deserialization(FILE* in_file);
	int get_code(int idx,uint& out_code,uint& out_len)const;
	~Huffman_encode_table_t();
	friend class Huffman_decode_table_t;
};

class Huffman_decode_table_t
{
	int capacity;
public:
	int n;
	uint *offset;
	int *map_table;
	uint *first_code;
	uint min_code_length;
	int resize(int);
	int serialization(FILE* out_file)const;
	int deserialization(FILE* in_file);
	Huffman_decode_table_t():capacity(0),n(0),offset(NULL),map_table(NULL),first_code(NULL),min_code_length(0){};
	int create(const Huffman_encode_table_t& encode_table);
	int Huffman_decode_nitem(const uint32_t[],const uint64,uint64&,int [],int )const;
	~Huffman_decode_table_t();
};
};


namespace Huffman
{

#define Delete_array(x) if(NULL!=(x))delete [] (x);
Huffman_encode_table_t::~Huffman_encode_table_t()
{
	Delete_array(code);
	Delete_array(length);
	Delete_array(map_table);
}
int Huffman_encode_table_t::resize(int size)
{
	if(capacity>=size) return 0;
	Delete_array(code);
	Delete_array(length);
	Delete_array(map_table);
	try
	{
		capacity=2*size;
		n=size;
		code=new uint[capacity];
		length=new uint[capacity];
		map_table=new int[capacity];
	}
	catch(std::bad_alloc)
	{
		Delete_array(code);
		Delete_array(length);
		Delete_array(map_table);
		Huffman_encode_table_t();
		com_writelog(COMLOG_FATAL,"Alolcating memory for Huffman_encode_table_t failed");
		return -1;
	}
	return 0;
}
int Huffman_encode_table_t::get_code(int idx,uint& out_code,uint& out_len)const 
{
		if(idx<0||idx>=n) {
			com_writelog(COMLOG_FATAL,"[idx:%d] does not in the range [%d,%d] in Huffman_encode_table_t::get_code",idx,0,n-1);	
			return -1;
		}
		out_code=code[idx];
		out_len=length[idx];
		return 0;	
}
Huffman_decode_table_t::~Huffman_decode_table_t()
{
	Delete_array(offset);
	Delete_array(first_code);
	Delete_array(map_table);
}
int Huffman_decode_table_t::resize(int size)
{
	if(capacity>=size) return 0;
	Delete_array(offset);
	Delete_array(map_table);
	Delete_array(first_code);
	try
	{
		capacity=2*size;
		n=size;
		offset=new uint[capacity];
		first_code=new uint[capacity];
		map_table=new int[capacity];
	}
	catch(std::bad_alloc)
	{
		Delete_array(offset);
		Delete_array(first_code);
		Delete_array(map_table);
		Huffman_decode_table_t();
		com_writelog(COMLOG_FATAL,"Alolcating memory for Huffman_decode_table_t failed");
		return -1;
	}
	return 0;
}



/*
 * A linear time counting sort
*/ 
void counting_sort(uint count_array[],uint32_t count_size,uint item[],uint32_t value[],int n,uint buffer[])
{
	memset(count_array,0,sizeof(uint32_t)*count_size);
	for(int i=0;i<n;i++) {
		assert(value[item[i]]<count_size);
		count_array[value[item[i]]]++;
	}
	for(uint32_t i=1;i<count_size;i++) {
		count_array[i]+=count_array[i-1];
	}
	for(int i=n-1;i>=0;i--) {
		buffer[--count_array[value[item[i]]]]=item[i];
	}
	memcpy(item,buffer,sizeof(int)*n);
}



int Huffman_encode_table_t::create(const uint frequency[],int fn,const int map_t[])
{
	if(fn<=0) {
		com_writelog(COMLOG_FATAL,"Illegal Huffman size [n:%d]",fn);
		return -1;
	}
	if(0>resize(fn)) {
		com_writelog(COMLOG_FATAL,"resize failed for Huffman_encode_table_t");
		return -1;
	}
	if(NULL!=map_t) {
		memcpy(map_table,map_t,sizeof(int)*n);
	}
	if(n==1)
	{
		length[0]=code[0]=0;
		return 0;
	}
	const int radix_length=16;
	uint *buf1=NULL,*buf2=NULL,*buf3=NULL,*buf4=NULL,*count_array=NULL;
	try
	{
		count_array=new uint[std::max(TWO(16),(uint)n)];
		buf1=new uint[2*n-1];
		buf2=new uint[2*n-1];
		buf3=new uint[2*n-1];
		buf4=new uint[2*n-1];
	}
	catch(std::bad_alloc)
	{
		Delete_array(count_array);
		Delete_array(buf1);
		Delete_array(buf2);
		Delete_array(buf3);
		Delete_array(buf4);
		com_writelog(COMLOG_FATAL,"Allocating buffers for Huffman_encode_table_t::create failed");
		return -1;
	}
	uint* item=buf1;
	uint* value=buf2;
	
	for(int i=0;i<n;i++) {
		item[i]=i;
		value[i]=frequency[i]&MASK(radix_length);
	}

	counting_sort(count_array,TWO(radix_length),item,value,n,buf3);

	for(int i=0;i<n;i++) {
		value[i]=frequency[i]>>radix_length;
	}
	
	counting_sort(count_array,TWO(radix_length),item,value,n,buf3);

	
	uint* const zero_branch=buf3;
	uint* const one_branch=buf4;

#ifndef NDEBUG
	for(int i=1;i<n;i++)
		assert(frequency[item[i]]>=frequency[item[i-1]]);
#endif
	for(int i=0;i<n;i++) {
		value[n-1+i]=frequency[i];
		item[i]+=n-1;
	}
	
	int top=n-2;
	int head1=0,tail1=0,head=0;
	while(tail1-head1+n-head>1) {
		int a,b;
		if(head1==tail1)a=item[head++];
		else if(head==n)a=item[head1++];
		else if(value[item[head]]<=value[item[head1]])a=item[head++];
		else a=item[head1++];
		
		if(head1==tail1)b=item[head++];
		else if(head==n)b=item[head1++];
		else if(value[item[head]]<=value[item[head1]])b=item[head++];
		else b=item[head1++];
		
		assert(top>=0);
		value[top]=value[a]+value[b];
		zero_branch[top]=a;
		one_branch[top]=b;
		assert(tail1<head);
		item[tail1++]=top;
		
		top--;
	}
	assert(top==-1);


	uint32_t *depth=buf2;
	
	depth[0]=0;
	for(int i=0;i<n-1;i++) {
		depth[zero_branch[i]]=depth[one_branch[i]]=depth[i]+1;
	}
	// sort by code length
	for(int i=0;i<n;i++) {
		item[i]=i;
	}
	counting_sort(count_array,n,item,depth+n-1,n,buf3);

	code[item[n-1]]=0;
	length[item[n-1]]=depth[item[n-1]+n-1];
//	std::cerr<<item[n-1]<<":"<<map_table[item[n-1]]<<" "<<code[item[n-1]]<<" "<<length[item[n-1]]<<std::endl;

	for(int i=n-2;i>=0;i--) {
		length[item[i]]=depth[item[i]+n-1];
		if(length[item[i]]==length[item[i+1]]) {
			code[item[i]]=code[item[i+1]]+1;
		}
		else {
			assert(length[item[i]]<length[item[i+1]]);
			code[item[i]]=(code[item[i+1]]+1)>>(length[item[i+1]]-length[item[i]]);
		}
		assert(code[item[i]]<n);
		assert(length[item[i]]<n);
		assert(length[item[i]]>=32||code[item[i]]<TWO(length[item[i]]));
//		std::cerr<<item[i]<<":"<<map_table[item[i]]<<" "<<code[item[i]]<<" "<<length[item[i]]<<std::endl;
	}
	delete [] count_array;
	delete [] buf1;
	delete [] buf2;
	delete [] buf3;
	delete [] buf4;
	com_writelog(COMLOG_DEBUG,"Huffman_encode_table_t::create succeeded:)");	
	return 0;	
}

int Huffman_decode_table_t::create(const Huffman_encode_table_t& encode_table)
{
	const uint* length=encode_table.length;
	const uint* code=encode_table.code;
	if(0>resize(encode_table.n)) {
		com_writelog(COMLOG_FATAL,"resize for Huffman_decode_table_t failed");
		return -1;
	}
	min_code_length=*std::min_element(length,length+n);
	//std::cerr<<"min_code_length="<<min_code_length<<std::endl;
	assert(*std::max_element(length,length+n)<n);
	uint* const count_array=new uint[n];

	if(NULL==(count_array)) {
		com_writelog(COMLOG_FATAL,"Allocating buffer for Huffman_decode_table_t::create");
		return -1;
	}
	memset(count_array,0,sizeof(uint)*n);

	for(int i=0;i<n;i++) {
		count_array[length[i]]++;
	}
	offset[n-1]=first_code[n-1]=0;
	for(int i=n-2;i>=0;i--) {
		first_code[i]=(first_code[i+1]+count_array[i+1])>>1;	
		offset[i]=count_array[i+1]+offset[i+1];
	}
	
	for(int i=0;i<n;i++) {
		uint len=length[i];
		map_table[offset[len]+code[i]-first_code[len]]=encode_table.map_table[i];
	}
#ifndef NDEBUG
//	puts("map_table");
//	for(int i=0;i<n;i++)printf("%d ",map_table[i]);
//	puts("");
//	for(int i=n-1;i;i--)
//		printf("len=%d first_code=%u offset=%u\n",i,first_code[i],offset[i]);
#endif
	delete [] count_array;
	return 0;
}
int Huffman_decode_table_t::serialization(FILE* out_file)const
{
	if(1!=fwrite(&n,sizeof(n),1,out_file)||
			(uint)n!=fwrite(map_table,sizeof(map_table[0]),n,out_file)||
			1!=fwrite(&min_code_length,sizeof(min_code_length),1,out_file)||
			(uint)n!=fwrite(first_code,sizeof(first_code[0]),n,out_file)||
			(uint)n!=fwrite(offset,sizeof(offset[0]),n,out_file)) {
		com_writelog(COMLOG_FATAL,"Huffman_decode_table_t::serialization writing file failed");
		return -1;
	}
	return 0;
}
int Huffman_decode_table_t::deserialization(FILE* in_file)
{
	if(1!=fread(&n,sizeof(n),1,in_file)) {
		com_writelog(COMLOG_FATAL,"Huffman_decode_table_t::serialization writing file failed");
		return -1;
	}
	if(0>resize(n)) {
		com_writelog(COMLOG_FATAL,"resize in Huffman_decode_table_t::deserialization failed");
		return -1;
	}
	if((uint)n!=fread(map_table,sizeof(map_table[0]),n,in_file)||
			1!=fread(&min_code_length,sizeof(min_code_length),1,in_file)||
			(uint)n!=fread(first_code,sizeof(first_code[0]),n,in_file)||
			(uint)n!=fread(offset,sizeof(offset[0]),n,in_file)) {
		com_writelog(COMLOG_FATAL,"Huffman_decode_table_t::serialization writing file failed");
		return -1;
	}
	//std::cerr<<"min_code_length="<<min_code_length<<std::endl;
	return 0;

}


int Huffman_decode_table_t::Huffman_decode_nitem(const uint32_t bit_stream[],
												 const uint64 bit_length,uint64& bit_offset,
												 int decoded_items[],int maximum_items)const
{
	int res=0;
	while(res<maximum_items) {
		uint code=0;
		for(uint i=0;i<min_code_length;i++)	{
			code<<=1;
			code^=((bit_stream[bit_offset>>5])>>(bit_offset&0x1f))&1;
			bit_offset++;
		}
		uint len;
		for(len=min_code_length;code<first_code[len];len++) {
			code<<=1;
			code^=((bit_stream[bit_offset>>5])>>(bit_offset&0x1f))&1;
			bit_offset++;
		}
		if(bit_offset>bit_length) {
			com_writelog(COMLOG_FATAL,"code boundary exceeds [bit_length:%llu] [offset%llu] [code%u]",bit_length,bit_offset,code);
			return -1;
		}
		if(code>=(uint)n) {
			com_writelog(COMLOG_FATAL, "Illegal Huffman code [code:%u] [n:%d]",code,n);
			return -1;
		}
		decoded_items[res++]=map_table[offset[len]+(code-first_code[len])];
	}
	return res;
}
};
int main()
{

}
