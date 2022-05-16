/*
  SLR文法分析器类
*/
#include "Grammer.cpp"

class SLR{
public:
  SLR(Grammer& G):G(G){
    st_stk.push_back(0);
  }
  //打印分析结果
  void print(){
    for(int i:which_prod)
      G.pvec[i].print();
  }
  //开始语法分析函数
  void start(){
    /*调用之前记得先调用make_input函数*/
    int i=0;
    while(i<in_stk.size()){
      string cur_in=in_stk[i];//当前输入符号
      int state=st_stk.back();//栈顶状态
      string action=G.action_table[state][cur_in];//这里之前将G设置为const导致出错
      if(action.empty()){
        cout<<"err!"<<endl;
        return;
      }
      if(action=="acc"){
        cout<<"acc!"<<endl;
        return;
      }
      char a=action[0];
      int b=atoi(action.substr(1).c_str());
      if(a=='s'){
        //移入
        sgn_stk.push_back(cur_in);
        st_stk.push_back(b);
        i++;
      }
      else if(a=='r'){
        //规约
        string left=G.pvec[b].left;//取得对应产生式左部
        vector<string> right=G.pvec[b].right;//取得对应产生式右部
        int r_nums=right.size()-1;//右部文法字符个数
        //两个栈都弹出r_nums次
        for(int i=0;i<r_nums;i++){
          st_stk.pop_back();
          sgn_stk.pop_back();
        }
        sgn_stk.push_back(left);
        st_stk.push_back(G.goto_table[st_stk.back()][left]);
        which_prod.push_back(b);//保存使用的产生式编号
      }


    }
  }
  //根据词法分析结果，构造输入串
  void make_input(const vector<Token*>& tkvec){
    for(const auto& tkp:tkvec){
      if(tkp->type==DEC){
        in_stk.emplace_back("int10");
      }
      else if(tkp->type==OCT){
        in_stk.emplace_back("int8");
      }
      else if(tkp->type==HEX){
        in_stk.emplace_back("int16");
      }
      else if(tkp->type==IDN){
        in_stk.emplace_back("id");
      }
      else{
        //剩下的可以直接取名字
        in_stk.push_back(tkp->name);
      }
    }
    in_stk.emplace_back("$");
  }
private:
  vector<int> st_stk;//状态栈
  vector<string> sgn_stk;//符号栈
  vector<string> in_stk;//输入栈，栈底是$符号
  Grammer& G;//引用一个文法
public:
  vector<int> which_prod;//记录规约过程中使用的产生式，最左规约对应最右派生，后面要用它构建分析树结构

};
