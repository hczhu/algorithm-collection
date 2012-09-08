#include "ACAuto.h"

template<typename T>
class twoD_matcher_t {
 public:
  struct match_t  {
    int r,c,idx;
    bool operator<(const match_t& b)const {
      return idx<b.idx;
    };
    bool operator==(const match_t& b) const {
      return memcmp(this,&b,sizeof(*this))==0;
    }
    match_t(int x=-1,int y=-1,int z=-1):r(x),c(y),idx(z){};
  };
  typedef vector<string> grid_t;
  vector<match_t> twoDMatcher(const grid_t& text,const vector<grid_t>& pat) {
    vector<match_t> res;
    if(pat.empty()) {
      return res;
    }
    const int m=pat[0][0].length();
    ACAuto<char,int> tree;
    for(int i=0;i<pat.size();i++) {
      assert(pat[i][0].length()==m);
      for(int j=0;j<pat[i].size();j++) {
        tree.insertString(pat[i][j].c_str(),pat[i][j].length(),i);
      }
    }
    if(!tree.construct()) {
      std::cerr<<"construct error"<<endl;
      debug("here");
      return res;
    }
    std::cerr<<"pat tree"<<endl;
    tree.print_status();
    //return res;
    ACAuto<int,int> one_tree;
    vector<vector<int> > oneD_pat;
    for(int i=0;i<pat.size();i++) {
      vector<int> one_pat(pat[i].size());
      for(int j=0;j<pat[i].size();j++) {
        int st=0;
        for(int k=0;k<pat[i][j].length();k++) {
          st=tree.goNext(st,pat[i][j][k]);
          assert(st>0);
        }
        one_pat[j]=st;
        assert(tree.getDep(st)==m);
      }
      one_tree.insertString(&one_pat[0],one_pat.size(),i);
      oneD_pat.push_back(one_pat);
    }
    if(!one_tree.construct()) {
      std::cerr<<"construct failed"<<endl;
      debug("here");
      return res;
    }
    std::cerr<<"one tree"<<endl;
    one_tree.print_status();
  
    vector<int> pat_stat(pat.size());
    for(int i=0;i<pat.size();i++) {
      pat_stat[i]=0;
      for(int j=0;j<oneD_pat[i].size();j++) {
        pat_stat[i]=one_tree.goNext(pat_stat[i],oneD_pat[i][j]);
        assert(pat_stat[i]>0);
      }
    }
    //debug(pat_stat);
    //start matching
    vector<int> compact(text.size(),0);
    for(int c=0;c<text[0].length();c++) {
      for(int r=0;r<text.size();r++) {
        compact[r]=tree.getNext(compact[r],text[r][c]);
      }
      if(c==105)debug(compact);
      if(c+1>=m) {
        int st=0;
        for(int r=0;r<compact.size();r++) {
          st=one_tree.getNext(st,compact[r]);
          //if(c==105&&r<20)cerr<<r<<" "<<st<<endl;
          for(int mst=one_tree.isMatched(st)?st:one_tree.getMatchlink(st);mst>0;
              mst=one_tree.getMatchlink(mst)) {
            int idx=one_tree.getInfo(mst);
            //if(c==105&&r<20)cerr<<"matched "<<pat[idx]<<" "<<idx<<endl;
            res.push_back(match_t(r+1-pat[idx].size(),c+1-m,idx));
          }
        }
      }
    }
    
  #ifdef LOCAL
    for(int i=0;i<res.size();i++) {
      assert(res[i].r>=0&&res[i].r+pat[res[i].idx].size()<=text.size());
      assert(res[i].c>=0&&res[i].c+m<=text[0].length());
      for(int r=0;r<pat[res[i].idx].size();r++) {
        for(int c=0;c<m;c++) {
          assert(text[r+res[i].r][c+res[i].c]==pat[res[i].idx][r][c]);
        }
      }
    }
  #endif
    for(int i=(int)res.size()-1;i>=0;i--) {
      int idx=res[i].idx;
      for(int j=0;j<pat_stat.size();j++) {
        if(j!=idx&&pat_stat[j]==pat_stat[idx]) {
          res.push_back(match_t(res[i].r,res[i].c,j));
        }
      }
    }
  #ifdef LOCAL
    for(int r=0;r<text.size();r++) {
      for(int c=0;c<text[r].length();c++) {
        for(int i=0;i<pat.size();i++) {
          const grid_t& let=pat[i];
          bool ok=(r+let.size()<=text.size())&&(c+m<=text[0].length());
          for(int x=0;x<let.size()&&ok;x++) {
            for(int y=0;y<m&&ok;y++) {
              ok=let[x][y]==text[r+x][c+y];
            }
          }
          if(ok) {
            //cerr<<"matched at"<<r<<" "<<c<<" with "<<i<<endl;
            //cerr<<pat[i]<<endl;
            //assert(0);
            assert(find(res.begin(),res.end(),match_t(r,c,i))!=res.end());
          }
        }
      }
    }
  #endif
    std::cerr<<"match column size="<<m<<" pattern "<<res.size()<<std::endl;
    return res;
  }
};
