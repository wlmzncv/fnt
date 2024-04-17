#include "fnt.h"

namespace Yesane {
    fnt::fnt(unsigned long long x){
        if (x <= maxR){
            real = x;
        }else {
            remainAccur = remainAccur && ullTofnt(x, real, exp);
        }
        resultCode = !remainAccur;
    }
    fnt::fnt(long long x)
    {
        if (x < 0)
        {
            sign = true;
            x = -x;
        }
        if (x <= maxR) {
            real = x;
        }else{
            remainAccur = remainAccur && ullTofnt(x, real, exp);
        }
        resultCode = !remainAccur;
    }
    fnt &fnt::operator=(const fnt &v)
    {
        sign = v.sign;
        exp = v.exp;
        real = v.real;
        return *this;
    }
    bool fnt::ullTofnt(unsigned long long x,
                       unsigned int& real,
                       unsigned char& exp) {
        if(x <= maxR){
            real = x;
            exp = 0;
            return true;
        }
        std::bitset<64> bs(x);
        std::bitset<32> bss;
        int i = 63;
        for (; i > 31; --i) {
            if (bs[i])  // 将bs[i](为1)开始的32bit复制到bss中
            {
                for (int j = 0; j < 32; ++j) {
                    bss[31 - j] = bs[i - j];
                }
                break;
            }
        }
        // 末尾有一些位不能复制到real中 可能有精度损失
        real = bss.to_ulong();
        exp = i - 32 + 1;
        // 确定是否有精度损失
        for (i -= 32; i >= 0; --i) {
            if (bs[i]) {  // 有精度损失
                return false;
            }
        }
        if (i < 0) {  // 没有精度损失
            return true;
        }
    }
    void fnt::shrinkReal(){
        //适用于1111100000 即以1开头 以连续个0结尾的real值
        bitvec = bits(real);
        if(bitvec[realLength-1]){ //real的高位存放在比特数组的高位
            while(exp < maxE-1 && !bitvec[0]){ //右移时确保不把1去掉 这样不会损失精度
                real >>= 1;
                bitvec >>= 1;
                ++exp;
            }
        }
    }
    bool fnt::normalize(int z, bool compulsory){
        if(0<=z && z<=maxE-1){ //z值应该属于0~254
            if(z==exp)
                return true;
            bitvec = bits(real);
            if(z>exp){ //要增大exp
                while(exp < z && (!bitvec[0] || compulsory)){ //末尾不为1 除非compulsory为true(此时一定能达到z值 但可能会造成real值全为0)
                    real >>= 1;
                    bitvec >>= 1;
                    ++exp;
                }
            }else if(z<exp){ //要减小exp
                while(z < exp && !bitvec[realLength-1]){ //real的最高位不能为1 否则会造成极大的数值扭曲 可能不会达到z值
                    real <<= 1;
                    bitvec <<= 1;
                    --exp;
                }
            }
            if (exp == z)
            {
                return true;
            }
            else
                return false;
        }else {
            return false;
        }
    }
    fnt& fnt::operator+=(const fnt& v) {
        remainAccur = true;
        resultCode = 0;
        // 检查是否含NaN
        if (exp == maxE || v.exp == maxE) {
            exp = maxE;
            real = 0;
            return *this;
        }
        // 检查是否含0
        if (real == 0) {
            sign = v.sign;
            exp = v.exp;
            real = v.real;
            return *this;
        }
        if (v.real == 0) {
            return *this;
        }
        fnt t(v);
        if (exp != v.exp) {  // 两者指数不同 先尽量降低大的指数 再强制提升小的指数
            if (exp > t.exp) {
                if (!normalize(t.exp))  // exp降低后仍不满足两者相等
                    remainAccur = remainAccur && t.normalize(exp, true);
            } else if (exp < t.exp) {
                if (!t.normalize(exp))  // t.exp降低后仍不满足两者相等
                    remainAccur = remainAccur && normalize(t.exp, true);
            }
        }
        // 此时应有exp==t.exp
        if (exp == t.exp) {
            if (sign != t.sign)  // 两者异号
            {
                if (real > t.real) {  // 符号保留
                    real -= t.real;   // real值减小
                    return *this;
                } else {  // 符号要转换
                    sign = t.sign;
                    real = t.real - real;  // 大的real减去小的
                    return *this;
                }
            } else {  // 两者同号
                unsigned long long r = real;
                // 此处注意 运算结果可能溢出时要先提升范围 再进行运算
                r += t.real;
                if (r <= maxR) {  // 有效数字足够 没有精度损失
                    real = r;
                    return *this;
                }
                unsigned char e;
                remainAccur = remainAccur && ullTofnt(r, real, e);  // 此处已设置好了real值
                int ee = e + exp;      // 最终的指数值
                if (ee < maxE) {  // 没有溢出 但有效数字不够 可能为近似结果
                    resultCode = !remainAccur;
                    exp = ee;
                    return *this;
                } else {  // 运算结果溢出
                    resultCode = 2;
                    exp = maxE;
                    real = 0;
                    return *this;
                }
            }
        } else {  // 运算出现了异常
            exp = maxE;
            resultCode = 2;
            return *this;
        }
    }
    fnt& fnt::operator-=(const fnt &v){
        fnt t(v);
        t.sign = !v.sign;
        return *this += t;
    }
    fnt& fnt::operator*=(const fnt &v){
        remainAccur = true;
        resultCode = 0;
        // 检查是否含NaN
        if (exp == maxE || v.exp == maxE){
            exp = maxE;
            real = 0;
            return *this;
        }
        // 检查是否含0
        if (real == 0 || v.real == 0) {
            exp = 0;
            real = 0;
            return *this;
        }
        if(sign != v.sign){
            sign = true;
        }else {
            sign = false;
        }
        // 提升变量范围 确保运算不溢出
        int e = exp;
        unsigned long long r = real;
        e += v.exp;
        r *= v.real;
        if(e>=maxE){ //运算溢出
            exp = maxE;
            real = 0;
            resultCode = 2;
            return *this;
        }else {
            if(r<=maxR){ //有效数字足够 没有精度损失
                exp = e;
                real = r;
                resultCode = 0;
                return *this;
            }
            //将r转为real和指数值ee
            unsigned char ee;
            remainAccur = remainAccur && ullTofnt(r, real, ee);
            e += ee; //最终的指数值
            if(e >= maxE){ //运算依然溢出
                exp = maxE;
                real = 0;
                resultCode = 2;
                return *this;
            }else { //没有溢出 但有效数字不够 可能为近似结果
                exp = e;
                resultCode = !remainAccur;
                return *this;
            }
        }
    }
    fnt& fnt::operator/=(const fnt &v){
        remainAccur = true;
        resultCode = 0;
        // 检查是否含NaN 或 除数为0
        if (exp == maxE || v.exp == maxE || v.real == 0) {
            exp = maxE;
            real = 0;
            return *this;
        }
        // 检查被除数是否为0
        if (real == 0) {
            exp = 0;
            real = 0;
            return *this;
        }
        if (sign != v.sign) {
            sign = true;
        } else {
            sign = false;
        }
        if(exp == v.exp){
            exp = 0;
            real /= v.real;
            return *this;
        }
        normalize(v.exp); //尝试改变exp值到与除数相同
        int c = exp - v.exp; //指数的差值
        if(c==0){
            exp = 0;
            real /= v.real;
            return *this;
        }else if(c<0){
            exp = 0;
            if (-c >= realLength) {  // 2的-c次方比real的最大值还大 结果为0
                real = 0;
                return *this;
            } else {
                unsigned int t = 1<<-c;
                real = real / v.real / t; //不要用乘法
                return *this;
            }
        }else { //仍有降低exp的空间
            unsigned long long lr = real;
            if(c <= 32){
                lr <<= c; //两者的指数此时相等了
                exp = 0;
                lr /= v.real;
                remainAccur = remainAccur && ullTofnt(lr, real, exp); //可能有精度损失
                resultCode = !remainAccur;
                return *this;
            }else { //直接提升real值到64位且最左端为1
                lr <<= 32;
                exp -= (unsigned char)32 + v.exp;
                lr /= v.real;
                unsigned char a;
                remainAccur = remainAccur && ullTofnt(lr, real, a); //可能有精度损失
                resultCode = !remainAccur;
                exp += a; //不会有溢出
                return *this;
            }
        }
    }
    fnt operator+(const fnt &v1, const fnt &v2){
        fnt t(v1);
        t += v2;
        return t;
    }
    fnt operator-(const fnt &v1, const fnt &v2){
        fnt t(v1);
        t -= v2;
        return t;
    }
    fnt operator*(const fnt &v1, const fnt &v2){
        fnt t(v1);
        t *= v2;
        return t;
    }
    fnt operator/(const fnt &v1, const fnt &v2){
        fnt t(v1);
        t /= v2;
        return t;
    }
    bool operator==(const fnt& v1, const fnt& v2){
        // 检查是否均为NaN
        if (v1.exp == v1.maxE && v2.exp == v1.maxE) {
            return true;
        }
        // 检查是否均为0
        if (v1.real == 0 && v2.real == 0) {
            return true;
        }
        if(v1.sign == v2.sign){
            if(v1.exp == v2.exp && v1.real == v2.real){
                return true;
            }else {
                unsigned int t;
                int s;
                if(v1.real > v2.real){
                    t = v1.real / v2.real;
                    s = v2.exp - v1.exp;
                }else {
                    t = v2.real / v1.real;
                    s = v1.exp - v2.exp;
                }
                //两者相等则应该2的s次方与t相同
                if((unsigned int)1<<s == t){
                    return true;
                }else {
                    return false;
                }
            }
        }else {
            return false;
        }
    }
    bool operator!=(const fnt& v1, const fnt& v2){
        return !(v1 == v2);
    }
    bool operator>(const fnt& v1, const fnt& v2){
        return !(v1 < v2) && !(v1 == v2);
    }
    bool operator<(const fnt& v1, const fnt& v2){
        //待补充
    }
    bool operator>=(const fnt& v1, const fnt& v2){
        return !(v1 < v2);
    }
    bool operator<=(const fnt& v1, const fnt& v2){
        return v1 < v2 || v1 == v2;
    }
    std::ostream &operator<<(std::ostream &os, const fnt &v){
        os << v.toS();
        return os;
    }
    std::istream &operator>>(std::istream &is, fnt &v){
        long long s;
        is >> s; //待补充 目前仅支持整型数字的读取
        v = fnt(s);
        return is;
    }
}
