#include "MinMaxQuery.h"
#include "segment_tree.h"
#include "numeric.h"

class range_maxer_tester {
  static const int N=10000;
  range_maxer_t<N> tree;
  int value[N];
  public:
    void run() {
      fill(value,value+N,numeric_limits<int>::min());
	//srand(time(NULL));
	for(int i=0;i<10*N;i++) {
		int pos=rand()%N;
		if(rand()&1) {
			int v=rand();
			tree.update(pos,v);
			value[pos]=max(value[pos],v);
			//debug(value[pos]);
			//debug(pos);
		}
		else {
			int ans=numeric_limits<int>::min();
			for(int i=0;i<=pos;i++)ans=max(ans,value[i]);
			//debug(ans);
			//debug(tree.query(pos));
			//debug(pos);
			assert(tree.query(pos)==ans);
		}
    }
  }
};
class segment_tree_tester_t {
  public:
  static void run() {
    const int N=17;
    vector<int> value(N,0);
    segment_tree::segment_tree_t<segment_tree::sum_node_t<int> > tree(N);
    for(int cc=0;cc<1000;cc++) {
      int f=rand()%2;
      int low=rand()%N;
      int high=low+(rand()%(N-low));
      int v=10-(rand()%20);
      segment_tree::sum_node_t<int> node;
      assert(low<N&&low<=high&&high<N);
      //cerr<<f<<" "<<low<<" "<<high<<" "<<v<<endl;
      if(f==0) {
        node.covered=true;
        node.v=v;
        tree.update(low,high,node);
        for(int i=low;i<=high;i++) {
          value[i]=v;
        }
      }
      else if(f==1) {
        node.add=v;
        tree.update(low,high,node);
        for(int i=low;i<=high;i++) {
          value[i]+=v;
        }
      }
      //for(int i=0;i<N;i++)cerr<<value[i]<<" ";cerr<<endl;
      //tree.print();
      for(int low=0;low<N;low++)
      for(int high=low;high<N;high++) {
        //cerr<<low<<" "<<high<<endl;
        segment_tree::sum_node_t<int> node;
        //debug(low);
        //debug(high);
        tree.query(low,high,node);
        int ans=0;
        for(int i=low;i<=high;i++)ans+=value[i];
        //debug(ans);
        //debug(node.s);
        assert(ans==node.s);
      }
    }
  }
};
class numeric_tester {
  public:
    static void run() {
      for(int cc=0;cc<100;cc++) {
        int a=50-(rand()%100);
        int b=50-(rand()%100);
        int c=50-(rand()%100);
        int d=50-(rand()%100);
        if(a>b)swap(a,b);
        if(c>d)swap(c,d);
        double t=interval_delta_sum(a,b,c,d);
        double ans=0.0;
        for(int x=a;x<=b;x++)
        for(int y=max(c,x);y<=d;y++) {
          ans+=y-x;
        }
        if(fabs(ans-t)>0.5) {
          debug(ans);
          debug(t);
        }
        assert(fabs(ans-t)<0.5);
      }
    }
};
int main() {
  segment_tree_tester_t::run();
  (new range_maxer_tester())->run();
  numeric_tester::run();
	return 0;
}
