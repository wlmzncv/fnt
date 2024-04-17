#include "fnt.h"
#include <iostream>
#include <string>

using std::cout,std::string,std::endl;
using Yesane::fnt;

int main(){
    fnt a(false,32,7688); //7688*(2^32)=33,019,708,571,648 33万零197亿零857万1648
    if(a.resultCode==0){
        cout <<"fnt a(false,32,7688) 没有精度损失"<<endl; 
    }
    cout <<"a="<<a<<endl;
    fnt b(38886752190464);
    if(b.resultCode==0){
        cout <<"b(38886752190464) 没有精度损失"<<endl; 
    }
    cout <<"b="<<b<<endl;
    //38万8867亿5219万零464 即2373458996*(2^14) 可以无损转为fnt表示
    fnt c = a+b;
    if(c.resultCode==0){
        cout <<"fnt c = a+b; 没有精度损失"<<endl; 
    }
    cout <<"a+b="<<c<<endl;
    //c=2194411034b15 即2194411034*(2^15)=71,906,460,762,112
    //无精度损失 计算正确!
    fnt d = a*b; 
    //d=2227435639b59=1,284,029,224,165,396,818,880,888,832
    //正确值应为       1,284,029,224,627,015,780,686,364,672
    if(d.resultCode==1){
        cout << "fnt d = a*b; 出现精度损失"<<endl;
    }
    cout <<"a*b="<<d<<endl;
    fnt e(38886752190477); //2373458996b14=38,886,752,190,464
    if(e.resultCode==1){
        cout << "fnt e(38886752190477); 出现精度损失"<<endl;
    }
    cout <<"e="<<e<<endl;
    fnt f = e+38886752190464;//77,773,504,380,928
    if(f.resultCode==0){
        cout << "fnt f = e+38886752190464; 没有精度损失"<<endl;
    }
    cout <<"f="<<f<<endl;
    if(f == 2*b){
        cout << "f==2b"<<endl;
    }
}