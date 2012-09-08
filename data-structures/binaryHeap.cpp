#include <assert.h>
#include <vector>
using namespace std;
//Heap
template<bool (*cmpLess)(int ,int )>
class binaryHeap
{
	vector<int> heap,hpos;
	int hs;
public:
	binaryHeap<cmpLess>(int cap=100){hs=0;heap.resize(cap);hpos.resize(cap);};
	void adjustUp(int key)
	{
		int pos=hpos[key];
		assert(pos>=0&&pos<hs);
		assert(heap[pos]==key);
		while(pos&&cmpLess(key,heap[(pos-1)>>1]))
		{
			heap[pos]=heap[(pos-1)>>1];
			hpos[heap[pos]]=pos;
			pos=(pos-1)>>1;
		}
		heap[pos]=key;
		hpos[key]=pos;
	}
	void adjustDown(int key)
	{
		int pos=hpos[key],mm;
		assert(pos>=0&&pos<hs);
		assert(heap[pos]==key);
		while((pos<<1)+1<hs)
		{
			mm=1+(pos<<1);
			if(mm+1<hs&&cmpLess(heap[mm+1],heap[mm]))mm++;
			if(cmpLess(key,heap[mm]))break;
			heap[pos]=heap[mm];
			hpos[heap[pos]]=pos;
			pos=mm;
		}
		heap[pos]=key;
		hpos[key]=pos;
	}
	int popHeap()
	{
		int re=heap[0];
		hs--;
		if(hs)
		{
			heap[0]=heap[hs];
			hpos[heap[0]]=0;
			adjustDown(heap[0]);
		}
		return re;
	}
	void removeHeap(int key)
	{
		int pos=hpos[key];
		assert(pos>=0&&pos<hs);
		assert(heap[pos]==key);
		heap[pos]=heap[hs-1];
		hpos[heap[pos]]=pos;
		hs--;
		if(hs)
		{
			adjustUp(heap[pos]);
			adjustDown(heap[pos]);
		}
	}
	void pushHeap(int key)
	{
    if(hs>=heap.size()||key>=hs) {
      heap.resize(max(hs+1,key+1));
      hpos.resize(heap.size());
      hs=heap.size();
    }
		heap[hs]=key;
		hpos[key]=hs;
		hs++;
		adjustUp(key);
	}
	int size(){return hs;};
	void clear(){hs=0;};
  int heapTop() {
    return heap[0];
  }
};
