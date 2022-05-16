/*
  文法类实现
*/

#include "../LEX/LEX.h"
#include "dictree.cpp"
#include <unordered_map>
#include <iomanip>
//产生式类
class Production{
public:
  string left;//左部
  vector<string> right;//右部
  Production(const string& l,const vector<string>& r):left(l),right(r){}
  //打印
  void print()const{
    cout<<left<<"->";
    for(const auto& str:right)
      cout<<str;
    cout<<endl;
  }
};
//项目类
class Item{
public:
  int prod_idx;//产生式编号
  int pnt_idx;//“点”的位置
  Item(int x,int y):prod_idx(x),pnt_idx(y){}
  bool operator==(const Item& I)const{
    return I.pnt_idx==pnt_idx&&I.prod_idx==prod_idx;
  }
};
typedef vector<Item> Items;//项集类
typedef vector<Items> Collection;//项集族
//为Item和Items特例化一个哈希模板
namespace std{
  template<>
  struct hash<Item>{
    typedef size_t result_type;
    typedef Item argument_type;
    size_t operator()(const Item& I)const{
      return hash<int>()(I.prod_idx)^hash<int>()(I.pnt_idx);
    }
  };
  template<>
  struct hash<Items>{
    typedef size_t result_type;
    typedef Items argument_type;
    size_t operator()(const Items& I)const{
      size_t res=0;
      for(const auto& i:I)
        res^=hash<Item>()(i);
      return res;
    }
  };
  
}

//文法类
class Grammer{
friend class SLR;
public:
  Grammer(ifstream& in){
    /*init*/
    /*产生式右部记得加一个空串，表示结尾！*/
    string buffer;//用来接受字符串
    vector<string> res;//存放字符串分割结果
    in>>start;//开始符号
    //获取非终结符
    in>>buffer;
    res=string_split(buffer,",");
    for(const auto& str:res)
      vset.insert(str);
    //获取终结符
    in>>buffer;
    res=string_split(buffer,",");
    for(const auto& str:res)
      tset.insert(str);
    //获取产生式
    vector<string> GX;//保存所有文法符号
    for(const auto& x:vset){
      GX.push_back(x);
    }
    for(const auto& x:tset){
      GX.push_back(x);
    }
    Trie t(GX); //构建字典树     
    while(!in.eof()){
      in>>buffer;
      string left;//产生式左部
      vector<string> right;//产生式右部
      res=string_split(buffer,"->");
      left=res[0];
      //右部字符串解析出各个文法符号需要用到字典树这个数据结构
      right=t.get_words(res[1]);
      right.emplace_back("");
      pvec.emplace_back(left,right);
    }
    make_collection();
    make_action();
    
  }
  //打印所有产生式
  void print_allprod(){
    for(const auto& P:pvec)
      P.print();
  }
  //打印action表
  void print_actiontable(){
      print_table(get_printable_actiontable());
  }
  //获取可打印的action表
  vector<vector<string>> get_printable_actiontable(){
      vector<vector<string>> res;
      //列属性
      res.push_back({"status"});
      res[0].insert(res[0].end(),tset.cbegin(),tset.cend());
      res[0].push_back("$");
      //列值
      for(int i=0;i<C.size();i++){
          vector<string> strs{to_string(i)};
          for(const string& x:tset){
              strs.emplace_back(action_table[i][x]);
          }
          strs.emplace_back(action_table[i]["$"]);
          res.push_back(std::move(strs));
      }
      return res;
  }
  //打印表
  void print_table(const vector<vector<string>>& table){
      for(const auto& i:table) {
          for (const string &str: i)
              cout <<setw(8) <<str;
          cout << endl;
      }
  }
  //构建action表
  void make_action(){
    string B;
    make_follow();
    for(int i=0;i<C.size();++i){
      for(int j=0;j<C[i].size();++j){
        int prod_idx=C[i][j].prod_idx,pnt_idx=C[i][j].pnt_idx;
        B=pvec[prod_idx].right[pnt_idx];
        //如果B是终结符
        if(tset.count(B)){
          int next=goto_table[i][B];
          action_table[i][B]="s"+to_string(next);
        }
        else if(B.empty()){
          string L=pvec[prod_idx].left;
          if(L==start){
            action_table[i]["$"]="acc";
          }
          else{
            for(const auto& a:follow[L]){
              action_table[i][a]="r"+to_string(prod_idx);
            }
          }
        }
      }
    }
  }
  //构造项集族函数
  void make_collection(){
    vector<string> GX;//保存所有文法符号
    for(const auto& x:vset){
      GX.push_back(x);
    }
    for(const auto& x:tset){
      GX.push_back(x);
    }
    unordered_map<Items,int> visited;//保存已经求出的项集
    //C={closure({[S'->·S]})}
    C={closure(Items({Item(0,0)}))};
    visited.insert({C[0],0});
    int over=0;
    while(!over){
      over=1;
      for(int i=0;i<C.size();i++){
        for(const string& X:GX){
          Items J=Goto(C[i],X);
          if(!J.empty()){
            if(!visited.count(J)){
                goto_table[i][X]=C.size();
                visited.insert({J,C.size()});
                C.push_back(std::move(J));
                over=0;
            }
            else{
                goto_table[i][X]=visited[J];
            }
          }
        }
      }
    }
  }
  //构造goto函数
  Items Goto(const Items& I,const string& X){
    Items J;
    string B;
    for(int i=0;i<I.size();++i){
      int prod_idx=I[i].prod_idx,pnt_idx=I[i].pnt_idx;
      B=pvec[prod_idx].right[pnt_idx];
      if(B==X){
        J.push_back(Item(I[i].prod_idx,I[i].pnt_idx+1));
      }
    }
    return closure(J);
  }
  //构建项集的闭包
  Items closure(const Items& I){
    Items J=I;
    int over=0;//是否结束
    string B;
    unordered_set<string> visited;//存放已经访问的非终结符
    while(!over){
      over=1;
      for(int i=0;i<J.size();i++){
        //对于J中的每一个A->a·Bb
        int prod_idx=J[i].prod_idx,pnt_idx=J[i].pnt_idx;
        B=pvec[prod_idx].right[pnt_idx];
        if(!visited.count(B)&&vset.count(B)){
          for(int j=0;j<pvec.size();j++){
            if(pvec[j].left==B){
              J.push_back(Item(j,0));
              over=0;//有项目更新
            }
          }
          visited.insert(B);//B已经访问完毕
        }
      }
        
    }
    return J;
  }
  //为每一个非终结符构建first集
  void make_first(){
    int over=0;
    while(!over){
      over=1;
      //对于每一个非终结符号X
      for(const string& X:vset){
        for(const auto& P:pvec){
          if(P.left==X){
            const auto& right=P.right;
            //注意去掉右部最后一个空字符串
            for(int i=0;i<right.size()-1;++i){
              const string& Y=right[i];
              //如果Y是终结符
              if(tset.count(Y)){
                  //在first(X)中不存在，就加入
                if(!first[X].count(Y)){
                    first[X].insert(Y);
                    over=0;
                }
                break;//进入下一个产生式
              }
              //否则，如果是非终结符
              if(vset.count(Y)){
                const auto& F=first[Y];
                if(F.empty()){
                  break;//如果是空，直接下一个产生式
                }
                else{
                  int has_episilon=0;//
                  for(const string& x:F){
                    if(x!="episilon"&&!first[X].count(x)){
                      first[X].insert(x);
                      over=0;
                    }
                    if(x=="episilon") has_episilon=1;
                  }
                  if(!has_episilon) break;//如果没有episilon，就进入下一个产生式
                  //有episilon
                  if(i==right.size()-1&&!first[X].count("episilon")){
                      first[X].insert("episilon");
                      over=0;
                  }

                }

              }
            }
          }
        }
      }
    }
  }
  //获取文法字符串的first集，为构建follow集做准备
  unordered_set<string> get_firstXs(const vector<string>& Xs){
    if(Xs.empty())return {};
    if(tset.count(Xs[0]))return {Xs[0]};
    unordered_set<string> res;
    for(int i=0;i<Xs.size();i++){
      const string& X=Xs[i];
      const auto& F=first[X];
      int has_episilon=0;
      for(const string& x:F){
        if(x!="episilon")
          res.insert(x);
        if(x=="episilon") has_episilon=1;
      }
      if(!has_episilon)break;
      if(i==Xs.size()-1)
        res.insert("episilon");
    }
    return res;
  }
  //为每一个非终结符构建follow集
  void make_follow(){
    make_first();
    //先将$放入follow(S)
    follow[start].insert("$");
    int over=0;
    while(!over){
      over=1;
      for(const auto& P:pvec){
      string left=P.left;
      const auto& right=P.right;
      for(int i=0;i<right.size()-1;i++){
        const string& B=right[i];
        if(vset.count(B)){
          //如果是A->aBb
          if(i<right.size()-2){
          vector<string> b(right.cbegin()+i+1,right.cend()-1);
          auto firstb=get_firstXs(b);
          int has_episilon=0;
          for(const string& x:firstb){
            if(x!="episilon"&&!follow[B].count(x)){
              follow[B].insert(x);
              over=0;
            }
              
            if(x=="episilon")
              has_episilon=1;
          }
          if(has_episilon){
            const auto& followleft=follow[left];
            for(const string& x:followleft){
                if(!follow[B].count(x)){
                    follow[B].insert(x);
                    over=0;
                }
            }
          }

        }else{
              //如果是A->aB
              const auto& followleft=follow[left];
              for(const string& x:followleft){
                  if(!follow[B].count(x)){
                      follow[B].insert(x);
                      over=0;
                  }
              }
          }


        }
      }
    }
    }
    
  }

private:
    string start;//开始符号
    unordered_set<string> vset;//非终结符集合
    unordered_set<string> tset;//终结符集合
    vector<Production> pvec;//产生式数组
    Collection C;//项集族
    unordered_map<string,unordered_set<string>> first;//first集
    unordered_map<string,unordered_set<string>> follow;//follow集
public:
    unordered_map<int,unordered_map<string,int>> goto_table;//goto表
    unordered_map<int,unordered_map<string,string>> action_table;//action表
};
