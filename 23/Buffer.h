#pragma once
#include <string>
#include <iostream>

class Buffer{
private:
    std::string buf_;

public:
    Buffer();
    ~Buffer();

    void append(const char *data, size_t size); //把数据追加到buf_中
    size_t size();                              //返回buf_的大小
    const char *data();                         //返回buf_的首地址
    void erase(size_t pos, size_t nn);        //擦除从某个特定位置开始的一串字符
    void clear();                                //清空buf_     
};