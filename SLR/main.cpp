#include "SLR.cpp"


int main(){
  ifstream in_text("../SLR/text.txt",ios::in),in_gram("../SLR/grammer.txt",ios::in);
  if(!in_text.is_open()){
      cout<<"open text.txt failed!"<<endl;
      return 1;
  }
  auto tkvec=getLexRes(in_text);
  Grammer G(in_gram);
  SLR slr(G);
  slr.make_input(tkvec);
  slr.start();
  slr.print();
}