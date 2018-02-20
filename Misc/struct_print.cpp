#include <algorithm>
#include <cassert>
#include <cmath>
#include <complex>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <vector>
using namespace std;
//#define NDEBUG
#ifndef NDEBUG
#define debug(x)                                                               \
  cerr << #x << "=\"" << x << "\""                                             \
       << " at line#" << __LINE__ << endl;
#define hline() cerr << "-----------------------------------------" << endl;
#else
#define debug(x)
#define hline()
#endif

typedef long long int llint;
//************************************************
template <typename A, typename B>
ostream &operator<<(ostream &out, const pair<A, B> &pp) {
  out << "(" << pp.first << "," << pp.second << ")";
  return out;
}
template <typename T> ostream &operator<<(ostream &out, const vector<T> &vect) {
  out << "length = " << vect.size() << endl;
  for (int i = 0; i < vect.size(); i++)
    out << vect[i] << " ";
  out << endl;
  return out;
}
ostream &operator<<(ostream &out, const vector<string> &vect) {
  out << vect.size() << " X " << vect[0].length() << endl;
  for (int i = 0; i < vect.size(); i++)
    out << vect[i] << endl;
  return out;
}

template <typename T>
ostream &operator<<(ostream &out, const vector<vector<T>> &vect) {
  out << "row number=" << vect.size() << endl;
  for (int i = 0; i < vect.size(); i++) {
    out << "row #" << i << ":";
    for (int j = 0; j < vect[i].size(); j++)
      out << " " << vect[i][j];
    out << endl;
  }
  return out;
}

// Be carefull for cut into strings!!!!
template <class T> vector<T> parse(const string &ss, const char *cut = " ") {
  vector<T> re;
  for (int j = 0; j < ss.size(); j++) {
    string s;
    while (j < ss.size() && NULL == strchr(cut, ss[j]))
      s += ss[j++];
    if (!s.empty()) {
      T tmp;
      istringstream is(s);
      is >> tmp;
      re.push_back(tmp);
    }
  }
  return re;
}
// Convert anything to string
template <class T> string convert(T vv) {
  ostringstream re;
  re << vv;
  return re.str();
}
template <typename T> T convert(const string &ss) {
  istringstream is(ss);
  T tmp;
  is >> tmp;
  return tmp;
}

void split(const string &token, vector<string> &seq) {
  string cur;
  for (int i = 0; i < token.length(); i++) {
    if (token[i] == '{' || token[i] == '}' || token[i] == ';' ||
        token[i] == ':' || token[i] == '[' || token[i] == ']') {
      if (cur.length())
        seq.push_back(cur);
      cur = "";
      seq.push_back(token.substr(i, 1));
    } else
      cur.push_back(token[i]);
  }
  if (cur.length())
    seq.push_back(cur);
}
struct item_t {
  string type, name;
  bool array;
  int size;
  item_t(string a, string b) : type(a), name(b), array(false){};
};

map<string, vector<item_t>> root;
map<string, string> macro;
void parse_line(const string &line, vector<string> &seq) {
  vector<string> tokens = parse<string>(line, " \t\r\n");
  if (tokens.empty())
    return;
  if (tokens[0][0] == '/' || tokens[0][0] == '*')
    return;
  if (tokens[0] == "#define") {
    string key = tokens[1];
    string value = "";
    for (int i = 2; i < tokens.size(); i++)
      value += tokens[i];
    cerr << "macro " << key << " = " << value << endl;
    macro[key] = value;
    return;
  }

  vector<string> tmp_seq;
  for (int i = 0; i < tokens.size(); i++)
    split(tokens[i], tmp_seq);

  for (int i = 0; i < tmp_seq.size(); i++) {
    if (tmp_seq[i][0] == '/')
      break;
    if (tmp_seq[i] == "union")
      tmp_seq[i] = "struct";
    seq.push_back(tmp_seq[i]);
  }
}

string valid_prefix(string &name) {
  if (name.length() == 0 || (name[0] != '_' && (!isalpha(name[0]))))
    return "";
  for (int i = 1; i < name.length(); i++)
    if ((!isalpha(name[i])) && (name[i] != '_') && (!isdigit(name[i])))
      return name.substr(0, i);
  return name;
}
vector<item_t> parse_struct(vector<string> &token) {
  assert(token.size() && token.back() == "{");
  vector<item_t> res;
  token.pop_back();
  while (token.size() && token.back() != "}") {
    if (token.back() == "typdef" || token.back() == "struct") {
      while (token.size() && token.back() != "{")
        token.pop_back();
      assert(token.size() && token.back() == "{");
      vector<item_t> sub = parse_struct(token);
      assert(token.size() && token.back() != ";");
      string sub_name = token.back();
      token.pop_back();
      for (int i = 0; i < sub.size(); i++) {
        sub[i].name = sub_name + "." + sub[i].name;
        res.push_back(sub[i]);
      }
      assert(token.size() && token.back() == ";");
      token.pop_back();
      continue;
    }
    string type = token.back();
    token.pop_back();
    assert(token.size());
    string name = token.back();
    token.pop_back();
    debug(type);
    assert(valid_prefix(type) == type);
    item_t item(type, name);
    assert(token.size());
    if (token.back() == "[") {
      token.pop_back();
      item.array = true;
      assert(token.size());
      string size;
      if (macro.count(token.back()))
        size = macro[token.back()];
      else
        size = token.back();
      item.size = convert<int>(size);
      token.pop_back();
      assert(token.back() == "]");
      token.pop_back();
    }
    debug(item.name);
    assert(item.name == valid_prefix(item.name));
    while (token.size() && token.back() != ";")
      token.pop_back();
    assert(token.size() && token.back() == ";");
    token.pop_back();
    res.push_back(item);
  }
  assert(token.size() && token.back() == "}");
  token.pop_back();
  return res;
}
void print_item(const string &t, const item_t &);
void print_struct(const string &prefix, string sname) {
  // printf("\tout<<\"struct %s : \";\n",sname.c_str());
  assert(root.count(sname));
  const vector<item_t> &list = root[sname];
  for (int i = 0; i < list.size(); i++) {
    string type = list[i].type;
    string name = list[i].name;
    if (root.count(type)) {
      print_struct(prefix + name + ".", type);
    } else {
      print_item(prefix, list[i]);
    }
  }
  // printf("\tout<<endl;\n");
}
void print_item(const string &prefix, const item_t &item) {
  string name = prefix + item.name;
  if (item.array && item.type != "char") {
    for (int i = 0; i < item.size; i++) {
      // printf("\tout<<\"%s[%d]=\"<<%s[%d]<<\"\\t\";\n",name.c_str(),i,("obj."+name).c_str(),i);
      printf("\tout<<%s[%d]<<\"\\t\";\n", ("obj." + name).c_str(), i);
    }
  } else {
    // printf("\tout<<\"%s=\"<<%s<<\"\\t\";\n",name.c_str(),("obj."+name).c_str());
    printf("\tout<<%s<<\"\\t\";\n", ("obj." + name).c_str());
  }
}
int main() {
  vector<string> token;
  char line[10000];
  while (gets(line)) {
    parse_line(string(line), token);
  }
  cerr << token << endl;
  reverse(token.begin(), token.end());
  while (token.size()) {
    // if(token.back()!="typdef"&&token.back()!="struct")
    if (token.back() != "struct") {
      token.pop_back();
      continue;
    }
    // while(token.size()&&token.back()!="struct")token.pop_back();
    assert(token.size());
    token.pop_back();
    assert(token.size());
    string sname = token.back();
    token.pop_back();
    assert(token.size() && token.back() == "{");
    vector<item_t> list = parse_struct(token);
    assert(token.size());
    if (token.back() != ";")
      sname = token.back(), token.pop_back();
    assert(token.size() && token.back() == ";");
    root[sname] = list;
    hline();
    debug(sname);
    for (int i = 0; i < list.size(); i++) {
      cerr << list[i].type << " " << list[i].name;
      if (list[i].array)
        cerr << "[" << list[i].size << "]";
      cerr << ";" << endl;
    }
  }
  puts("//This is an auto-generated code!");
  puts("#include <iostream>");
  puts("using namespace std;");
  for (map<string, vector<item_t>>::iterator itr = root.begin();
       itr != root.end(); itr++) {
    printf("void print_%s(const %s& obj,ostream& out)\n", itr->first.c_str(),
           itr->first.c_str());
    puts("{");
    print_struct("", itr->first);
    puts("}");
  }
  return 0;
}
