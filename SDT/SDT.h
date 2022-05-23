//
// Created by 86182 on 2022/5/23.
//

#ifndef COMPILERS_PRINCIPLE_MASTER_SDT_H
#define COMPILERS_PRINCIPLE_MASTER_SDT_H

#include "../SLR/SLR.cpp"
/*S的属性类*/
struct AttrS{
    mutable string begin;
    string next;

};
/*C的属性*/
struct AttrC{
    string true_entry;
    string false_entry;

};
/*实现SDT的类*/
class SDT{
public:
    SDT(SLR& slr,ofstream& ofs):slr(slr),pvec(slr.G.pvec),ofs(ofs){
        prod_vec=slr.get_leftmostderivation();
        id_stack=slr.get_idstack();
        label_idx=0;
        var_idx=0;
    }
    void start();
    void actionP();
    void actionL();
    void actionS(const AttrS& S);
    void actionC(const AttrC& C);
    string actionE();
    string actionT();
    string actionF();
    //返回label号
    string newlabel(){
        return "L"+ to_string(label_idx++);
    }
    //返回临时变量
    string newtemp(){
        return "t"+ to_string(var_idx++);
    }
    //确定label位置
    void label(const string& L){
        ofs<<L<<':';
    }
private:
    SLR& slr;//引用一个SLR类
    ofstream& ofs;//引用一个输出流
    vector<Production>& pvec;
    vector<int> prod_vec;//最左派生产生式栈，尾部为栈顶
    vector<string> id_stack;//标识符栈，栈顶为当前id的词法值
    int code_line;//三地址码行号
    int label_idx;//label的序号
    int var_idx;
};

#endif //COMPILERS_PRINCIPLE_MASTER_SDT_H
