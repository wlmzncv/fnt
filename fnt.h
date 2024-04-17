#pragma once
#ifndef FNT_H
#define FNT_H

#include <string>
#include <sstream>
#include <bitset>

namespace Yesane {
    class fnt;
    fnt operator+(const fnt &v1, const fnt &v2);
    fnt operator-(const fnt &v1, const fnt &v2);
    fnt operator*(const fnt &v1, const fnt &v2);
    fnt operator/(const fnt &v1, const fnt &v2);
    bool operator==(const fnt &v1, const fnt &v2);
    bool operator!=(const fnt &v1, const fnt &v2);
    bool operator>(const fnt &v1, const fnt &v2);
    bool operator<(const fnt &v1, const fnt &v2);
    bool operator>=(const fnt &v1, const fnt &v2);
    bool operator<=(const fnt &v1, const fnt &v2);
    std::ostream& operator<<(std::ostream& os, const fnt& v);
    std::istream& operator>>(std::istream& is, fnt& v);
}

class Yesane::fnt {

    friend std::ostream &operator<<(std::ostream &os, const fnt &v);
    friend std::istream &operator>>(std::istream &is, fnt &v);
    friend bool operator==(const fnt& v1, const fnt& v2);
    friend bool operator!=(const fnt& v1, const fnt& v2);
    friend bool operator>(const fnt& v1, const fnt& v2);
    friend bool operator<(const fnt& v1, const fnt& v2);
    friend bool operator>=(const fnt& v1, const fnt& v2);
    friend bool operator<=(const fnt& v1, const fnt& v2);

    using bits = std::bitset<32>; //32也是realLength的值

private:
    bool sign = false; //符号位 true时为负数
    unsigned char exp = 0; //指数位 0~254
    unsigned int real = 0; //有效数字位
    bits bitvec; //仅供类内函数临时存储real的二进制表示时使用 不可直接使用bitvec的值

public:
    static const int realLength = 32; //real所占的bit数
    static const int maxE = 255; //exp的最大值 不可用
    static const long long maxR = 4294967295; //real的最大值 可用
    /*
    记录上一步操作结果
    0-无精度损失
    1-有精度损失
    2-产生了NaN
    */
    std::uint32_t resultCode = 0;

    fnt(){}
    fnt(const fnt& v):sign(v.sign),exp(v.exp),real(v.real){}
    fnt(unsigned int x):real(x){}
    fnt(int x){
        if(x<0){
            sign = true;
            x = -x;
        }
        real = x;
    }
    fnt(bool a, unsigned char b, unsigned int c):sign(a),exp(b),real(c){}
    fnt(unsigned long long x);
    fnt(long long x);
    fnt &operator=(const fnt &v);

    std::string toS() const {
        if(exp == maxE){
            return "NaN";
        }
        std::stringstream s;
        if(sign){
            s << "-";
        }
        s << real;
        if(exp!=0){
            s << "b" << +exp; //对char使用+使得当作数字输出
        }
        return s.str();
    }
    //只有real的最高位为1时 才将低位的0用exp表示
    void shrinkReal();

    fnt &operator+=(const fnt &v);
    fnt &operator-=(const fnt &v);
    fnt &operator*=(const fnt &v);
    // 运算对象指数值相同将便利计算过程
    fnt &operator/=(const fnt &v);

    //将unsigned long long转为相应的real值和exp值
    //返回true没有精度损失
    static bool ullTofnt(unsigned long long x,
                        unsigned int& real,
                        unsigned char& exp);

   private:
    // 将exp尽量向z靠拢 若达到z则返回true 否则返回false
    // compulsory为true时会强制增加exp到指定值(但有可能导致real变为0
    // 或至少可能会损失精度) 若z的值比exp小 compulsory不应起作用
    bool normalize(int z, bool compulsory = false);
    // 在有可能损失精度的操作后记录是否真的损失了精度
    bool remainAccur = true;
};

#endif
