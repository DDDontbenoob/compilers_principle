#include "SLR.cpp"


int main(){
  ifstream in_text("../SLR/text.txt",ios::in),in_gram("../SLR/grammer.txt",ios::in);
  if(!in_text.is_open()){
      cout<<"open text.txt failed!"<<endl;
      return 1;
  }
  auto tkvec=getLexRes(in_text);
  print_lexres(tkvec);
  Grammer G(in_gram);
  ofstream os1("../SLR/action_table.txt",ios::out);
  ofstream os2("../SLR/first_follow.txt",ios::trunc);//追加方式打开
  G.print_follow(os2);
  G.print_first(os2);
  G.print_actiontable(os1);
  SLR slr(G);
  slr.make_input(tkvec);
  slr.start();
  slr.print();
}