//字典树实现
#include <unordered_map>
#include <memory>
#include <vector>
#include <string>
#include <functional>
using namespace std;
class Trie{
public:
  Trie():root(new char){}
  Trie(const vector<string>& words):root(new char){
    make_trie(words);
  }
  //根据字典树来匹配输入字符串，返回单词数组
  vector<string> get_words(const string& in_string){
    if(in_string.empty()) return {};
    vector<string> res;
    function<bool(int)> dfs=[&](int start){
        if(start==in_string.size())return true;
        int l=start,r=start;
        shared_ptr<char> p=root;
        char c;
        while(r<in_string.size()){
            c=in_string[r];
            if(trie_map[p].count(c)&&*(trie_map[p][c])!='$'){
                r++;
                p=trie_map[p][c];
                continue;
            }
            //匹配失败
            if(!trie_map[p].count(c)){
                return false;
            }
            if(*(trie_map[p][c])=='$'){
                res.push_back(in_string.substr(l,r+1-l));
                if(dfs(r+1)) return true;
                res.pop_back();
                r++;
                p=trie_map[p][c];
                continue;
            }
        }
        return false;

      };
    dfs(0);
    return res;
  }
  //输入单词组，构建字典树
  void make_trie(const vector<string>& words){
    shared_ptr<char> p;
    for(const string& word:words){
      p=root;
      for(int i=0;i<word.size();i++){
        char c=word[i];
        auto& child=trie_map[p];
        if(!child.count(c)){
          if(i==word.size()-1)
            child.insert({c,make_shared<char>('$')});//$标识单词结尾
          else
            child.insert({c,make_shared<char>('0')});
        }
        else{
            if(i==word.size()-1)
                *child[c]='$';//$标识单词结尾
        }
        p=child[c];//进入下一个结点
      }
    }
  }
private:
  unordered_map<shared_ptr<char>,unordered_map<char,shared_ptr<char>>> trie_map;
  shared_ptr<char> root;
};