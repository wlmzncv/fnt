#include "fnt.h"
#include <iostream>
#include <string>

using std::cout,std::string,std::endl;
using Yesane::fnt;

int main(){
    fnt a(false,32,7688); //7688*(2^32)=33,019,708,571,648 33万零197亿零857万1648
    fnt b(38886752190464); //38万8867亿5219万零464 即2373458996*(2^14)
    fnt c = a+b; //2194411034b15 即2194411034*(2^15)=71,906,460,762,112
    cout << "a="<<a<<"\nb="<<b<<"\na+b="<<c;
}