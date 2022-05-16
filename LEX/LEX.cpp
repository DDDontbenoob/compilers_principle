//
// Created by 86182 on 2022/5/15.
//
#include "LEX.h"
unordered_set<string> keyid{"while","do","if","then","else","begin","end"};//关键词集合
bool isdigit(char c){return c>='0'&&c<='9';}
bool isalpha(char c){return c>='A'&&c<='z';}
bool ishexdig(char c){return isdigit(c)||c>='a'&&c<='f'||c>='A'&&c<='F';}
bool isilhex(char c){return c>='g'&&c<='z'||c>='G'&&c<='Z';}
bool isoctdig(char c){return c>='0'&&c<='7';}
bool isiloct(char c){return c=='8'||c=='9';}
Token* TokenScan(ifstream& in){
    if(in.peek()==EOF) return nullptr;
    char c;
    string name;
    //跳过空白部分
    while(c=in.get(),c==' '||c=='\n');
    if(isalpha(c)){//如果是字母
        while(isdigit(c)|| isalpha(c)){
            name.push_back(c);
            c=in.get();
        }
        in.unget();//回退给输入流
        //name是关键字
        if(keyid.count(name)){
            return new Token(NONE,name,"-");
        }
        //name是标识符
        return new Token(IDN,name,"IDN");
    }
    else if(isdigit(c)){//如果是数字
        name.push_back(c);
        if(c=='0'){
            //开头是0，可能是dec0、hex，oct
            c=in.get();
            if(c=='x'||c=='X'){
                //是十六进制数
                name.push_back(c);
                while(c=in.get(), ishexdig(c)) name.push_back(c);
                //通过最后一次读入的字符判断是否是非法十六进制
                if(isilhex(c)){
                    //非法十六进制数
                    name.push_back(c);
                    while(c=in.get(),isilhex(c)|| ishexdig(c)) name.push_back(c);
                    in.unget();
                    return new Token(ILHEX,name,"ILHEX");
                }
                //十六进制合法
                in.unget();
                return new Token(HEX,name,"HEX");
            }
            else if(isdigit(c)){
                //是八进制
                while(isoctdig(c)){
                    name.push_back(c);
                    c=in.get();
                }
                if(isiloct(c)) {
                    name.push_back(c);
                    while(c=in.get(), isdigit(c)) name.push_back(c);
                    in.unget();
                    return new Token(ILOCT,name,"ILOCT");
                }
                in.unget();
                return new Token(OCT,name,"OCT");
            }
            else {
                //是十进制0
                in.unget();
                return new Token(DEC,name,"DEC");
            }
        }
        else{
            //非0十进制数
            while(c=in.get(), isdigit(c)) name.push_back(c);
            in.unget();
            return new Token(DEC,name,"DEC");
        }
    }
    //为特殊符号,先是双字节符号
    if(in.peek()=='='||in.peek()=='>'){
        if(c=='>') return new Token(GE,">=","GE");
        else {
            c=in.get();
            if(c=='=') return new Token(LE,"<=","LE");
            else return new Token(NEQ,"<>","NEQ");
        }
    }
    else{
        switch(c){
            case '+': return new Token(ADD,"+","ADD");
            case '-': return new Token(SUB,"-","SUB");
            case '*': return new Token(MUL,"*","MUL");
            case '/': return new Token(DIV,"/","DIV");
            case '>': return new Token(GT,">","GT");
            case '<': return new Token(LT,"<","LT");
            case '=': return new Token(EQ,"=","EQ");
            case '(': return new Token(SLP,"(","SLP");
            case ')': return new Token(SRP,")","SRP");
            case ';': return new Token(SEMI,";","SEMI");
            default : return nullptr;
        }
    }
}
//返回词法分析结果
vector<Token*> getLexRes(ifstream& in){
    vector<Token*> tkvec;
    while(in.peek()!=EOF){
        tkvec.push_back(TokenScan(in));
    }
    return tkvec;
}
//打印词法分析结果
void print_lexres(const vector<Token*>& lexres){
    for(auto tkp:lexres)
        tkp->print();
}
//字符分割函数
vector<string> string_split(const string& str, const string& splits){
    if (str.empty())return {};
    vector<string> res;
    //在字符串末尾也加入分隔符，方便截取最后一段
    string strs = str + splits;
    size_t pos = strs.find(splits);
    int step = splits.size();

    // 若找不到内容则字符串搜索函数返回 npos
    while (pos != strs.npos)
    {
        string temp = strs.substr(0, pos);
        res.push_back(temp);
        //去掉已分割的字符串,在剩下的字符串中进行分割
        strs = strs.substr(pos + step, strs.size());
        pos = strs.find(splits);
    }
    return res;
}
