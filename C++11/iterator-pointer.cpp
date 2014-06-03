
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <iostream>
#include <cmath>
#include <vector>
#include <queue>
#include <set>
#include <sstream>
#include <map>
#include <cstring>
#include <complex>
#include <numeric>
#include <functional>
#include <cassert>
#include <limits>
#include <limits.h>
#include <cstdarg>
#include <iterator>
#include <valarray>
#include <thread>
#include <memory>
using namespace std;
#define debug(x) cerr<<#x<<"=\""<<x<<"\""<<" at line#"<<__LINE__<<endl;

struct Candidate {
  double weight;
  string str;
  Candidate(double w, string s) : weight(w), str(s) {}
  Candidate() {}
  bool operator<(const Candidate& other) const {
    return weight < other.weight; 
  }
};

using CandidateList = vector<Candidate>;

class CandidatePointer {
  public:
    CandidatePointer(const CandidateList& list)
      : itr_(list.begin()) 
      , end_(list.end()) {}
    CandidatePointer(const CandidatePointer& other) = default;
    CandidatePointer& operator=(const CandidatePointer& other) = default;

    const CandidateList::value_type* operator->() const {
      return itr_ < end_ ? &(*itr_) : nullptr;
    }
    CandidatePointer& operator++() {
      ++itr_;
      return *this;
    }
    bool hasCandidate() const {
      return itr_ < end_;
    }

  private:              
    CandidateList::const_iterator itr_;
    CandidateList::const_iterator end_;
};        

int main() {
  CandidateList list;
  list.emplace_back(10.0, "adfasd");
  list.emplace_back(1.0, "adfasd");
  list.emplace_back(100.0, "adfasd");
  sort(list.begin(), list.end());
  CandidatePointer pointer1(list);
  cout << pointer1->str << " = " << pointer1->weight << endl;
  auto pointer2 = ++pointer1;
  for (;pointer2.hasCandidate(); ++pointer2) {
    cout << pointer2->str << " = " << pointer2->weight << endl;
  }
  return 0;
}
